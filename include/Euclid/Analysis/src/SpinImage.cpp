#include <cmath>
#include <tuple>
#include <unordered_map>

#include <boost/math/constants/constants.hpp>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Math/Vector.h>
#include <Euclid/Math/Transformation.h>

namespace Euclid
{

template<typename Mesh>
SpinImage<Mesh>::~SpinImage()
{
    if (!_is_shared) { delete this->vnormals; }
}

template<typename Mesh>
void SpinImage<Mesh>::build(const Mesh& mesh,
                            const std::vector<Vector_3>* vnormals,
                            FT resolution)
{
    this->mesh = &mesh;

    if (vnormals != nullptr) {
        this->vnormals = vnormals;
        _is_shared = true;
    }
    else {
        using Face = boost::graph_traits<Mesh>::face_descriptor;
        using FNMap = std::unordered_map<Face, Vector_3>;
        FNMap fnmap;
        fnmap.reserve(num_faces(mesh));
        for (const auto& f : faces(mesh)) {
            fnmap.insert({ f, Euclid::face_normal(f, mesh) });
        }
        auto fn_map = boost::make_assoc_property_map(fnmap);

        std::vector<Vector_3>* vertex_normals = new std::vector<Vector_3>;
        vertex_normals->reserve(num_vertices(mesh));
        for (const auto& v : vertices(mesh)) {
            vertex_normals->push_back(Euclid::vertex_normal(v, mesh, fn_map));
        }
        this->vnormals = vertex_normals;
        _is_shared = false;
    }

    if (resolution != 0.0) { this->resolution = resolution; }
    else {
        this->resolution = 0.0;
        for (const auto& e : edges(mesh)) {
            this->resolution += edge_length(e, mesh);
        }
        this->resolution /= static_cast<FT>(num_edges(mesh));
    }
}

template<typename Mesh>
template<typename T>
void SpinImage<Mesh>::compute(const Vertex& v,
                              Eigen::Array<T, Eigen::Dynamic, 1>& spin_img,
                              float bin_size,
                              int image_width,
                              float support_angle)
{
    auto vpmap = get(boost::vertex_point, *this->mesh);
    auto vimap = get(boost::vertex_index, *this->mesh);
    auto bin_width = this->resolution * static_cast<FT>(bin_size);
    spin_img = Eigen::Array<T, Eigen::Dynamic, 1>::Zero((image_width + 1) *
                                                        (image_width + 1));

    // Transform the coordinate system so that vi is at origin and the vertex
    // normal points in the y axis, while the x and z axes are arbitrary
    auto pi = get(vpmap, v);
    auto ni = (*this->vnormals)[get(vimap, v)];
    CGAL::Plane_3<Kernel> plane(pi, ni);
    auto tangent = normalized(plane.base1());
    auto transform =
        transform_from_world_coord<Kernel>(pi, pi + tangent, pi + ni);

    // Find all vertices that lie in the support and compute the spin image
    for (const auto& vj : vertices(*this->mesh)) {
        if (ni * (*this->vnormals)[get(vimap, vj)] <=
            std::cos(support_angle * boost::math::float_constants::degree)) {
            continue;
        }

        auto pj = transform(get(vpmap, vj));

        auto alpha = std::sqrt(pj.x() * pj.x() + pj.z() * pj.z()) / bin_width;
        auto col = static_cast<int>(std::floor(alpha));
        if (col >= image_width) { continue; }
        alpha -= col;

        auto beta = pj.y() / bin_width;
        auto beta_max = image_width * 0.5f;
        auto row = static_cast<int>(std::floor(beta_max - beta));
        if (row >= image_width || row < 0) { continue; }
        beta = beta_max - beta - row;

        // Bilinear interpolation
        spin_img(row * image_width + col) += (1.0f - alpha) * (1.0f - beta);
        spin_img(row * image_width + col + 1) += alpha * (1.0f - beta);
        spin_img((row + 1) * image_width + col) += (1.0f - alpha) * beta;
        spin_img((row + 1) * image_width + col + 1) += alpha * beta;
    }
}

template<typename Mesh>
template<typename T>
void SpinImage<Mesh>::compute(
    Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic>& spin_img,
    float bin_size,
    int image_width,
    float support_angle)
{
    auto vpmap = get(boost::vertex_point, *this->mesh);
    auto vimap = get(boost::vertex_index, *this->mesh);
    auto bin_width = this->resolution * static_cast<FT>(bin_size);
    spin_img = Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic>::Zero(
        (image_width + 1) * (image_width + 1), num_vertices(*this->mesh));

    for (const auto& v : vertices(*this->mesh)) {
        auto idx = get(vimap, v);

        // Transform the coordinate system so that vi is at origin and the
        // vertex normal points in the y axis, while the x and z axes are
        // arbitrary
        auto pi = get(vpmap, v);
        auto ni = (*this->vnormals)[idx];
        CGAL::Plane_3<Kernel> plane(pi, ni);
        auto tangent = normalized(plane.base1());
        auto transform =
            transform_from_world_coord<Kernel>(pi, pi + tangent, pi + ni);

        // Find all vertices that lie in the support and compute the spin image
        for (const auto& vj : vertices(*this->mesh)) {
            if (ni * (*this->vnormals)[get(vimap, vj)] <=
                std::cos(support_angle *
                         boost::math::float_constants::degree)) {
                continue;
            }

            auto pj = transform(get(vpmap, vj));

            auto alpha =
                std::sqrt(pj.x() * pj.x() + pj.z() * pj.z()) / bin_width;
            auto col = static_cast<int>(std::floor(alpha));
            if (col >= image_width) { continue; }
            alpha -= col;

            auto beta = pj.y() / bin_width;
            auto beta_max = image_width * 0.5f;
            auto row = static_cast<int>(std::floor(beta_max - beta));
            if (row >= image_width || row < 0) { continue; }
            beta = beta_max - beta - row;

            // Bilinear interpolation
            spin_img(row * image_width + col, idx) +=
                (1.0f - alpha) * (1.0f - beta);
            spin_img(row * image_width + col + 1, idx) += alpha * (1.0f - beta);
            spin_img((row + 1) * image_width + col, idx) +=
                (1.0f - alpha) * beta;
            spin_img((row + 1) * image_width + col + 1, idx) += alpha * beta;
        }
    }
}

} // namespace Euclid
