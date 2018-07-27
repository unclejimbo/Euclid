/** Distance between probability distributions.
 *
 *  This package contains several distance measures between two probability
 *  distributions, e.g. histograms.
 *  @defgroup PkgDistance Distance
 *  @ingroup PkgMath
 */
#pragma once

#include <Eigen/Core>

namespace Euclid
{
/** @{*/

/** L1 distance.
 *
 */
template<typename Derived, typename T = typename Derived::Scalar>
T l1(const Eigen::ArrayBase<Derived>& d1, const Eigen::ArrayBase<Derived>& d2);

/** L2 distance.
 *
 */
template<typename Derived, typename T = typename Derived::Scalar>
T l2(const Eigen::ArrayBase<Derived>& d1, const Eigen::ArrayBase<Derived>& d2);

/** Chi-squared distance.
 *
 *  \f$D(d_1, d_2) = 2\sum_i \frac{(d1(i) - d2(i))^2}{(d1(i) + d2(i))}\f$
 */
template<typename Derived, typename T = typename Derived::Scalar>
T chi2(const Eigen::ArrayBase<Derived>& d1,
       const Eigen::ArrayBase<Derived>& d2);

/** Asymmetric chi-squared distance.
 *
 *  \f$D(d_1, d_2) = \sum_i \frac{(d1(i) - d2(i))^2}{d1(i)}\f$
 */
template<typename Derived, typename T = typename Derived::Scalar>
T chi2_asym(const Eigen::ArrayBase<Derived>& d1,
            const Eigen::ArrayBase<Derived>& d2);

/** @}*/
} // namespace Euclid

#include "src/Distance.cpp"
