#pragma once

#include <Eigen/Core>

namespace Euclid
{
/**@{ @ingroup PkgDistance*/

/**Diffusion distance.
 *
 * @param lambdas Eigenvectors.
 * @param phis Eigenvalues.
 * @param x Vertex x.
 * @param y Vertex y.
 * @param t Diffusion time.
 *
 * **Reference**
 *
 * [1] Coifman, R., Lafon, S.
 * Diffusion maps.
 */
template<typename DerivedA,
         typename DerivedB,
         typename T = typename DerivedA::Scalar>
T diffusion_distance(const Eigen::MatrixBase<DerivedA>& lambdas,
                     const Eigen::MatrixBase<DerivedB>& phis,
                     int x,
                     int y,
                     double t);

/** @}*/
} // namespace Euclid

#include "src/DiffusionDistance.cpp"
