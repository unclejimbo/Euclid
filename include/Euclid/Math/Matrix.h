/** Matrix utilities.
 *
 * This package contains convenient functions to work with Eigen Matrix.
 * @defgroup PkgMatrix Matrix
 * @ingroup PkgMath
 */
#pragma once

#include <vector>

#include <Eigen/Dense>
#include <Eigen/SparseCore>

namespace Euclid
{
/** @{*/

/** Convariance matrix of vectors.
 *
 */
template<typename FT, int RowSize>
Eigen::Matrix<FT, RowSize, RowSize> covariance_matrix(
    const std::vector<Eigen::Matrix<FT, RowSize, 1>>& points);

/** Apply a function for each non-zero element in a sparse matrix.
 *
 */
template<typename Scalar,
         int Options,
         typename StorageIndex,
         typename UnaryFunction>
void for_each(Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat,
              UnaryFunction f);

/** Apply a function for each non-zero element in a sparse matrix and
 *  store the result to another.
 *
 */
template<typename Scalar,
         int Options,
         typename StorageIndex,
         typename UnaryFunction>
void transform(const Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat_in,
               Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat_out,
               UnaryFunction f);

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
