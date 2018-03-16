#include <exception>
#include <iostream>
#include <string>
#include <unordered_map>

#include <CGAL/boost/graph/properties.h>
#include <CGAL/boost/graph/Euler_operations.h>

namespace Euclid
{

template<int N, typename Mesh, typename FT, typename IT>
std::enable_if_t<std::is_arithmetic_v<FT>, void> make_mesh(
    Mesh& mesh,
    const std::vector<FT>& positions,
    const std::vector<IT>& indices)
{
    static_assert(N >= 3);
    if (positions.empty() || indices.empty()) {
        std::cerr << "Warning: positions or indices is empty, check your input"
                  << std::endl;
        return;
    }
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Input positions size is not divisible by 3");
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input indices size is not divisible by ");
        err_str.append(std::to_string(N));
        throw std::runtime_error(err_str);
    }
    using VPMap =
        typename boost::property_map<Mesh, CGAL::vertex_point_t>::type;
    using Point_3 = typename boost::property_traits<VPMap>::value_type;
    using vertex_descriptor =
        typename boost::graph_traits<Mesh>::vertex_descriptor;

    std::vector<vertex_descriptor> vds(positions.size() / 3);
    for (auto& v : vds) {
        v = add_vertex(mesh);
    }

    auto vpmap = get(CGAL::vertex_point, mesh);
    for (size_t i = 0; i < positions.size(); i += 3) {
        put(vpmap,
            vds[i / 3],
            Point_3(positions[i], positions[i + 1], positions[i + 2]));
    }

    std::vector<vertex_descriptor> face(N);
    for (size_t i = 0; i < indices.size(); i += N) {
        for (size_t j = 0; j < N; ++j) {
            face[j] = vds[indices[i + j]];
        }
        CGAL::Euler::add_face(face, mesh);
    }
}

template<int N, typename Mesh, typename Point_3, typename IT>
std::enable_if_t<!std::is_arithmetic_v<Point_3>, void> make_mesh(
    Mesh& mesh,
    const std::vector<Point_3>& points,
    const std::vector<IT>& indices)
{
    static_assert(N >= 3);
    if (points.empty() || indices.empty()) {
        std::cerr << "Warning: points or indices is empty, check your input"
                  << std::endl;
        return;
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input indices size is not divisible by ");
        err_str.append(std::to_string(N));
        throw std::runtime_error(err_str);
    }
    using vertex_descriptor =
        typename boost::graph_traits<Mesh>::vertex_descriptor;

    std::vector<vertex_descriptor> vds(points.size());
    for (auto& v : vds) {
        v = add_vertex(mesh);
    }

    auto vpmap = get(CGAL::vertex_point, mesh);
    for (size_t i = 0; i < points.size(); ++i) {
        put(vpmap, vds[i], points[i]);
    }

    std::vector<vertex_descriptor> face(N);
    for (size_t i = 0; i < indices.size(); i += N) {
        for (size_t j = 0; j < N; ++j) {
            face[j] = vds[indices[i + j]];
        }
        CGAL::Euler::add_face(face, mesh);
    }
}

template<int N, typename Mesh, typename FT, typename IT>
std::enable_if_t<std::is_arithmetic_v<FT>, void>
extract_mesh(Mesh& mesh, std::vector<FT>& positions, std::vector<IT>& indices)
{
    positions.clear();
    indices.clear();
    positions.reserve(num_vertices(mesh) * 3);
    indices.reserve(num_faces(mesh) * N);
    using vertex_descriptor =
        typename boost::graph_traits<Mesh>::vertex_descriptor;

    auto vpmap = get(CGAL::vertex_point, mesh);
    std::unordered_map<vertex_descriptor, int> vimap;
    int idx = 0;
    for (auto[beg, end] = vertices(mesh); beg != end; ++beg) {
        positions.push_back(vpmap[*beg].x());
        positions.push_back(vpmap[*beg].y());
        positions.push_back(vpmap[*beg].z());
        vimap[*beg] = idx++;
    }

    for (auto[fb, fe] = faces(mesh); fb != fe; ++fb) {
        int i = 0;
        for (auto[vb, ve] = vertices_around_face(halfedge(*fb, mesh), mesh);
             vb != ve;
             ++vb) {
            if (i++ >= N) {
                std::string err_str("The mesh is not a regular ");
                err_str.append(std::to_string(N));
                err_str.append("-mesh");
                throw std::runtime_error(err_str);
            }
            indices.push_back(vimap[*vb]);
        }
    }
}

template<int N, typename Mesh, typename Point_3, typename IT>
std::enable_if_t<!std::is_arithmetic_v<Point_3>, void>
extract_mesh(Mesh& mesh, std::vector<Point_3>& points, std::vector<IT>& indices)
{
    points.clear();
    indices.clear();
    points.reserve(num_vertices(mesh));
    indices.reserve(num_faces(mesh) * 3);
    using vertex_descriptor =
        typename boost::graph_traits<Mesh>::vertex_descriptor;

    auto vpmap = get(CGAL::vertex_point, mesh);
    std::unordered_map<vertex_descriptor, int> vimap;
    int idx = 0;
    for (auto[beg, end] = vertices(mesh); beg != end; ++beg) {
        points.push_back(vpmap[*beg]);
        vimap[*beg] = idx++;
    }

    for (auto[fb, fe] = faces(mesh); fb != fe; ++fb) {
        int i = 0;
        for (auto[vb, ve] = vertices_around_face(halfedge(*fb, mesh), mesh);
             vb != ve;
             ++vb) {
            if (i++ >= N) {
                std::string err_str("The mesh is not a regular ");
                err_str.append(std::to_string(N));
                err_str.append("-mesh");
                throw std::runtime_error(err_str);
            }
            indices.push_back(vimap[*vb]);
        }
    }
}

} // namespace Euclid
