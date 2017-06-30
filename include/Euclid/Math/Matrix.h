/*************************************
* Package Overview:                  *
* Common matrix associated utilities *
*************************************/
#pragma once
#include <Eigen/Dense>

namespace Euclid
{

template<typename FT, typename CGALVec>
inline Eigen::Matrix<FT, 3, 1> cgal_to_eigen(const CGALVec& vec);

template<typename CGALVec, typename FT>
inline CGALVec eigen_to_cgal(const Eigen::Matrix<FT, 3, 1>& vec);

template<typename FT, int RowSize>
inline bool covariance_matrix(
	const std::vector<Eigen::Matrix<FT, RowSize, 1>>& points,
	Eigen::Matrix<FT, RowSize, RowSize>& cov);

template<typename FT, int RowSize>
class PCA
{
	using Point = Eigen::Matrix<FT, RowSize, 1>;
	using Matrix = Eigen::Matrix<FT, RowSize, RowSize>;

public:
	explicit PCA(const std::vector<Point>& points);
	~PCA();

	FT eigen_value(int i) const;
	Point eigen_vector(int i) const;

private:
	std::vector<FT> _eigen_values;
	std::vector<Point> _eigen_vectors;
};

} // namespace Euclid

#include "src/Matrix.cpp"
