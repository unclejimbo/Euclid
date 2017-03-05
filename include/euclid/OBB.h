#pragma once

#include <Eigen/Dense>
#include <array>
#include <vector>
#include <cmath>

namespace euclid
{

template<typename Polyhedron_3>
class OBB
{
	using FT = typename Polyhedron_3::Traits::Kernel::FT;
	using Vec3 = Eigen::Matrix<FT, 3, 1>;
	using Mat3 = Eigen::Matrix<FT, 3, 3>;

public:
	explicit OBB(const Polyhedron_3& mesh);
	explicit OBB(const std::vector<Vec3>& vertices);
	~OBB();

	Vec3 center() const;
	std::array<Vec3, 3> directions() const;
	Vec3 halfsize() const;
	float radius() const;
	Vec3 lbb() const;
	Vec3 lbf() const;
	Vec3 ltb() const;
	Vec3 ltf() const;
	Vec3 rbb() const;
	Vec3 rbf() const;
	Vec3 rtb() const;
	Vec3 rtf() const;

private:
	Mat3 _directions;
	Vec3 _center;
	Vec3 _halfsize;
};

template<typename Polyhedron_3>
inline OBB<Polyhedron_3>::OBB(const Polyhedron_3& mesh)
{
	auto mean = Vec3();
	mean.setZero();
	for (auto p = mesh.points_begin(); p != mesh.points_end(); ++p) {
		mean += Vec3(p->x(), p->y(), p->z());
	}
	mean /= mesh.size_of_vertices();

	Mat3 covariance;
	covariance.setZero();
	for (auto p = mesh.points_begin(); p != mesh.points_end(); ++p) {
		covariance(0, 0) += (p->x() - mean(0)) * (p->x() - mean(0));
		covariance(0, 1) += (p->x() - mean(0)) * (p->y() - mean(1));
		covariance(0, 2) += (p->x() - mean(0)) * (p->z() - mean(2));
		covariance(1, 1) += (p->y() - mean(1)) * (p->y() - mean(1));
		covariance(1, 2) += (p->y() - mean(1)) * (p->z() - mean(2));
		covariance(2, 2) += (p->z() - mean(2)) * (p->z() - mean(2));
	}
	covariance(0, 0) /= mesh.size_of_vertices();
	covariance(0, 1) /= mesh.size_of_vertices();
	covariance(0, 2) /= mesh.size_of_vertices();
	covariance(1, 1) /= mesh.size_of_vertices();
	covariance(1, 2) /= mesh.size_of_vertices();
	covariance(2, 2) /= mesh.size_of_vertices();
	covariance(1, 0) = covariance(0, 1);
	covariance(2, 0) = covariance(0, 2);
	covariance(2, 1) = covariance(1, 2);
	
	Eigen::SelfAdjointEigenSolver<Mat3> eigensolver(covariance);
	if (eigensolver.info() != Eigen::Success) {
		assert(-1);
	}

	_directions.col(0) = eigensolver.eigenvectors().col(0);
	_directions.col(1) = eigensolver.eigenvectors().col(1);
	_directions.col(2) = eigensolver.eigenvectors().col(2);

	auto xMax = mean.dot(_directions.col(0));
	auto xMin = xMax;
	auto yMax = mean.dot(_directions.col(1));
	auto yMin = yMax;
	auto zMax = mean.dot(_directions.col(2));
	auto zMin = zMax;
	for (auto p = mesh.points_begin(); p != mesh.points_end(); ++p) {
		auto vec = Vec3(p->x(), p->y(), p->z());
		xMax = std::max(xMax, vec.dot(_directions.col(0)));
		yMax = std::max(yMax, vec.dot(_directions.col(1)));
		zMax = std::max(zMax, vec.dot(_directions.col(2)));
		xMin = std::min(xMin, vec.dot(_directions.col(0)));
		yMin = std::min(yMin, vec.dot(_directions.col(1)));
		zMin = std::min(zMin, vec.dot(_directions.col(2)));
	}
	_halfsize = Vec3(xMax - xMin, yMax - yMin, zMax - zMin) * 0.5f;
	_center = _directions.col(0) * (xMax + xMin) * 0.5f +
		_directions.col(1) * (yMax + yMin) * 0.5f +
		_directions.col(2) * (zMax + zMin) * 0.5f;
}

template<typename Polyhedron_3>
inline OBB<Polyhedron_3>::OBB(const std::vector<Vec3>& vertices)
{
	auto mean = Vec3();
	mean.setZero();
	for (const auto& v : vertices) {
		mean += Vec3(v(0), v(1), v(2));
	}
	mean /= vertices.size();

	Mat3 covariance;
	covariance.setZero();
	for (const auto& v : vertices) {
		covariance(0, 0) += (v(0) - mean(0)) * (v(0) - mean(0));
		covariance(0, 1) += (v(0) - mean(0)) * (v(1) - mean(1));
		covariance(0, 2) += (v(0) - mean(0)) * (v(2) - mean(2));
		covariance(1, 1) += (v(1) - mean(1)) * (v(1) - mean(1));
		covariance(1, 2) += (v(1) - mean(1)) * (v(2) - mean(2));
		covariance(2, 2) += (v(2) - mean(2)) * (v(2) - mean(2));
	}
	covariance(0, 0) /= vertices.size();
	covariance(0, 1) /= vertices.size();
	covariance(0, 2) /= vertices.size();
	covariance(1, 1) /= vertices.size();
	covariance(1, 2) /= vertices.size();
	covariance(2, 2) /= vertices.size();
	covariance(1, 0) = covariance(0, 1);
	covariance(2, 0) = covariance(0, 2);
	covariance(2, 1) = covariance(1, 2);

	Eigen::SelfAdjointEigenSolver<Mat3> eigensolver(covariance);
	if (eigensolver.info() != Eigen::Success) {
		assert(-1);
	}

	_directions.col(0) = eigensolver.eigenvectors().col(0);
	_directions.col(1) = eigensolver.eigenvectors().col(1);
	_directions.col(2) = eigensolver.eigenvectors().col(2);

	auto xMax = mean.dot(_directions.col(0));
	auto xMin = xMax;
	auto yMax = mean.dot(_directions.col(1));
	auto yMin = yMax;
	auto zMax = mean.dot(_directions.col(2));
	auto zMin = zMax;
	for (const auto& v : vertices) {
		auto vec = Vec3(v(0), v(1), v(2));
		xMax = std::max(xMax, vec.dot(_directions.col(0)));
		yMax = std::max(yMax, vec.dot(_directions.col(1)));
		zMax = std::max(zMax, vec.dot(_directions.col(2)));
		xMin = std::min(xMin, vec.dot(_directions.col(0)));
		yMin = std::min(yMin, vec.dot(_directions.col(1)));
		zMin = std::min(zMin, vec.dot(_directions.col(2)));
	}
	_halfsize = Vec3(xMax - xMin, yMax - yMin, zMax - zMin) * 0.5f;
	_center = _directions.col(0) * (xMax + xMin) * 0.5f +
		_directions.col(1) * (yMax + yMin) * 0.5f +
		_directions.col(2) * (zMax + zMin) * 0.5f;
}

template<typename Polyhedron_3>
inline OBB<Polyhedron_3>::~OBB()
{
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3 
OBB<Polyhedron_3>::center() const
{
	return _center;
}

template<typename Polyhedron_3>
inline std::array<typename OBB<Polyhedron_3>::Vec3, 3> 
OBB<Polyhedron_3>::directions() const
{
	std::array<typename OBB<Polyhedron_3>::Vec3, 3> dirs{
		_directions.col(0), _directions.col(1), _directions.col(2) };
	return dirs;
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3
OBB<Polyhedron_3>::halfsize() const
{
	return _halfsize;
}

template<typename Polyhedron_3>
inline float
OBB<Polyhedron_3>::radius() const
{
	auto half_vec = halfsize();
	return std::sqrt(half_vec(0) * half_vec(0) + half_vec(1) * half_vec(1) + half_vec(2) * half_vec(2));
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3
OBB<Polyhedron_3>::lbb() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center - x - y - z;
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3
OBB<Polyhedron_3>::lbf() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center - x - y + z;
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3
OBB<Polyhedron_3>::ltb() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center - x + y - z;
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3
OBB<Polyhedron_3>::ltf() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center - x + y + z;
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3
OBB<Polyhedron_3>::rbb() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center + x - y - z;
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3
OBB<Polyhedron_3>::rbf() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center + x - y + z;
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3
OBB<Polyhedron_3>::rtb() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center + x + y - z;
}

template<typename Polyhedron_3>
inline typename OBB<Polyhedron_3>::Vec3
OBB<Polyhedron_3>::rtf() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center + x + y + z;
}

} // namespace euclid
