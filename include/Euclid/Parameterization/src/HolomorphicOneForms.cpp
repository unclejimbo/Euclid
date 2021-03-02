#include <map>
#include <queue>
#include <unordered_map>
#include <Eigen/Sparse>
#include <Euclid/Geometry/DEC.h>
#include <Euclid/Topology/HomologyGenerator.h>

namespace Euclid
{

namespace _impl
{

template<typename Mesh, typename T>
void build_closedness_matrix(const Mesh& mesh,
                             std::vector<Eigen::Triplet<T>>& triplets)
{
    auto fimap = get(boost::face_index, mesh);
    auto eimap = get(boost::edge_index, mesh);
    auto frange = faces(mesh);
    --frange.second; // the last face is redundant
    for (auto f : frange) {
        auto fidx = get(fimap, f);
        for (auto h : halfedges_around_face(halfedge(f, mesh), mesh)) {
            auto e = edge(h, mesh);
            triplets.emplace_back(
                fidx, get(eimap, e), halfedge_orientation(mesh, h));
        }
    }
}

template<typename Mesh, typename T>
void build_harmonicity_matrix(const Mesh& mesh,
                              std::vector<Eigen::Triplet<T>>& triplets)
{
    auto vimap = get(boost::vertex_index, mesh);
    auto eimap = get(boost::edge_index, mesh);
    auto offset = static_cast<int>(num_faces(mesh)) - 1;
    auto vrange = vertices(mesh);
    --vrange.second; // the last vertex is redundant
    for (auto v : vrange) {
        auto vidx = get(vimap, v);
        for (auto h : halfedges_around_target(v, mesh)) {
            auto e = edge(h, mesh);
            auto w = cotangent_weight(h, mesh);
            triplets.emplace_back(offset + vidx,
                                  get(eimap, e),
                                  halfedge_orientation(mesh, h) * w);
        }
    }
}

template<typename Mesh, typename T>
void build_duality_matrix(const Mesh& mesh,
                          const VertexChains<Mesh>& basis,
                          std::vector<Eigen::Triplet<T>>& triplets)
{
    auto eimap = get(boost::edge_index, mesh);
    auto offset = static_cast<int>(num_faces(mesh) + num_vertices(mesh)) - 2;
    for (size_t i = 0; i < basis.size(); ++i) {
        for (size_t j = 0; j < basis[i].size(); ++j) {
            auto vcurrent = basis[i][j];
            auto vnext = basis[i][(j + 1) % basis[i].size()];
            auto [h, hfound] = halfedge(vcurrent, vnext, mesh);
            auto e = edge(h, mesh);
            triplets.emplace_back(
                offset + i, get(eimap, e), halfedge_orientation(mesh, h));
        }
    }
}

template<typename Mesh,
         typename Derived,
         typename SEM,
         typename SVM,
         typename VertexUVMap,
         typename VertexParameterizedMap>
void integrate_holomorphic_one_forms(
    const Mesh& mesh,
    const Eigen::MatrixBase<Derived>& one_forms,
    CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh,
    VertexUVMap uvmap,
    VertexParameterizedMap vpmap)
{
    using Seam_mesh = CGAL::Seam_mesh<Mesh, SEM, SVM>;
    using Point_2 =
        typename CGAL::Kernel_traits<typename boost::property_traits<
            VertexUVMap>::value_type>::Kernel::Point_2;
    using Vector_2 =
        typename CGAL::Kernel_traits<typename boost::property_traits<
            VertexUVMap>::value_type>::Kernel::Vector_2;
    using vertex_descriptor =
        typename boost::graph_traits<Seam_mesh>::vertex_descriptor;
    auto underlying_eimap = get(boost::edge_index, mesh);

    std::queue<vertex_descriptor> queue;
    auto root = *(vertices(seam_mesh).first);
    queue.push(root);
    put(uvmap, root, Point_2(0, 0));
    put(vpmap, root, true);
    while (!queue.empty()) {
        auto v = queue.front();
        queue.pop();
        for (auto h : halfedges_around_source(v, seam_mesh)) {
            auto vv = target(h, seam_mesh);
            if (!get(vpmap, vv)) {
                auto underlying_e = edge(h.tmhd, mesh);
                auto e_idx = get(underlying_eimap, underlying_e);
                auto uv = get(uvmap, v);
                auto s = halfedge_orientation(mesh, h.tmhd);
                uv += s * Vector_2(one_forms(e_idx, 0), one_forms(e_idx, 1));
                put(uvmap, vv, uv);
                put(vpmap, vv, true);
                queue.push(vv);
            }
        }
    }
}

} // namespace _impl

template<typename Mesh, typename DerivedA, typename DerivedB>
void holomorphic_one_form_basis(const Mesh& mesh,
                                Eigen::MatrixBase<DerivedA>& primal,
                                Eigen::MatrixBase<DerivedB>& conjugate)
{
    auto basis = greedy_homology_generators(mesh);
    holomorphic_one_form_basis(mesh, basis, primal, conjugate);
}

template<typename Mesh, typename DerivedA, typename DerivedB>
void holomorphic_one_form_basis(const Mesh& mesh,
                                const VertexChains<Mesh>& homology_generators,
                                Eigen::MatrixBase<DerivedA>& primal,
                                Eigen::MatrixBase<DerivedB>& conjugate)
{
    using Scalar = typename DerivedA::Scalar;
    using Triplet = Eigen::Triplet<Scalar>;
    using Triplets = std::vector<Triplet>;
    using SpMat = Eigen::SparseMatrix<Scalar>;
    using Vec = Eigen::Matrix<Scalar, Eigen::Dynamic, 1>;
    using Mat = Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>;
    auto ng = homology_generators.size();
    auto nv = num_vertices(mesh);
    auto ne = num_edges(mesh);
    auto nf = num_faces(mesh);
    primal.derived().resize(ne, ng);

    // Primal one form basis
    Triplets triplets;
    _impl::build_closedness_matrix(mesh, triplets);
    _impl::build_harmonicity_matrix(mesh, triplets);
    _impl::build_duality_matrix(mesh, homology_generators, triplets);
    SpMat lhs(ne, ne);
    lhs.setFromTriplets(triplets.begin(), triplets.end());
    Eigen::SparseLU<SpMat> primal_solver;
    primal_solver.compute(lhs);
    for (size_t i = 0; i < ng; ++i) {
        Vec rhs = Vec::Zero(ne);
        rhs(nv + nf - 2 + i) = 1.0;
        primal.col(i) = primal_solver.solve(rhs);
    }

    // Conjugate one form basis
    Mat W(ng, ng);
    for (size_t i = 0; i < ng; ++i) {
        for (size_t j = 0; j < ng; ++j) {
            Vec wedge;
            wedge1(mesh, primal.col(i), primal.col(j), wedge);
            W(i, j) = wedge.sum();
        }
    }
    Mat B(ng, ng);
    for (size_t i = 0; i < ng; ++i) {
        for (size_t j = 0; j < ng; ++j) {
            Vec wedge;
            star_wedge1(mesh, primal.col(i), primal.col(j), wedge);
            B(i, j) = wedge.sum();
        }
    }
    Eigen::ColPivHouseholderQR<Mat> conjugate_solver;
    conjugate_solver.compute(W);
    Mat L = conjugate_solver.solve(B);
    conjugate = primal * L;
}

template<typename Mesh,
         typename Derived,
         typename SEM,
         typename SVM,
         typename VertexUVMap>
void integrate_holomorphic_one_forms(
    const Mesh& mesh,
    const Eigen::MatrixBase<Derived>& one_forms,
    CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh,
    VertexUVMap uvmap)
{
    using SM = CGAL::Seam_mesh<Mesh, SEM, SVM>;
    using vd = typename boost::graph_traits<SM>::vertex_descriptor;
    using PM = std::map<vd, bool>;
    PM parameterized;
    boost::associative_property_map<PM> vpmap(parameterized);
    _impl::integrate_holomorphic_one_forms(
        mesh, one_forms, seam_mesh, uvmap, vpmap);
}

template<typename Mesh, typename SEM, typename SVM>
Holomorphic_one_forms_parameterizer3<Mesh, SEM, SVM>::
    Holomorphic_one_forms_parameterizer3(const Mesh& mesh)
    : _underlying_mesh(mesh)
{
    holomorphic_one_form_basis(mesh, _primal, _conjugate);
    Eigen::VectorXd coeffs = Eigen::VectorXd::Zero(_primal.cols());
    coeffs(0) = 1.0;
    set_coeffs(coeffs);
}

template<typename Mesh, typename SEM, typename SVM>
Holomorphic_one_forms_parameterizer3<Mesh, SEM, SVM>::
    Holomorphic_one_forms_parameterizer3(
        const Mesh& mesh,
        const VertexChains<Mesh>& homology_generators)
    : _underlying_mesh(mesh)
{
    holomorphic_one_form_basis(mesh, homology_generators, _primal, _conjugate);
    Eigen::VectorXd coeffs = Eigen::VectorXd::Zero(_primal.cols());
    coeffs(0) = 1.0;
    set_coeffs(coeffs);
}

template<typename Mesh, typename SEM, typename SVM>
template<typename Derived>
void Holomorphic_one_forms_parameterizer3<Mesh, SEM, SVM>::set_coeffs(
    const Eigen::MatrixBase<Derived>& coeffs)
{
    _one_forms.resize(_primal.rows(), 2);
    _one_forms.col(0) = _primal * coeffs;
    _one_forms.col(1) = _conjugate * coeffs;
}

template<typename Mesh, typename SEM, typename SVM>
template<typename VertexUVMap,
         typename VertexIndexMap,
         typename VertexParameterizedMap>
CGAL::Surface_mesh_parameterization::Error_code
Holomorphic_one_forms_parameterizer3<Mesh, SEM, SVM>::parameterize(
    TriangleMesh& mesh,
    halfedge_descriptor,
    VertexUVMap uvmap,
    VertexIndexMap,
    VertexParameterizedMap vpmap)
{
    _impl::integrate_holomorphic_one_forms(
        _underlying_mesh, _one_forms, mesh, uvmap, vpmap);
    return CGAL::Surface_mesh_parameterization::OK;
}

} // namespace Euclid
