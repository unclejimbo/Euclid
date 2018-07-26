#include <Eigen/Dense>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Math/Vector.h>
#include <Euclid/Util/Assert.h>

namespace Euclid
{

template<typename Mesh>
GeodesicsInHeat<Mesh>::GeodesicsInHeat(const Mesh& mesh, float scale)
{
    build(mesh, scale);
}

template<typename Mesh>
bool GeodesicsInHeat<Mesh>::build(const Mesh& mesh, float scale)
{
    const auto zero = static_cast<FT>(0.0);
    _mesh = &mesh;

    // Construct the equations
    _avg_spacing = zero;
    for (const auto& e : edges(mesh)) {
        _avg_spacing += edge_length(e, mesh);
    }
    _avg_spacing /= num_edges(mesh);
    FT diffuse_time = _avg_spacing * _avg_spacing * static_cast<FT>(scale);
    _cot_mat = laplacian_matrix(mesh);
    _mass_mat = mass_matrix(mesh);
    SpMat heat_mat = _mass_mat - diffuse_time * _cot_mat;

    // Factorize the heat matrix
    _heat_solver.compute(heat_mat);
    if (_heat_solver.info() != Eigen::Success) {
        std::cerr << "Unable to solve the heat flow" << std::endl;
        return false;
    }

    // Factorize the laplacian matrix
    _poisson_solver.compute(_cot_mat);
    if (_poisson_solver.info() != Eigen::Success) {
        std::cerr << "Unable to solve the poisson equation" << std::endl;
        return false;
    }

    return true;
}

template<typename Mesh>
bool GeodesicsInHeat<Mesh>::scale(float scale)
{
    FT diffuse_time = _avg_spacing * _avg_spacing * scale;
    SpMat heat_mat = _mass_mat - diffuse_time * _cot_mat;

    // Factorize the heat matrix
    _heat_solver.compute(heat_mat);
    if (_heat_solver.info() != Eigen::Success) {
        std::cerr << "Unable to solve the heat flow" << std::endl;
        return false;
    }

    // Factorize the laplacian matrix
    _poisson_solver.compute(_cot_mat);
    if (_poisson_solver.info() != Eigen::Success) {
        std::cerr << "Unable to solve the poisson equation" << std::endl;
        return false;
    }

    return true;
}

template<typename Mesh>
template<typename T>
bool GeodesicsInHeat<Mesh>::compute(
    const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
    std::vector<T>& geodesics)
{
    auto vpmap = get(boost::vertex_point, *_mesh);
    auto vimap = get(boost::vertex_index, *_mesh);
    auto fimap = get(boost::face_index, *_mesh);
    const auto zero = static_cast<FT>(0.0);
    const auto half = static_cast<FT>(0.5);
    const auto one = static_cast<FT>(1.0);
    using Mat = Eigen::Matrix<FT, Eigen::Dynamic, 1>;

    // Solve the heat equation
    Mat delta = Mat::Zero(num_vertices(*_mesh));
    delta(get(vimap, v), 0) = 1.0f;
    Mat heat = _heat_solver.solve(delta);
    if (_heat_solver.info() != Eigen::Success) {
        std::cerr << "Unable to solve the heat flow" << std::endl;
        return false;
    }

    // Evaluate the normalized gradient field of the diffusion
    size_t fidx = 0;
    std::vector<Vector_3> gradients(num_faces(*_mesh));
    for (const auto& f : faces(*_mesh)) {
        auto fn = face_normal(f, *_mesh);
        auto fa = face_area(f, *_mesh);
        auto he = halfedge(f, *_mesh);
        auto v0 = source(he, *_mesh);
        auto v1 = target(he, *_mesh);
        auto v2 = target(next(he, *_mesh), *_mesh);
        auto e0 = get(vpmap, v2) - get(vpmap, v1);
        auto e1 = get(vpmap, v0) - get(vpmap, v2);
        auto e2 = get(vpmap, v1) - get(vpmap, v0);
        auto g = half / fa *
                 (heat[get(vimap, v0)] * CGAL::cross_product(fn, e0) +
                  heat[get(vimap, v1)] * CGAL::cross_product(fn, e1) +
                  heat[get(vimap, v2)] * CGAL::cross_product(fn, e2));
        gradients[fidx++] = -normalized(g);
    }

    // Compute the integrated divergence of gradients
    Mat divs(num_vertices(*_mesh));
    for (const auto& v : vertices(*_mesh)) {
        FT divergence = zero;
        auto p = get(vpmap, v);
        for (const auto& he : halfedges_around_target(v, *_mesh)) {
            auto f = face(he, *_mesh);
            auto g = gradients[get(fimap, f)];
            auto vi = source(he, *_mesh);
            auto vj = target(next(he, *_mesh), *_mesh);
            auto pi = get(vpmap, vi);
            auto pj = get(vpmap, vj);
            divergence += cotangent(p, pi, pj) * ((pj - p) * g) +
                          cotangent(p, pj, pi) * ((pi - p) * g);
        }
        divs(get(vimap, v), 0) = divergence * half;
    }

    // Solve the poisson equation
    Mat geod = _poisson_solver.solve(divs);
    if (_poisson_solver.info() != Eigen::Success) {
        std::cerr << "Unable to solve the poisson equation" << std::endl;
        return false;
    }

    geodesics.resize(num_vertices(*_mesh));
    for (size_t i = 0; i < num_vertices(*_mesh); ++i) {
        geodesics[i] = static_cast<T>(geod(i, 0) - geod(get(vimap, v), 0));
        EASSERT(geodesics[i] >= 0.0);
    }
    return true;
}

} // namespace Euclid
