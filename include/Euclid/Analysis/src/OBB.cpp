#include <Euclid/Math/Matrix.h>
#include <Euclid/Math/Vector.h>
#include <tuple>
#include <cmath>

namespace Euclid
{

template<typename Kernel>
template<typename Mesh>
inline OBB<Kernel>::OBB(const Mesh& mesh)
{
	using vertex_iterator = boost::graph_traits<Mesh>::vertex_iterator;
	using Vec3 = Eigen::Matrix<FT, 3, 1>;

	std::vector<Vec3> points;
	points.reserve(num_vertices(mesh));
	auto vpmap = get(CGAL::vertex_point, mesh);
	vertex_iterator v_beg, v_end;
	std::tie(v_beg, v_end) = vertices(mesh);
	while (v_beg != v_end) {
		auto p = vpmap[*v_beg++];
		points.emplace_back(p.x(), p.y(), p.z());
	}

	Euclid::PCA<FT, 3> pca(points);

	_directions[0] = Euclid::eigen_to_cgal<Vector_3>(pca.eigen_vector(0));
	_directions[1] = Euclid::eigen_to_cgal<Vector_3>(pca.eigen_vector(1));
	_directions[2] = Euclid::eigen_to_cgal<Vector_3>(pca.eigen_vector(2));

	std::tie(v_beg, v_end) = vertices(mesh);
	auto p = vpmap[*v_beg++];
	auto vec = Vec3(p.x(), p.y(), p.z());
	auto x_max = vec.dot(pca.eigen_vector(0));
	auto x_min = x_max;
	auto y_max = vec.dot(pca.eigen_vector(1));
	auto y_min = y_max;
	auto z_max = vec.dot(pca.eigen_vector(2));
	auto z_min = z_max;
	while (v_beg != v_end) {
		auto p = vpmap[*v_beg++];
		auto vec = Vec3(p.x(), p.y(), p.z());
		x_max = std::max(x_max, vec.dot(pca.eigen_vector(0)));
		x_min = std::min(x_min, vec.dot(pca.eigen_vector(0)));
		y_max = std::max(y_max, vec.dot(pca.eigen_vector(1)));
		y_min = std::min(y_min, vec.dot(pca.eigen_vector(1)));
		z_max = std::max(z_max, vec.dot(pca.eigen_vector(2)));
		z_min = std::min(z_min, vec.dot(pca.eigen_vector(2)));
	}

	_lbb = CGAL::ORIGIN + x_min * _directions[0] + y_min * _directions[1] + z_min * _directions[2];

	_directions[0] *= (x_max - x_min);
	_directions[1] *= (y_max - y_min);
	_directions[2] *= (z_max - z_min);
}

template<typename Kernel>
inline OBB<Kernel>::OBB(const std::vector<Point_3>& points)
{
	using Vec3 = Eigen::Matrix<FT, 3, 1>;

	std::vector<Vec3> vertices;
	vertices.reserve(points.size());
	for (const auto& p : points) {
		vertices.emplace_back(p.x(), p.y(), p.z());
	}

	Euclid::PCA<FT, 3> pca(vertices);

	_directions[0] = Euclid::eigen_to_cgal<Vector_3>(pca.eigen_vector(0));
	_directions[1] = Euclid::eigen_to_cgal<Vector_3>(pca.eigen_vector(1));
	_directions[2] = Euclid::eigen_to_cgal<Vector_3>(pca.eigen_vector(2));

	auto p = vertices[0];
	auto vec = Vec3(p.x(), p.y(), p.z());
	auto x_max = vec.dot(pca.eigen_vector(0));
	auto x_min = x_max;
	auto y_max = vec.dot(pca.eigen_vector(1));
	auto y_min = y_max;
	auto z_max = vec.dot(pca.eigen_vector(2));
	auto z_min = z_max;
	for (auto i = 1; i < vertices.size(); ++i) {
		auto p = vertices[i];
		auto vec = Vec3(p.x(), p.y(), p.z());
		x_max = std::max(x_max, vec.dot(pca.eigen_vector(0)));
		x_min = std::min(x_min, vec.dot(pca.eigen_vector(0)));
		y_max = std::max(y_max, vec.dot(pca.eigen_vector(1)));
		y_min = std::min(y_min, vec.dot(pca.eigen_vector(1)));
		z_max = std::max(z_max, vec.dot(pca.eigen_vector(2)));
		z_min = std::min(z_min, vec.dot(pca.eigen_vector(2)));
	}

	_lbb = CGAL::ORIGIN + x_min * _directions[0] + y_min * _directions[1] + z_min * _directions[2];

	_directions[0] *= (x_max - x_min);
	_directions[1] *= (y_max - y_min);
	_directions[2] *= (z_max - z_min);
}

template<typename Kernel>
template<typename ForwardIterator, typename PPMap>
inline OBB<Kernel>::OBB(ForwardIterator first,
	ForwardIterator beyond, PPMap point_pmap)
{
	_buildOBB(first, beyond, point_pmap);
}

template<typename Kernel>
inline OBB<Kernel>::OBB(const CGAL::Point_set_3<Point_3>& point_set)
{
	_buildOBB(point_set.begin(), point_set.end(), point_set.point_map());
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3
OBB<Kernel>::center() const
{
	return _lbb + _directions[0] * 0.5 + _directions[1] * 0.5 + _directions[2] * 0.5;
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3
OBB<Kernel>::lbb() const
{
	return _lbb;
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3
OBB<Kernel>::lbf() const
{
	return _lbb + _directions[2];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3
OBB<Kernel>::ltb() const
{
	return _lbb + _directions[1];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3
OBB<Kernel>::ltf() const
{
	return _lbb + _directions[1] + _directions[2];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3
OBB<Kernel>::rbb() const
{
	return _lbb + _directions[0];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3
OBB<Kernel>::rbf() const
{
	return _lbb + _directions[0] + _directions[2];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3
OBB<Kernel>::rtb() const
{
	return _lbb + _directions[0] + _directions[1];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3
OBB<Kernel>::rtf() const
{
	return _lbb + _directions[0] + _directions[1] + _directions[2];
}

template<typename Kernel>
template<typename ForwardIterator, typename PPMap>
inline void OBB<Kernel>::_buildOBB(ForwardIterator first,
	ForwardIterator beyond, PPMap point_pmap)
{
	using Vec3 = Eigen::Matrix<FT, 3, 1>;
	assert(first != beyond);
	auto iter = first;

	std::vector<Vec3> vertices;
	do {
		auto p = point_pmap[*first++];
		vertices.emplace_back(p.x(), p.y(), p.z());
	} while (first != beyond);

	Euclid::PCA<FT, 3> pca(vertices);

	_directions[0] = Euclid::eigen_to_cgal<Vector_3>(pca.eigen_vector(0));
	_directions[1] = Euclid::eigen_to_cgal<Vector_3>(pca.eigen_vector(1));
	_directions[2] = Euclid::eigen_to_cgal<Vector_3>(pca.eigen_vector(2));


	auto p = point_pmap[*iter++];
	auto vec = Vec3(p.x(), p.y(), p.z());
	auto x_max = vec.dot(pca.eigen_vector(0));
	auto x_min = x_max;
	auto y_max = vec.dot(pca.eigen_vector(1));
	auto y_min = y_max;
	auto z_max = vec.dot(pca.eigen_vector(2));
	auto z_min = z_max;
	while (iter != beyond) {
		auto p = point_pmap[*iter++];
		auto vec = Vec3(p.x(), p.y(), p.z());
		x_max = std::max(x_max, vec.dot(pca.eigen_vector(0)));
		x_min = std::min(x_min, vec.dot(pca.eigen_vector(0)));
		y_max = std::max(y_max, vec.dot(pca.eigen_vector(1)));
		y_min = std::min(y_min, vec.dot(pca.eigen_vector(1)));
		z_max = std::max(z_max, vec.dot(pca.eigen_vector(2)));
		z_min = std::min(z_min, vec.dot(pca.eigen_vector(2)));
	}

	_lbb = CGAL::ORIGIN + x_min * _directions[0] + y_min * _directions[1] + z_min * _directions[2];

	_directions[0] *= (x_max - x_min);
	_directions[1] *= (y_max - y_min);
	_directions[2] *= (z_max - z_min);
}

} // namespace Euclid
