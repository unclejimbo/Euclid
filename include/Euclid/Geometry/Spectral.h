/** Spectral decomposition of a 3D shape.
 *
 *  This package contains several methods to conduct spectral decomposition of a
 *  given shape by computing the eigenvalues and eigenvectors of the Laplacian
 *  matrix. It provides the basis for various spectral shape processing
 *  algorithms.
 *
 *  **References**
 *
 *  [1] Zhang, H., Van Kaick, O., Dyer, R.
 *  Spectral Mesh Processing.
 *  Computer Graphics Forum, 2010.
 *
 *  [2] Vallet, B., Levy, B.
 *  Spectral Geometry Processing with Manifold Harmonics.
 *  Computer Graphics Forum, 2008.
 *
 *  @defgroup PkgSpectral Spectral
 *  @ingroup PkgGeometry
 */
#pragma once
#include <Euclid/Config.h>
#ifdef EUCLID_USE_SPECTRA

#include <Eigen/Core>

namespace Euclid
{
/** @{*/

/** The operator to use for spectral decomposition.
 *
 */
enum class SpecOp
{
    /** The Laplace-Beltrami operator.
     *
     */
    laplace_beltrami,

    /** The graph Laplacian operator.
     *
     */
    graph_laplacian
};

/** The method to use for spectral decomposition.
 *
 */
enum class SpecDecomp
{
    /** Solving the symmetric Laplacian matrix.
     *
     *  Let @f$L=D^{-1/2}SD^{-1/2}@f$, then solving the eigenvalue problem of
     *  this symmetric matrix @f$LX=\lambda X@f$.
     */
    symmetric,

    /** Solving the generalized eigenvalue problem.
     *
     *  Let @f$L=D^{-1}S@f$, then solving the generalized eigenvalue problem
     *  @f$SX=\lambda DX@f$.
     */
    generalized
};

/** Spectral decomposition of a mesh.
 *
 *  @param mesh The input mesh.
 *  @param k The number of eigenvalues to compute. Note that the actual size of
 *  the spectrum might be smaller than k when the computation doesn't converge.
 *  @param lambdas The output eigenvalues, sorted in ascending order.
 *  @param phis The output eigenfunctions corresponding to the eigenvalues.
 *  @param op The Laplace operator to use.
 *  @param decomp The eigen system to solve.
 *  @param max_iter The maximum number of iterations for eigen decomposition.
 *  @param tolerance The tolerance of accuracy loss in eigen decomposition.
 *
 *  @return The number of converged eigenvalues.
 *
 *  @sa SpecOp
 *  @sa SpecDecomp
 */
template<typename Mesh, typename DerivedA, typename DerivedB>
unsigned spectrum(const Mesh& mesh,
                  unsigned k,
                  Eigen::MatrixBase<DerivedA>& lambdas,
                  Eigen::MatrixBase<DerivedB>& phis,
                  SpecOp op = SpecOp::laplace_beltrami,
                  SpecDecomp decomp = SpecDecomp::symmetric,
                  unsigned max_iter = 1000,
                  double tolerance = 1e-10);

/** @}*/
} // namespace Euclid

#include "src/Spectral.cpp"

#endif
