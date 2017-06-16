/*************************************
* Package Overview:                  *
* Common matrix associated utilities *
*************************************/
#pragma once
#include <Eigen/Dense>

namespace Euclid
{

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
	const std::vector<FT>& eigen_values() const;
	Point eigen_vector(int i) const;
	const std::vector<Point>& eigen_vectors() const;

private:
	std::vector<FT> _eigen_values;
	std::vector<Point> _eigen_vectors;
};

} // namespace Euclid

#include "src/Matrix.cpp"
