/** Matrix utilities.
 *
 * This package contains convenient functions to work with Eigen Matrix.
 * @defgroup PkgMatrix Matrix
 * @ingroup PkgMath
 */
#pragma once

#include <vector>

#include <Eigen/Dense>

namespace Euclid
{
/** @{*/

/** Convariance matrix of vectors.
 *
 */
template<typename FT, int RowSize>
Eigen::Matrix<FT, RowSize, RowSize> covariance_matrix(
    const std::vector<Eigen::Matrix<FT, RowSize, 1>>& points);

/** Principle component analysis.
 *
 *  The eigen values and eigen vectors are sorted in descending order.
 */
template<typename FT, int RowSize>
class PCA
{
public:
    /** Compute PCA for a vector of Eigen::Vectors.
     *
     */
    explicit PCA(const std::vector<Eigen::Matrix<FT, RowSize, 1>>& points);

    /** The i-th eigen value.
     *
     *  Eigen values are sorted in descending order.
     */
    template<int i>
    FT eigen_value() const;

    /** The i-th eigen vector.
     *
     *  Eigen vectors are sorted w.r.t. eigen values in descending order.
     */
    template<int i>
    Eigen::Matrix<FT, RowSize, 1> eigen_vector() const;

private:
    std::vector<FT> _eigen_values;
    std::vector<Eigen::Matrix<FT, RowSize, 1>> _eigen_vectors;
};

/** @}*/
} // namespace Euclid

#include "src/Matrix.cpp"
