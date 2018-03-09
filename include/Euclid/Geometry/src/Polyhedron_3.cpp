#include <exception>
#include <string>

#include <CGAL/Polyhedron_incremental_builder_3.h>

namespace Euclid
{

template<int N,
         typename Kernel,
         typename PT,
         typename IT,
         typename Enable = std::enable_if_t<std::is_arithmetic_v<PT>, void>>
void build_polyhedron_3(CGAL::Polyhedron_3<Kernel>& mesh,
                        const std::vector<PT>& positions,
                        const std::vector<IT>& indices)
{
    if (positions.empty() || indices.empty()) {
        throw std::runtime_error("Input is empty");
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input indices is not divisble by ");
        err_str.append(std::to_string(N));
        throw std::runtime_error(err_str);
    }

    using HDS = typename CGAL::Polyhedron_3<Kernel>::HalfedgeDS;

    struct Builder : public CGAL::Modifier_base<HDS>
    {
    public:
        Builder(const std::vector<PT>& positions,
                const std::vector<IT>& indices)
            : _positions(positions), _indices(indices)
        {
        }

        void operator()(HDS& hds)
        {
            CGAL::Polyhedron_incremental_builder_3<HDS> builder(hds, true);

            builder.begin_surface(_positions.size() / 3, _indices.size() / N);

            for (size_t i = 0; i < _positions.size(); i += 3) {
                builder.add_vertex(typename HDS::Vertex::Point(
                    _positions[i], _positions[i + 1], _positions[i + 2]));
            }

            for (size_t i = 0; i < _indices.size(); i += N) {
                builder.begin_facet();
                for (int j = 0; j < N; ++j) {
                    builder.add_vertex_to_facet(_indices[i + j]);
                }
                builder.end_facet();
            }

            builder.end_surface();
        }

    private:
        const std::vector<PT>& _positions;
        const std::vector<IT>& _indices;
    };

    Builder builder(positions, indices);
    mesh.delegate(builder);
}

template<int N, typename Kernel, typename IT>
void build_polyhedron_3(CGAL::Polyhedron_3<Kernel>& mesh,
                        const std::vector<typename Kernel::Point_3>& points,
                        const std::vector<IT>& indices)
{
    if (points.empty() || indices.empty()) {
        throw std::runtime_error("Input is empty");
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input indices is not divisble by ");
        err_str.append(std::to_string(N));
        throw std::runtime_error(err_str);
    }

    using Point_3 = typename Kernel::Point_3;
    using HDS = typename CGAL::Polyhedron_3<Kernel>::HalfedgeDS;

    struct Builder : public CGAL::Modifier_base<HDS>
    {
    public:
        Builder(const std::vector<Point_3>& points,
                const std::vector<IT>& indices)
            : _points(points), _indices(indices)
        {
        }

        void operator()(HDS& hds)
        {
            CGAL::Polyhedron_incremental_builder_3<HDS> builder(hds, true);

            builder.begin_surface(_points.size(), _indices.size() / N);

            for (const auto& p : _points) {
                builder.add_vertex(p);
            }

            for (size_t i = 0; i < _indices.size(); i += N) {
                builder.begin_facet();
                for (int j = 0; j < N; ++j) {
                    builder.add_vertex_to_facet(_indices[i + j]);
                }
                builder.end_facet();
            }

            builder.end_surface();
        }

    private:
        const std::vector<Point_3>& _points;
        const std::vector<IT>& _indices;
    };

    Builder builder(points, indices);
    mesh.delegate(builder);
}

} // namespace Euclid
