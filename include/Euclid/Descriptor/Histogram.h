/**Measure histograms.
 *
 * Histograms are commonly used as shape descriptors. This package contains
 * functions to compute distances between histograms.
 * @defgroup PkgHistogram Histogram
 * @ingroup PkgDescriptor
 */
#pragma once

#include <type_traits>

#include <Eigen/Core>

namespace Euclid
{
/** @{*/

/** L1 distance.
 *
 */
template<typename DerivedA,
         typename DerivedB,
         typename T = typename DerivedA::Scalar,
         typename = std::enable_if_t<std::is_same_v<typename DerivedA::Scalar,
                                                    typename DerivedB::Scalar>>>
T l1(const Eigen::ArrayBase<DerivedA>& d1,
     const Eigen::ArrayBase<DerivedB>& d2);

/** L2 distance.
 *
 */
template<typename DerivedA,
         typename DerivedB,
         typename T = typename DerivedA::Scalar,
         typename = std::enable_if_t<std::is_same_v<typename DerivedA::Scalar,
                                                    typename DerivedB::Scalar>>>
T l2(const Eigen::ArrayBase<DerivedA>& d1,
     const Eigen::ArrayBase<DerivedB>& d2);

/** Chi-squared distance.
 *
 *  @f$D(d_1, d_2) = 2\sum_i \frac{(d1(i) - d2(i))^2}{(d1(i) + d2(i))}@f$
 */
template<typename DerivedA,
         typename DerivedB,
         typename T = typename DerivedA::Scalar,
         typename = std::enable_if_t<std::is_same_v<typename DerivedA::Scalar,
                                                    typename DerivedB::Scalar>>>
T chi2(const Eigen::ArrayBase<DerivedA>& d1,
       const Eigen::ArrayBase<DerivedB>& d2);

/** Asymmetric chi-squared distance.
 *
 *  @f$D(d_1, d_2) = \sum_i \frac{(d1(i) - d2(i))^2}{d1(i)}@f$
 */
template<typename DerivedA,
         typename DerivedB,
         typename T = typename DerivedA::Scalar,
         typename = std::enable_if_t<std::is_same_v<typename DerivedA::Scalar,
                                                    typename DerivedB::Scalar>>>
T chi2_asym(const Eigen::ArrayBase<DerivedA>& d1,
            const Eigen::ArrayBase<DerivedB>& d2);

/** @}*/
} // namespace Euclid

#include "src/Histogram.cpp"
