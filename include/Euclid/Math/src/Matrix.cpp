#include <vector>
#include <algorithm>
#include <iostream>

namespace Euclid
{

template<typename FT, typename CGALVec>
inline Eigen::Matrix<FT, 3, 1> cgal_to_eigen(const CGALVec& vec)
{
	return Eigen::Matrix<FT, 3, 1>(vec.x(), vec.y(), vec.z());
}

template<typename CGALVec, typename FT>
inline CGALVec eigen_to_cgal(const Eigen::Matrix<FT, 3, 1>& vec)
{
	return CGALVec(vec(0), vec(1), vec(2));
}

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

template<typename FT, int RowSize>
inline PCA<FT, RowSize>::PCA(const std::vector<Point>& points)
{
	Matrix covariance;
	Euclid::covariance_matrix<FT, RowSize>(points, covariance);

	Eigen::SelfAdjointEigenSolver<Matrix> eigensolver(covariance);
	if (eigensolver.info() != Eigen::Success) {
		std::cerr << "PCA analysis failed" << std::endl;
		assert(-1);
	}
	else {
		// Sort the eigen vectors wrt eigen values in descending order
		auto eigen_values = eigensolver.eigenvalues();
		std::vector<std::pair<FT, int>> value_index_map;
		value_index_map.reserve(eigen_values.rows());
		for (auto i = 0; i < eigen_values.rows(); ++i) {
			value_index_map.emplace_back(eigen_values(i, 0), i);
		}
		std::sort(value_index_map.begin(), value_index_map.end(),
			[](const std::pair<FT, int>& v1, const std::pair<FT, int>& v2) {
			return v1.first > v2.first;
		}
		);

		_eigen_values.resize(value_index_map.size());
		std::transform(value_index_map.begin(), value_index_map.end(), _eigen_values.begin(),
			[](const std::pair<FT, int>& v) {
			return v.first;
		}
		);

		auto eigen_vectors = eigensolver.eigenvectors();
		_eigen_vectors.resize(value_index_map.size());
		std::transform(value_index_map.begin(), value_index_map.end(), _eigen_vectors.begin(),
			[&eigen_vectors](const std::pair<FT, int>& v) {
			return Point(eigen_vectors.col(v.second));
		}
		);
	}
}

template<typename FT, int RowSize>
inline PCA<FT, RowSize>::~PCA()
{
}

template<typename FT, int RowSize>
inline FT PCA<FT, RowSize>::eigen_value(int i) const
{
	return _eigen_values[i];
}

template<typename FT, int RowSize>
inline typename PCA<FT, RowSize>::Point
PCA<FT, RowSize>::eigen_vector(int i) const
{
	return _eigen_vectors[i];
}

} // namespace Euclid