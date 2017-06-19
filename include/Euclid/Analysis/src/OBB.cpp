#include <Euclid/Math/Matrix.h>
#include <array>
#include <vector>
#include <tuple>
#include <cmath>

namespace Euclid
{

template<typename FT>
template<typename Mesh>
inline OBB<FT>::OBB(const Mesh& mesh)
{
	using vertex_iterator = boost::graph_traits<Mesh>::vertex_iterator;

	std::vector<Vec3> points;
	points.reserve(num_vertices(mesh));
	auto vpmap = get(CGAL::vertex_point, mesh);
	Vec3 mean;
	mean.setZero();
	vertex_iterator v_beg, v_end;
	std::tie(v_beg, v_end) = vertices(mesh);
	while (v_beg != v_end) {
		auto p = vpmap[*v_beg++];
		mean += Vec3(p.x(), p.y(), p.z());
		points.emplace_back(p.x(), p.y(), p.z());
	}
	mean /= num_vertices(mesh);

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
	std::tie(v_beg, v_end) = vertices(mesh);
	while (v_beg != v_end) {
		auto p = vpmap[*v_beg++];
		auto vec = Vec3(p.x(), p.y(), p.z());
		xMax = std::max(xMax, vec.dot(_directions.col(0)));
		yMax = std::max(yMax, vec.dot(_directions.col(1)));
		zMax = std::max(zMax, vec.dot(_directions.col(2)));
		xMin = std::min(xMin, vec.dot(_directions.col(0)));
		yMin = std::min(yMin, vec.dot(_directions.col(1)));
		zMin = std::min(zMin, vec.dot(_directions.col(2)));
	}
	_halfsize = Vec3(xMax - xMin, yMax - yMin, zMax - zMin) * 0.5;
	_center = _directions.col(0) * (xMax + xMin) * 0.5 +
		_directions.col(1) * (yMax + yMin) * 0.5 +
		_directions.col(2) * (zMax + zMin) * 0.5;
}

template<typename FT>
inline OBB<FT>::OBB(const std::vector<Vec3>& vertices)
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
	_halfsize = Vec3(xMax - xMin, yMax - yMin, zMax - zMin) * 0.5;
	_center = _directions.col(0) * (xMax + xMin) * 0.5 +
		_directions.col(1) * (yMax + yMin) * 0.5 +
		_directions.col(2) * (zMax + zMin) * 0.5;
}

template<typename FT>
inline OBB<FT>::~OBB()
{
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::center() const
{
	return _center;
}

template<typename FT>
inline std::array<typename OBB<FT>::Vec3, 3>
OBB<FT>::directions() const
{
	std::array<typename OBB<FT>::Vec3, 3> dirs{
		_directions.col(0), _directions.col(1), _directions.col(2) };
	return dirs;
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::halfsize() const
{
	return _halfsize;
}

template<typename FT>
inline float
OBB<FT>::radius() const
{
	auto half_vec = halfsize();
	return std::sqrt(half_vec(0) * half_vec(0) + half_vec(1) * half_vec(1) + half_vec(2) * half_vec(2));
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::lbb() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center - x - y - z;
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::lbf() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center - x - y + z;
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::ltb() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center - x + y - z;
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::ltf() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center - x + y + z;
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::rbb() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center + x - y - z;
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::rbf() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center + x - y + z;
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::rtb() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center + x + y - z;
}

template<typename FT>
inline typename OBB<FT>::Vec3
OBB<FT>::rtf() const
{
	Vec3 x = _directions.col(0) * _halfsize(0);
	Vec3 y = _directions.col(1) * _halfsize(1);
	Vec3 z = _directions.col(2) * _halfsize(2);
	return _center + x + y + z;
}

} // namespace Euclid