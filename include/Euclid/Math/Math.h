#pragma once

#include <Eigen/Dense>
#include <vector>
#include <iostream>

/************************
* Package Overview:     *
* Common math utilities *
************************/
namespace Euclid
{

template<typename FT, int RowSize>
inline bool covariance_matrix(
	const std::vector<Eigen::Matrix<FT, RowSize, 1>>& points,
	Eigen::Matrix<FT, RowSize, RowSize>& cov)
{
	if (points.size() > 1) {
		Eigen::Matrix<FT, RowSize, Eigen::Dynamic> mat(RowSize, points.size());
		for (auto i = 0; i < points.size(); ++i) {
			mat.col(i) = points[i];
		}
		auto centered = mat.colwise() - mat.rowwise().mean();
		cov = (centered * centered.adjoint()) / (mat.cols() - 1.0);
		return true;
	}
	else if (points.size() == 1) {
		cov.setZero();
		return true;
	}
	else {
		std::cerr << "Can't compute covariance matrix for empty set" << std::endl;
		return false;
	}
}

} // namespace Euclid