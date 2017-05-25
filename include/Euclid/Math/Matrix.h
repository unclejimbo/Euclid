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

template<typename FT, int RowSize>
class PCA
{
	using Point = Eigen::Matrix<FT, RowSize, 1>;
	using Matrix = Eigen::Matrix<FT, RowSize, RowSize>;

public:
	explicit PCA(const std::vector<Point>& points);
	~PCA();

	Point eigen_vector(int i) const;
	const Matrix& eigen_vectors() const;
	FT eigen_value(int i) const;
	const Point& eigen_values() const;

private:
	Matrix _eigen_vectors;
	Point _eigen_values;
};

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
		_eigen_vectors = eigensolver.eigenvectors();
		_eigen_values = eigensolver.eigenvalues();
	}
}

template<typename FT, int RowSize>
inline PCA<FT, RowSize>::~PCA()
{
}

template<typename FT, int RowSize>
inline typename PCA<FT, RowSize>::Point
PCA<FT, RowSize>::eigen_vector(int i) const
{
	return _eigen_vectors.col(i);
}

template<typename FT, int RowSize>
inline const typename PCA<FT, RowSize>::Matrix&
PCA<FT, RowSize>::eigen_vectors() const
{
	return _eigen_vectors;
}

template<typename FT, int RowSize>
inline FT PCA<FT, RowSize>::eigen_value(int i) const
{
	return _eigen_values(i, 0);
}

template<typename FT, int RowSize>
inline const typename PCA<FT, RowSize>::Point&
PCA<FT, RowSize>::eigen_values() const
{
	return _eigen_values();
}

} // namespace Euclid