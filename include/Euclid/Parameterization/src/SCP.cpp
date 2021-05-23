#include <Eigen/Core>
#include <Eigen/SparseCholesky>
#include <Euclid/MeshUtil/CGALMesh.h>
#include <Euclid/MeshUtil/EigenMesh.h>
#include <Euclid/Topology/MeshTopology.h>
#include <igl/cotmatrix.h>
#include <igl/vector_area_matrix.h>
#include <igl/repdiag.h>

namespace Euclid
{

namespace _impl
{

template<typename Mesh, typename T>
void boundary_matrix(const Mesh& mesh,
                     Eigen::SparseMatrix<T>& B,
                     Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>& E)
{
    using Triplet = Eigen::Triplet<T>;
    std::vector<Triplet> triplets;
    auto vimap = get(CGAL::vertex_index, mesh);

    auto nv = mesh.number_of_vertices();
    auto sz = nv * 2;
    B.resize(sz, sz);
    E.setZero(sz, 2);

    auto bnds = boundary_components(mesh);
    int nb = 0;
    for (auto h : bnds) {
        auto hi = h;
        do {
            auto v = target(hi, mesh);
            auto vi = get(vimap, v);
            hi = next(hi, mesh);
            triplets.emplace_back(vi, vi, 1.0);
            triplets.emplace_back(nv + vi, nv + vi, 1.0);
            E(vi, 0) = 1.0;
            E(nv + vi, 1) = 1.0;
            ++nb;
        } while (hi != h);
        E /= std::sqrt(nb + 0.0);
    }
    B.setFromTriplets(triplets.begin(), triplets.end());
    B.makeCompressed();
}

} // namespace _impl

template<typename Mesh, typename VertexUVMap>
void spectral_conformal_parameterization(Mesh& mesh, VertexUVMap uvm)
{
    using Point_2 = typename boost::property_traits<VertexUVMap>::value_type;
    using FT = typename CGAL::Kernel_traits<Point_2>::Kernel::FT;
    using SpMat = Eigen::SparseMatrix<FT>;
    using Mat = Eigen::Matrix<FT, Eigen::Dynamic, Eigen::Dynamic>;
    using Vec = Eigen::Matrix<FT, Eigen::Dynamic, 1>;
    constexpr const int MAX_ITERS = 30;

    // convert mesh
    std::vector<FT> positions;
    std::vector<int> indices;
    extract_mesh<3>(mesh, positions, indices);
    Mat V;
    Eigen::MatrixXi F;
    make_mesh<3>(V, F, positions, indices);

    // assemble matrices
    SpMat L, LD, A, LC, B;
    Mat E;
    igl::cotmatrix(V, F, L);
    igl::repdiag(L, 2, LD);
    igl::vector_area_matrix(F, A);
    LC = -LD + 2.0 * A;
    _impl::boundary_matrix(mesh, B, E);

    // inverse power iterations
    Vec fidler = Vec::Random(LC.rows());
    Eigen::SimplicialLDLT<SpMat> solver(LC);
    for (int i = 0; i < MAX_ITERS; ++i) {
        fidler = (B - E * E.transpose()) * fidler;
        fidler = solver.solve(fidler);
        fidler.normalize();
    }

    // assemble results
    auto nv = mesh.number_of_vertices();
    auto vimap = get(CGAL::vertex_index, mesh);
    for (auto v : vertices(mesh)) {
        auto i = get(vimap, v);
        auto x = fidler(i);
        auto y = fidler(nv + i);
        put(uvm, v, Point_2(x, y));
    }
}

template<typename Mesh>
template<typename VertexUVMap,
         typename VertexIndexMap,
         typename VertexParameterizedMap>
CGAL::Surface_mesh_parameterization::Error_code
SCP_parameterizer_3<Mesh>::parameterize(TriangleMesh& mesh,
                                        halfedge_descriptor bhd,
                                        VertexUVMap uvmap,
                                        VertexIndexMap vimap,
                                        VertexParameterizedMap vpmap)
{
    spectral_conformal_parameterization(mesh, uvmap);
    return CGAL::Surface_mesh_parameterization::OK;
}

} // namespace Euclid
