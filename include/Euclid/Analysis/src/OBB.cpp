#include <Euclid/Math/Matrix.h>
#include <Eigen/Dense>
#include <array>
#include <vector>
#include <cmath>

namespace Euclid
{

template<typename Polyhedron_3>
inline OBB<Polyhedron_3>::OBB(const Polyhedron_3& mesh)
{
	std::vector<Vec3> points;
	points.reserve((mesh.size_of_vertices()));
	Vec3 mean;
	mean.setZero();
	for (auto p = mesh.points_begin(); p != mesh.points_end(); ++p) {
		mean += Vec3(p->x(), p->y(), p->z());
		points.emplace_back(p->x(), p->y(), p->z());
	}
	mean /= mesh.size_of_vertices();

	Euclid::PCA<FT, 3> pca(points);

	_directions.col(0) = pca.eigen_vector(0);
	_directions.col(1) = pca.eigen_vector(1);
	_directions.col(2) = pca.eigen_vector(2);

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
	Vec3 mean;
	mean.setZero();
	for (const auto& v : vertices) {
		mean += Vec3(v(0), v(1), v(2));
	}
	mean /= vertices.size();

	Euclid::PCA<FT, 3> pca(vertices);

	_directions.col(0) = pca.eigen_vector(0);
	_directions.col(1) = pca.eigen_vector(1);
	_directions.col(2) = pca.eigen_vector(2);

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

} // namespace Euclid