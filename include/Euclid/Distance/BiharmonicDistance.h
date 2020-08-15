#pragma once

#include <Eigen/Core>

namespace Euclid
{
/**@{ @ingroup PkgDistance*/

/**Biharmonic distance.
 *
 * @param lambdas Eigenvectors.
 * @param phis Eigenvalues.
 * @param x Vertex x.
 * @param y Vertex y.
 *
 * **Reference**
 *
 * [1] Lipman, Y., Rustamov, R., Funkhouser, T.
 * Biharmonic distance.
 */
template<typename DerivedA,
         typename DerivedB,
         typename T = typename DerivedA::Scalar>
T biharmonic_distance(const Eigen::MatrixBase<DerivedA>& lambdas,
                      const Eigen::MatrixBase<DerivedB>& phis,
                      int x,
                      int y);

/** @}*/
} // namespace Euclid

#include "src/BiharmonicDistance.cpp"
