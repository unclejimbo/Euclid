#include <Eigen/Core>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Math/Vector.h>
#include <Euclid/Util/Assert.h>

namespace Euclid
{

template<typename Mesh>
void GeodesicsInHeat<Mesh>::build(const Mesh& mesh,
                                  float scale,
                                  FT resolution,
                                  const SpMat* cot_mat,
                                  const SpMat* mass_mat)
{
    this->mesh = &mesh;

    // Mesh resolution
    if (resolution != 0) {
        this->resolution = resolution;
    }
    else {
        this->resolution = 0;
        for (const auto& e : edges(mesh)) {
            this->resolution += edge_length(e, mesh);
        }
        this->resolution /= num_edges(mesh);
    }

    // Construct the equations
    FT diffuse_time =
        this->resolution * this->resolution * static_cast<FT>(scale);
    if (cot_mat) {
        this->cot_mat.reset(cot_mat);
    }
    else {
        this->cot_mat.reset(new SpMat(cotangent_matrix(mesh)), true);
    }
    if (mass_mat) {
        this->mass_mat.reset(mass_mat);
    }
    else {
        this->mass_mat.reset(new SpMat(mass_matrix(mesh)), true);
    }
    SpMat heat_mat = *this->mass_mat + diffuse_time * *this->cot_mat;

    // Factorize the heat matrix
    this->heat_solver.compute(heat_mat);
    if (this->heat_solver.info() != Eigen::Success) {
        throw std::runtime_error("Unable to factor the heat equation.");
    }

    // Factorize the laplacian matrix
    this->poisson_solver.compute(-*this->cot_mat);
    if (this->poisson_solver.info() != Eigen::Success) {
        throw std::runtime_error("Unable to factor the poisson equation.");
    }
}

template<typename Mesh>
void GeodesicsInHeat<Mesh>::scale(float scale)
{
    FT diffuse_time = this->resolution * this->resolution * scale;
    SpMat heat_mat = *this->mass_mat + diffuse_time * *this->cot_mat;

    // Factorize the heat matrix
    this->heat_solver.compute(heat_mat);
    if (this->heat_solver.info() != Eigen::Success) {
        throw std::runtime_error("Unable to factor the heat equation.");
    }

    // Factorize the laplacian matrix
    this->poisson_solver.compute(-*this->cot_mat);
    if (this->poisson_solver.info() != Eigen::Success) {
        throw std::runtime_error("Unable to factor the poisson equation.");
    }
}

template<typename Mesh>
template<typename T>
void GeodesicsInHeat<Mesh>::compute(
    const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
    std::vector<T>& geodesics)
{
    auto vpmap = get(boost::vertex_point, *this->mesh);
    auto vimap = get(boost::vertex_index, *this->mesh);
    auto fimap = get(boost::face_index, *this->mesh);
    const auto zero = static_cast<FT>(0.0);
    const auto half = static_cast<FT>(0.5);
    using Mat = Eigen::Matrix<FT, Eigen::Dynamic, 1>;

    // Solve the heat equation
    Mat delta = Mat::Zero(num_vertices(*this->mesh));
    delta(get(vimap, v), 0) = 1.0f;
    Mat heat = this->heat_solver.solve(delta);
    if (this->heat_solver.info() != Eigen::Success) {
        throw std::runtime_error("Unable to solve the heat equation.");
    }

    // Evaluate the normalized gradient field of the diffusion
    size_t fidx = 0;
    std::vector<Vector_3> gradients(num_faces(*this->mesh));
    for (const auto& f : faces(*this->mesh)) {
        auto fn = face_normal(f, *this->mesh);
        auto fa = face_area(f, *this->mesh);
        auto he = halfedge(f, *this->mesh);
        auto v0 = source(he, *this->mesh);
        auto v1 = target(he, *this->mesh);
        auto v2 = target(next(he, *this->mesh), *this->mesh);
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
    Mat divs(num_vertices(*this->mesh));
    for (const auto& v : vertices(*this->mesh)) {
        FT divergence = zero;
        auto p = get(vpmap, v);
        for (const auto& he : CGAL::halfedges_around_target(v, *this->mesh)) {
            auto f = face(he, *this->mesh);
            auto g = gradients[get(fimap, f)];
            auto vi = source(he, *this->mesh);
            auto vj = target(next(he, *this->mesh), *this->mesh);
            auto pi = get(vpmap, vi);
            auto pj = get(vpmap, vj);
            divergence += cotangent(p, pi, pj) * ((pj - p) * g) +
                          cotangent(p, pj, pi) * ((pi - p) * g);
        }
        divs(get(vimap, v), 0) = divergence * half;
    }

    // Solve the poisson equation
    Mat geod = this->poisson_solver.solve(divs);
    if (this->poisson_solver.info() != Eigen::Success) {
        throw std::runtime_error("Unable to solve the poisson equation.");
    }

    geodesics.resize(num_vertices(*this->mesh));
    for (size_t i = 0; i < num_vertices(*this->mesh); ++i) {
        geodesics[i] = static_cast<T>(geod(i, 0) - geod(get(vimap, v), 0));
        EASSERT(geodesics[i] >= 0.0);
    }
}

} // namespace Euclid
