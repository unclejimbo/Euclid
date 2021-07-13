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
 *  @defgroup PkgSpectral Spectral
 *  @ingroup PkgGeometry
 */
#pragma once

#include <Eigen/Core>

namespace Euclid
{
/** @{*/

/**The operator to use for spectral decomposition.
 *
 */
enum class SpecOp
{
    /**The discrete Laplace-Beltrami operator.
     *
     * Weighted using the lumped mass matrix.
     */
    mesh_laplacian,

    /**The graph Laplacian operator.
     *
     */
    graph_laplacian
};

/**Spectral decomposition of a mesh.
 *
 * @param mesh The input mesh.
 * @param k The number of eigenvalues to compute. Note that the actual size of
 * the spectrum might be smaller than k when the computation doesn't converge.
 * @param lambdas The output eigenvalues, sorted in ascending order.
 * @param phis The output eigenfunctions corresponding to the eigenvalues.
 * @param op The operator to use.
 * @param max_iter The maximum number of iterations for eigen decomposition.
 * @param tolerance The tolerance of accuracy loss in eigen decomposition.
 *
 * @return The number of converged eigenvalues.
 *
 * @sa SpecOp
 */
template<typename Mesh, typename DerivedA, typename DerivedB>
unsigned spectrum(const Mesh& mesh,
                  unsigned k,
                  Eigen::MatrixBase<DerivedA>& lambdas,
                  Eigen::MatrixBase<DerivedB>& phis,
                  SpecOp op = SpecOp::mesh_laplacian,
                  unsigned max_iter = 1000,
                  double tolerance = 1e-10);

/** @}*/
} // namespace Euclid

#include "src/Spectral.cpp"
