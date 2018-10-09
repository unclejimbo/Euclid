#include <string>

#include <CGAL/boost/graph/properties.h>
#include <Eigen/SparseCore>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Util/Assert.h>
#include <Spectra/MatOp/SparseCholesky.h>
#include <Spectra/MatOp/SparseSymMatProd.h>
#include <Spectra/MatOp/SparseSymShiftSolve.h>
#include <Spectra/SymEigsShiftSolver.h>
#include <Spectra/SymGEigsSolver.h>

namespace Euclid
{

namespace _impl
{

template<typename Mesh, typename T>
void get_mat(const Mesh& mesh,
             SpecOp op,
             Eigen::SparseMatrix<T>& S,
             Eigen::SparseMatrix<T>& D)
{
    if (op == SpecOp::laplace_beltrami) {
        S = Euclid::cotangent_matrix(mesh);
        D = Euclid::mass_matrix(mesh);
    }
    else {
        auto result = Euclid::adjacency_matrix(mesh);
        auto A = std::get<0>(result);
        D = std::get<1>(result);
        S = D - A;
    }
}

template<typename T, typename DerivedA, typename DerivedB>
unsigned sym_solve(const Eigen::SparseMatrix<T>& S,
                   const Eigen::SparseMatrix<T>& D,
                   int k,
                   int nv,
                   unsigned max_iter,
                   double tolerance,
                   Eigen::MatrixBase<DerivedA>& lambdas,
                   Eigen::MatrixBase<DerivedB>& phis)
{
    Eigen::SparseMatrix<T> B =
        D.unaryExpr([](T v) { return v == 0 ? 0 : 1 / std::sqrt(v); });
    Eigen::SparseMatrix<T> L = B * S * B;

    // use shift-invert mode to get the smallest eigenvalues fast
    auto convergence = std::min(2 * k + 1, nv);
    Spectra::SparseSymShiftSolve<T> op(L);
    Spectra::SymEigsShiftSolver<T,
                                Spectra::LARGEST_MAGN,
                                Spectra::SparseSymShiftSolve<T>>
        eigensolver(&op, k, convergence, 0.0f);
    eigensolver.init();
    unsigned n = eigensolver.compute(
        max_iter, static_cast<T>(tolerance), Spectra::SMALLEST_MAGN);
    if (eigensolver.info() != Spectra::SUCCESSFUL) {
        throw std::runtime_error(
            "Unable to compute eigen values of the Laplacian matrix.");
    }
    lambdas = eigensolver.eigenvalues();
    phis = B * eigensolver.eigenvectors();
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
    Spectra::SparseSymMatProd<T> op_a(S);
    Spectra::SparseCholesky<T> op_b(D);
    Spectra::SymGEigsSolver<T,
                            Spectra::SMALLEST_MAGN,
                            Spectra::SparseSymMatProd<T>,
                            Spectra::SparseCholesky<T>,
                            Spectra::GEIGS_MODE::GEIGS_CHOLESKY>
        eigensolver(&op_a, &op_b, k, convergence);
    eigensolver.init();
    unsigned n = eigensolver.compute(
        max_iter, static_cast<T>(tolerance), Spectra::SMALLEST_MAGN);
    if (eigensolver.info() != Spectra::SUCCESSFUL) {
        throw std::runtime_error(
            "Unable to compute eigen values of the Laplacian matrix.");
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
                  SpecDecomp decomp,
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
        err.append(" eigen values but there are only ");
        err.append(std::to_string(nv));
        err.append(" vertices in your mesh.");
        EWARNING(err);
        k = nv;
    }

    SpMat S, D;
    _impl::get_mat(mesh, op, S, D);

    unsigned n;
    if (decomp == SpecDecomp::symmetric) {
        n = _impl::sym_solve(S, D, k, nv, max_iter, tolerance, lambdas, phis);
    }
    else {
        n = _impl::gen_solve(S, D, k, nv, max_iter, tolerance, lambdas, phis);
    }
    if (n < k) {
        auto str = std::to_string(k);
        str.append(" eigen values are requested, but only ");
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
