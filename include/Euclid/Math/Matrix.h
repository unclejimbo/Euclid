/** Matrix utilities.
 *
 * This package contains convenient functions to work with Eigen Matrix.
 * @defgroup PkgMatrix Matrix
 * @ingroup PkgMath
 */
#pragma once
#include <Eigen/Dense>
#include <vector>

namespace Euclid
{
/** @{*/

/** Convariance matrix of vectors.
 *
 */
template<typename FT, int RowSize>
Eigen::Matrix<FT, RowSize, RowSize>
covariance_matrix(const std::vector<Eigen::Matrix<FT, RowSize, 1>>& points);


/** Principle component analysis.
 *
 */
template<typename FT, int RowSize>
class PCA
{
public:
	/** Compute PCA for a vector of Eigen::Vectors
	 *
	 */
	explicit PCA(const std::vector<Eigen::Matrix<FT, RowSize, 1>>& points);


	/** The i-th eigen value.
	 *
	 *  Eigen values are sorted in descending order.
	 */
	FT eigen_value(int i) const;


	/** The i-th eigen vector.
	 *
	 *  Eigen vectors are sorted w.r.t. eigen values in descending order.
	 */
	Eigen::Matrix<FT, RowSize, 1> eigen_vector(int i) const;

private:
	std::vector<FT> _eigen_values;
	std::vector<Eigen::Matrix<FT, RowSize, 1>> _eigen_vectors;
};

/** @}*/
} // namespace Euclid

#include "src/Matrix.cpp"
