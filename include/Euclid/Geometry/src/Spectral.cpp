#include <string>

#include <CGAL/boost/graph/properties.h>
#include <Eigen/SparseCore>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Util/Assert.h>
#include <Spectra/MatOp/SparseSymMatProd.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/MatOp/SymShiftInvert.h>
#include <Spectra/SymEigsShiftSolver.h>
#include <Spectra/SymGEigsShiftSolver.h>

namespace Euclid
{

namespace _impl
{

template<typename T, typename DerivedA, typename DerivedB>
unsigned sym_solve(const Eigen::SparseMatrix<T>& L,
                   int k,
                   int nv,
                   unsigned max_iter,
                   double tolerance,
                   Eigen::MatrixBase<DerivedA>& lambdas,
                   Eigen::MatrixBase<DerivedB>& phis)
{
    // use shift-invert mode to get the smallest eigenvalues fast
    auto convergence = std::min(2 * k + 1, nv);
    using Operator = Spectra::SparseSymShiftSolve<T>;
    using Solver = Spectra::SymEigsShiftSolver<Operator>;
    Operator op(L);
    Solver eigensolver(op, k, convergence, -1.0);
    eigensolver.init();
    unsigned n = eigensolver.compute(Spectra::SortRule::LargestMagn,
                                     max_iter,
                                     static_cast<T>(tolerance),
                                     Spectra::SortRule::SmallestMagn);
    if (eigensolver.info() != Spectra::CompInfo::Successful) {
        throw std::runtime_error("Eigen decomposition failed.");
    }
    lambdas = eigensolver.eigenvalues();
    phis = eigensolver.eigenvectors();
    return n;
}

template<typename T, typename DerivedA, typename DerivedB>
unsigned gen_solve(const Eigen::SparseMatrix<T>& S,
                   const Eigen::SparseMatrix<T>& D,
                   int k,
                   int nv,
                   unsigned max_iter,
                   double tolerance,
                   Eigen::MatrixBase<DerivedA>& lambdas,
                   Eigen::MatrixBase<DerivedB>& phis)
{
    int convergence = std::min(2 * k + 1, nv);
    using Operator = Spectra::SymShiftInvert<T, Eigen::Sparse, Eigen::Sparse>;
    using BOperator = Spectra::SparseSymMatProd<T>;
    using Solver =
        Spectra::SymGEigsShiftSolver<Operator,
                                     BOperator,
                                     Spectra::GEigsMode::ShiftInvert>;
    Operator op(S, D);
    BOperator bop(D);
    Solver eigensolver(op, bop, k, convergence, -1.0);
    eigensolver.init();
    unsigned n = eigensolver.compute(Spectra::SortRule::LargestMagn,
                                     max_iter,
                                     static_cast<T>(tolerance),
                                     Spectra::SortRule::SmallestMagn);
    if (eigensolver.info() != Spectra::CompInfo::Successful) {
        throw std::runtime_error("Eigen decomposition failed.");
    }
    lambdas = eigensolver.eigenvalues();
    phis = eigensolver.eigenvectors();
    return n;
}

} // namespace _impl

template<typename Mesh, typename DerivedA, typename DerivedB>
unsigned spectrum(const Mesh& mesh,
                  unsigned k,
                  Eigen::MatrixBase<DerivedA>& lambdas,
                  Eigen::MatrixBase<DerivedB>& phis,
                  SpecOp op,
                  unsigned max_iter,
                  double tolerance)
{
    using T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                               value_type>::Kernel::FT;
    using SpMat = Eigen::SparseMatrix<T>;
    auto nv = num_vertices(mesh);

    if (k > nv) {
        std::string err("You've requested ");
        err.append(std::to_string(k));
        err.append(" eigenvalues but there are only ");
        err.append(std::to_string(nv));
        err.append(" vertices in your mesh.");
        EWARNING(err);
        k = nv;
    }

    unsigned n;
    if (op == SpecOp::mesh_laplacian) {
        SpMat C = Euclid::cotangent_matrix(mesh);
        SpMat D = Euclid::mass_matrix(mesh);
        n = _impl::gen_solve(C, D, k, nv, max_iter, tolerance, lambdas, phis);
    }
    else {
        auto result = Euclid::adjacency_matrix(mesh);
        SpMat A = std::get<0>(result);
        SpMat D = std::get<1>(result);
        SpMat L = D - A;
        n = _impl::sym_solve(L, k, nv, max_iter, tolerance, lambdas, phis);
    }

    if (n < k) {
        auto str = std::to_string(k);
        str.append(" eigenvalues are requested, but only ");
        str.append(std::to_string(n));
        str.append(" values converged in computation.");
        EWARNING(str);
    }
    EASSERT(lambdas.rows() == n);
    EASSERT(phis.cols() == n);
    EASSERT(phis.rows() == nv);

    return n;
}

} // namespace Euclid
