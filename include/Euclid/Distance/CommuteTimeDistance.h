#pragma once

#include <Eigen/Core>

namespace Euclid
{
/**@{ @ingroup PkgDistance*/

/**Commute time distance.
 *
 * @param lambdas Eigenvectors.
 * @param phis Eigenvalues.
 * @param x Vertex x.
 * @param y Vertex y.
 *
 * **Reference**
 *
 * [1] Fouss, F. etc.
 * Random-walk computation of similarities between nodes of a graph with
 * application to collaborative recommendation.
 */
template<typename DerivedA,
         typename DerivedB,
         typename T = typename DerivedA::Scalar>
T commute_time_distance(const Eigen::MatrixBase<DerivedA>& lambdas,
                        const Eigen::MatrixBase<DerivedB>& phis,
                        int x,
                        int y);

/** @}*/
} // namespace Euclid

#include "src/CommuteTimeDistance.cpp"
