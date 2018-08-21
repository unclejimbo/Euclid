/** Statistics.
 *
 *  Running statistical analysis on a set of random variables.
 *  @defgroup PkgStatistics Statistics
 *  @ingroup PkgMath
 */
#pragma once

#include <Eigen/Core>

namespace Euclid
{
/** @{*/

/** Convariance matrix of a set of variables.
 *
 *  @param vars A @f$ n \times d @f$ matrix, having n observations. Each row is
 *  an d-dimensional variable.
 *  @param cov_mat The output covariance matrix, having size of @f$ d \times d
 *  @f$.
 */
template<typename DerivedA, typename DerivedB>
void covariance_matrix(const Eigen::MatrixBase<DerivedA>& vars,
                       Eigen::MatrixBase<DerivedB>& cov_mat);

/** @}*/
} // namespace Euclid

#include "src/Statistics.cpp"
