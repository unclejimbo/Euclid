#include <unordered_map>
#include <vector>
#define _USE_MATH_DEFINES
#include <cmath>

#include <Euclid/Math/Vector.h>
#include <Euclid/Util/Assert.h>

namespace Euclid
{

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
    typename boost::property_map<Mesh, CGAL::vertex_point_t>::type>::
                                 value_type>::Kernel::Vector_3
vertex_normal(
    const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
    const Mesh& mesh,
    const VertexNormal& weight)
{
    using face_descriptor =
        typename boost::graph_traits<const Mesh>::face_descriptor;
    using VPMap =
        typename boost::property_map<Mesh, CGAL::vertex_point_t>::type;
    using Point_3 = typename boost::property_traits<VPMap>::value_type;
    using Vector_3 = typename CGAL::Kernel_traits<Point_3>::Kernel::Vector_3;
    using FNMap = std::unordered_map<face_descriptor, Vector_3>;
    using FaceNormalMap = boost::const_associative_property_map<FNMap>;

    auto vpmap = get(boost::vertex_point, mesh);
    FNMap fnmap;
    for (const auto& he : halfedges_around_source(v, mesh)) {
        auto he0 = he;
        auto he1 = next(he0, mesh);
        auto v0 = source(he0, mesh);
        auto v1 = target(he0, mesh);
        auto v2 = target(he1, mesh);
        auto f = face(he0, mesh);
        auto e0 = vpmap[v1] - vpmap[v0];
        auto e1 = vpmap[v2] - vpmap[v1];
        auto n = normalized(CGAL::cross_product(e0, e1));
        fnmap.insert({ f, n });
    }
    FaceNormalMap map(fnmap);

    return vertex_normal<Mesh, FaceNormalMap>(v, mesh, map, weight);
}

template<typename Mesh, typename FaceNormalMap>
typename boost::property_traits<FaceNormalMap>::value_type vertex_normal(
    const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
    const Mesh& mesh,
    const FaceNormalMap& fnmap,
    const VertexNormal& weight)
{
    using Vector_3 = typename boost::property_traits<FaceNormalMap>::value_type;

    auto vpmap = get(boost::vertex_point, mesh);
    Vector_3 normal(0.0, 0.0, 0.0);
    for (const auto& he : halfedges_around_source(v, mesh)) {
        auto f = face(he, mesh);
        auto fn = fnmap[f];

        if (weight == VertexNormal::uniform) { normal += fn; }
        else if (weight == VertexNormal::face_area) {
            auto area = face_area(f, mesh);
            normal += area * fn;
        }
        else { // incident_angle
            auto he_next = next(he, mesh);
            auto t = target(he, mesh);
            auto s1 = source(he, mesh);
            auto s2 = target(he_next, mesh);
            auto pt = vpmap[t];
            auto ps1 = vpmap[s1];
            auto ps2 = vpmap[s2];
            auto vec1 = normalized(ps1 - pt);
            auto vec2 = normalized(ps2 - pt);
            auto angle = std::acos(vec1 * vec2);
            normal += angle * fn;
        }
    }
    return Euclid::normalized(normal);
}

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
    typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                 value_type>::Kernel::FT
vertex_area(
    const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
    const Mesh& mesh,
    const VertexArea& method)
{
    using FT = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                value_type>::Kernel::FT;
    auto vpmap = get(boost::vertex_point, mesh);
    const FT one_third = 1.0 / 3.0;
    FT va = 0.0;
    if (method == VertexArea::barycentric) {
        for (const auto& he : halfedges_around_target(v, mesh)) {
            auto p1 = vpmap[source(he, mesh)];
            auto p2 = vpmap[target(he, mesh)];
            auto p3 = vpmap[target(next(he, mesh), mesh)];
            va += area(p1, p2, p3);
        }
        va *= one_third;
    }
    else if (method == VertexArea::voronoi) {
        for (auto he : halfedges_around_target(v, mesh)) {
            auto p1 = vpmap[source(he, mesh)];
            auto p2 = vpmap[target(he, mesh)];
            auto p3 = vpmap[target(next(he, mesh), mesh)];
            auto mid1 = CGAL::midpoint(p2, p1);
            auto mid2 = CGAL::midpoint(p2, p3);
            auto center = CGAL::circumcenter(p1, p2, p3);
            if (CGAL::angle(p2, p1, p3) == CGAL::OBTUSE) {
                va += area(mid1, p2, center) - area(mid2, center, p2);
            }
            else if (CGAL::angle(p2, p3, p1) == CGAL::OBTUSE) {
                va += area(mid2, center, p2) - area(mid1, p2, center);
            }
            else {
                va += area(mid1, p2, center) + area(mid2, center, p2);
            }
        }
    }
    else { // mixed
        for (auto he : halfedges_around_target(v, mesh)) {
            auto p1 = vpmap[source(he, mesh)];
            auto p2 = vpmap[target(he, mesh)];
            auto p3 = vpmap[target(next(he, mesh), mesh)];
            if (CGAL::angle(p1, p2, p3) == CGAL::OBTUSE) {
                va += area(p1, p2, p3) * 0.5;
            }
            else if (CGAL::angle(p3, p1, p2) == CGAL::OBTUSE ||
                     CGAL::angle(p1, p3, p2) == CGAL::OBTUSE) {
                va += area(p1, p2, p3) * 0.25;
            }
            else { // triangle is acute or right
                auto mid1 = CGAL::midpoint(p2, p1);
                auto mid2 = CGAL::midpoint(p2, p3);
                auto center = CGAL::circumcenter(p1, p2, p3);
                va += area(mid1, p2, center) + area(mid2, center, p2);
            }
        }
    }
    return va;
}

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
    typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                 value_type>::Kernel::FT
edge_length(
    const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
    const Mesh& mesh)
{
    auto vpmap = get(boost::vertex_point, mesh);
    auto p1 = vpmap[source(he, mesh)];
    auto p2 = vpmap[target(he, mesh)];
    return length(p2 - p1);
}

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
    typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                 value_type>::Kernel::FT
edge_length(const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
            const Mesh& mesh)
{
    auto he = halfedge(e, mesh);
    return edge_length(he, mesh);
}

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
    typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                 value_type>::Kernel::FT
squared_edge_length(
    const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
    const Mesh& mesh)
{
    auto vpmap = get(boost::vertex_point, mesh);
    auto p1 = vpmap[source(he, mesh)];
    auto p2 = vpmap[target(he, mesh)];
    return (p1 - p2).squared_length();
}

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
    typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                 value_type>::Kernel::FT
squared_edge_length(
    const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
    const Mesh& mesh)
{
    auto he = halfedge(e, mesh);
    return squared_edge_length(he, mesh);
}

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
    typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                 value_type>::Kernel::Vector_3
face_normal(const typename boost::graph_traits<const Mesh>::face_descriptor& f,
            const Mesh& mesh)
{
    using Vector_3 =
        typename CGAL::Kernel_traits<typename boost::property_traits<
            typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                         value_type>::Kernel::Vector_3;

    auto vpmap = get(CGAL::vertex_point, mesh);
    auto he = halfedge(f, mesh);
    auto p1 = vpmap[source(he, mesh)];
    auto p2 = vpmap[target(he, mesh)];
    auto p3 = vpmap[target(next(he, mesh), mesh)];

    Vector_3 result;
    if (CGAL::collinear(p1, p2, p3)) {
        EWARNING("Degenerate face, normal is set to zero vector");
        result = Vector_3(0.0, 0.0, 0.0);
    }
    else {
        result = normalized(CGAL::normal(p1, p2, p3));
    }
    return result;
}

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
    typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                 value_type>::Kernel::FT
face_area(const typename boost::graph_traits<const Mesh>::face_descriptor& f,
          const Mesh& mesh)
{
    auto vpmap = get(boost::vertex_point, mesh);
    auto he = halfedge(f, mesh);
    auto p1 = vpmap[source(he, mesh)];
    auto p2 = vpmap[target(he, mesh)];
    auto p3 = vpmap[target(next(he, mesh), mesh)];
    return area(p1, p2, p3);
}

// template<typename Mesh, typename VertexValueMap>
// CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
//     typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
//                                                 value_type>::Kernel>
// gradient(const typename boost::graph_traits<const Mesh>::face_descriptor& f,
//          const Mesh& mesh,
//          const VertexValueMap& vvmap)
// {
//     using Vector_3 = CGAL::Vector_3<
//         typename CGAL::Kernel_traits<typename boost::property_traits<
//             typename boost::property_map<Mesh,
//             boost::vertex_point_t>::type>::
//                                          value_type>::Kernel>;
//     auto vpmap = get(boost::vertex_point, mesh);
//     auto normal = face_normal(f, mesh);
//     Vector_3 grad(0.0, 0.0, 0.0);
//     for (const auto& he : halfedges_around_face(halfedge(f, mesh), mesh)) {
//         auto v = source(he, mesh);
//         auto e = vpmap[target(next(he, mesh), mesh)] - vpmap[target(he,
//         mesh)]; grad += vvmap[v] * CGAL::cross_product(normal, e);
//     }
//     grad *= 0.5 / face_area(f, mesh);
//     return grad;
// }

template<typename Mesh>
Eigen::SparseMatrix<
    typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                     value_type>::Kernel::FT>
laplacian_matrix(const Mesh& mesh, const Laplacian& method)
{
    using T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                               value_type>::Kernel::FT;
    using vertex_descriptor =
        typename boost::graph_traits<Mesh>::vertex_descriptor;
    auto vpmap = get(boost::vertex_point, mesh);
    const auto nv = num_vertices(mesh);
    Eigen::SparseMatrix<T> mat(nv, nv);
    std::vector<Eigen::Triplet<T>> values;
    values.reserve(nv * 7);

    std::unordered_map<vertex_descriptor, int> vimap;
    int cnt = 0;
    for (const auto& v : vertices(mesh)) {
        vimap.insert({ v, cnt++ });
    }

    for (const auto& vi : vertices(mesh)) {
        int i = vimap[vi];
        T row_sum = 0.0;
        for (const auto& he : halfedges_around_target(vi, mesh)) {
            auto vj = source(he, mesh);
            int j = vimap[vj];
            if (method == Laplacian::uniform) {
                values.emplace_back(i, j, static_cast<T>(1));
                row_sum += 1.0;
            }
            else { // cotangent
                auto va = target(next(he, mesh), mesh);
                auto vb = target(next(opposite(he, mesh), mesh), mesh);
                auto cota = cotangent(vpmap[vi], vpmap[va], vpmap[vj]);
                auto cotb = cotangent(vpmap[vi], vpmap[vb], vpmap[vj]);
                auto value = static_cast<T>((cota + cotb) * 0.5);
                values.emplace_back(i, j, value);
                row_sum += value;
            }
        }
        values.emplace_back(i, i, -row_sum);
    }

    mat.setFromTriplets(values.begin(), values.end());
    mat.makeCompressed();
    return mat;
}

template<typename Mesh>
Eigen::SparseMatrix<
    typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                     value_type>::Kernel::FT>
mass_matrix(const Mesh& mesh, const Mass& method)
{
    using T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                               value_type>::Kernel::FT;
    using vertex_descriptor =
        typename boost::graph_traits<Mesh>::vertex_descriptor;
    const auto nv = num_vertices(mesh);
    Eigen::SparseMatrix<T> mass(nv, nv);
    std::vector<Eigen::Triplet<T>> values;

    std::unordered_map<vertex_descriptor, int> vimap;
    if (method == Mass::fem) {
        int cnt = 0;
        for (const auto& v : vertices(mesh)) {
            vimap.insert({ v, cnt++ });
        }
    }

    int i = 0;
    for (const auto& v : vertices(mesh)) {
        T area = 0.0;
        if (method == Mass::fem) {
            T area_sum = 0.0;
            for (const auto& he : halfedges_around_target(v, mesh)) {
                auto vj = source(he, mesh);
                auto j = vimap[vj];
                auto a1 = face_area(face(he, mesh), mesh);
                auto a2 = face_area(face(opposite(he, mesh), mesh), mesh);
                area_sum += a1;
                values.emplace_back(i, j, (a1 + a2) / static_cast<T>(12));
            }
            values.emplace_back(i, i, area_sum / static_cast<T>(6));
        }
        else if (method == Mass::barycentric) {
            area = vertex_area(v, mesh, VertexArea::barycentric);
            values.emplace_back(i, i, area);
        }
        else { // voronoi
            area = vertex_area(v, mesh, VertexArea::voronoi);
            values.emplace_back(i, i, area);
        }
        ++i;
    }

    mass.setFromTriplets(values.begin(), values.end());
    mass.makeCompressed();
    return mass;
}

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
    typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                 value_type>::Kernel::FT
gaussian_curvature(
    const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
    const Mesh& mesh)
{
    using T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                               value_type>::Kernel::FT;
    auto vpmap = get(boost::vertex_point, mesh);

    T angle_defect = 2.0 * M_PI;
    for (const auto& he : halfedges_around_target(v, mesh)) {
        auto vp = source(he, mesh);
        auto vq = target(next(he, mesh), mesh);
        angle_defect -= std::acos(cosine(vpmap[vp], vpmap[v], vpmap[vq]));
    }
    return angle_defect / vertex_area(v, mesh);
}

} // namespace Euclid

#undef _USE_MATH_DEFINES
