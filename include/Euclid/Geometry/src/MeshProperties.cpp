#include <Euclid/Math/Vector.h>
#include <vector>
#include <ostream>
#define _USE_MATH_DEFINES
#include <cmath>

namespace Euclid
{

template<typename Mesh, typename FaceNormalMap>
inline typename boost::property_traits<FaceNormalMap>::value_type
vertex_normal(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	const FaceNormalMap& fnmap,
	const VertexNormal& weight)
{
	using Vec3 = boost::property_traits<FaceNormalMap>::value_type;
	auto vpmap = get(boost::vertex_point, mesh);

	auto normal = Vec3(0.0, 0.0, 0.0);
	for (auto he : CGAL::halfedges_around_target(v, mesh)) {
		auto f = face(he, mesh);
		auto fn = fnmap[f];

		if (weight == VertexNormal::constant) {
			normal += fn;
		}
		else if (weight == VertexNormal::face_area) {
			auto area = face_area(f, mesh);
			normal += area * fn;
		}
		else { // incident_angle
			auto he_next = opposite(next(he, mesh), mesh);
			auto t = target(he, mesh);
			auto s1 = source(he, mesh);
			auto s2 = source(he_next, mesh);
			auto pt = vpmap[t];
			auto ps1 = vpmap[s1];
			auto ps2 = vpmap[s2];
			auto vec1 = ps1 - pt;
			auto vec2 = ps2 - pt;
			vec1 /= std::sqrt(vec1.squared_length());
			vec2 /= std::sqrt(vec2.squared_length());
			auto angle = vec1 * vec2;
			normal += angle * fn;
		}
	}

	return normal / std::sqrt(normal.squared_length());
}

template<typename Mesh>
inline typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
vertex_area(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	const VertexArea& method)
{
	using FT = typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT;
	auto vpmap = get(boost::vertex_point, mesh);
	const FT one_third = 1.0 / 3.0;
	FT area = 0.0;
	if (method == VertexArea::barycentric) {
		for (auto he : CGAL::halfedges_around_target(v, mesh)) {
			auto p1 = vpmap[source(he, mesh)];
			auto p2 = vpmap[target(he, mesh)];
			auto p3 = vpmap[target(next(he, mesh), mesh)];
			auto mid1 = CGAL::midpoint(p2, p1);
			auto mid2 = CGAL::midpoint(p2, p3);
			auto center = CGAL::barycenter(p1, one_third, p2, one_third, p3);
			area += Euclid::area(mid1, p2, center) + Euclid::area(mid2, center, p2);
		}
	}
	else if (method == VertexArea::voronoi) {
		for (auto he : CGAL::halfedges_around_target(v, mesh)) {
			auto p1 = vpmap[source(he, mesh)];
			auto p2 = vpmap[target(he, mesh)];
			auto p3 = vpmap[target(next(he, mesh), mesh)];
			auto mid1 = CGAL::midpoint(p2, p1);
			auto mid2 = CGAL::midpoint(p2, p3);
			auto center = CGAL::circumcenter(p1, p2, p3);
			area += Euclid::area(mid1, p2, center) + Euclid::area(mid2, center, p2);
		}
	}
	else { // mixed
		for (auto he : CGAL::halfedges_around_target(v, mesh)) {
			auto p1 = vpmap[source(he, mesh)];
			auto p2 = vpmap[target(he, mesh)];
			auto p3 = vpmap[target(next(he, mesh), mesh)];
			auto mid1 = CGAL::midpoint(p2, p1);
			auto mid2 = CGAL::midpoint(p2, p3);
			if (CGAL::angle(p1, p2, p3) == CGAL::OBTUSE) {
				area += Euclid::area(p1, p2, p3) * 0.5;
			}
			else if (CGAL::angle(p2, p3, p1) == CGAL::OBTUSE ||
				CGAL::angle(p3, p1, p2) == CGAL::OBTUSE) {
				area += Euclid::area(p1, p2, p3) * 0.5;
			}
			else { // triangle is acute or right
				auto center = CGAL::circumcenter(p1, p2, p3);
				area += Euclid::area(mid1, p2, center) + Euclid::area(mid2, center, p2);
			}
		}
	}
	return area;
}

template<typename Mesh>
inline typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
edge_length(
	const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
	const Mesh& mesh)
{
	auto vpmap = get(boost::vertex_point, mesh);
	auto p1 = vpmap[source(he, mesh)];
	auto p2 = vpmap[target(he, mesh)];
	auto e = p1 - p2;
	return std::sqrt(e.squared_length());
}

template<typename Mesh>
inline typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
edge_length(
	const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
	const Mesh& mesh)
{
	auto he = halfedge(e, mesh);
	return edge_length(he, mesh);
}

template<typename Mesh>
inline typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
squared_edge_length(
	const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
	const Mesh& mesh)
{
	auto vpmap = get(boost::vertex_point, mesh);
	auto p1 = vpmap[source(he, mesh)];
	auto p2 = vpmap[target(he, mesh)];
	auto e = p1 - p2;
	return e.squared_length();
}

template<typename Mesh>
inline typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
squared_edge_length(
	const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
	const Mesh& mesh)
{
	auto he = halfedge(e, mesh);
	return squared_edge_length(he, mesh);
}

template<typename Mesh>
inline typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::Vector_3
face_normal(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh)
{
	using Vector_3 = typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::Vector_3;

	auto vpmap = get(CGAL::vertex_point, mesh);
	auto he = halfedge(f, mesh);
	auto p1 = vpmap[source(he, mesh)];
	auto p2 = vpmap[target(he, mesh)];
	auto p3 = vpmap[target(next(he, mesh), mesh)];

	Vector_3 result;
	if (CGAL::collinear(p1, p2, p3)) {
		std::cerr << "Degenerate facet, normal is set to zero!" << std::endl;
		result = Vector_3(0.0, 0.0, 0.0);
	}
	else {
		result = normalized(CGAL::normal(p1, p2, p3));
	}
	return result;
}

template<typename Mesh>
inline typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
face_area(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh)
{
	auto vpmap = get(boost::vertex_point, mesh);
	auto he = halfedge(f, mesh);
	auto p1 = vpmap[source(he, mesh)];
	auto p2 = vpmap[target(he, mesh)];
	auto p3 = vpmap[target(next(he, mesh), mesh)];
	return area(p1, p2, p3);
}

template<typename Mesh, typename VertexValueMap>
inline CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel>
gradient(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh,
	const VertexValueMap& vvmap)
{
	using Vector_3 = CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel>;
	auto vpmap = get(boost::vertex_point, mesh);
	auto normal = face_normal(f, mesh);
	Vector_3 grad(0.0, 0.0, 0.0);
	for (const auto& he : halfedges_around_face(halfedge(f, mesh), mesh)) {
		auto v = source(he, mesh);
		auto e = vpmap[target(next(he, mesh), mesh)] - vpmap[target(he, mesh)];
		grad += vvmap[v] * CGAL::cross_product(normal, e);
	}
	grad *= 0.5 / face_area(f, mesh);
	return grad;
}

template<typename Mesh, typename VertexValueMap>
inline std::vector<CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel>>
gradient_field(
	const Mesh& mesh,
	const VertexValueMap& vvmap)
{
	using Vector_3 = CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel>;
	auto vpmap = get(boost::vertex_point, mesh);
	std::vector<Vector_3> gradients;
	gradients.reserve(num_faces(mesh));
	for (const auto& f : faces(mesh)) {
		auto normal = face_normal(f, mesh);
		Vector_3 grad(0.0, 0.0, 0.0);
		for (const auto& he : halfedges_around_face(halfedge(f, mesh), mesh)) {
			auto v = source(he, mesh);
			auto e = vpmap[target(next(he, mesh), mesh)] - vpmap[target(he, mesh)];
			grad += vvmap[v] * CGAL::cross_product(normal, e);
		}
		grad *= 0.5 / face_area(f, mesh);
		gradients.push_back(grad);
	}
	return gradients;
}


template<typename Mesh>
inline CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel>
laplace_beltrami(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh)
{
	using Kernel = typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel;
	using T = typename Kernel::FT;
	using Vector_3 = CGAL::Vector_3<Kernel>;
	auto vpmap = get(boost::vertex_point, mesh);

	Vector_3 flow(0.0, 0.0, 0.0);
	for (const auto& he : halfedges_around_target(v, mesh)) {
		auto vj = source(he, mesh);
		auto va = target(next(he, mesh), mesh);
		auto vb = target(next(opposite(he, mesh), mesh), mesh);
		auto cosa = cosine(vpmap[v], vpmap[va], vpmap[vj]);
		auto cosb = cosine(vpmap[v], vpmap[vb], vpmap[vj]);
		auto cota = cosa / std::sqrt(1.0 - cosa * cosa);
		auto cotb = cosb / std::sqrt(1.0 - cosb * cosb);
		flow += (vpmap[vj] - vpmap[v]) * (cota + cotb);
	}
	return flow / (vertex_area(v, mesh) * 2.0);
}

template<typename Mesh>
inline Eigen::SparseMatrix<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT>
cotangent_matrix(const Mesh& mesh)
{
	using T = typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT;
	auto vimap = get(boost::vertex_index, mesh);
	auto vpmap = get(boost::vertex_point, mesh);
	const auto nv = num_vertices(mesh);
	Eigen::SparseMatrix<T> cotmat(nv, nv);
	std::vector<Eigen::Triplet<T>> values;
	values.reserve(nv * 7);

	for (const auto& vi : vertices(mesh)) {
		int i = vimap[vi];
		T row_sum = 0.0;
		for (const auto& he : CGAL::halfedges_around_target(vi, mesh)) {
			auto vj = source(he, mesh);
			auto va = target(next(he, mesh), mesh);
			auto vb = target(next(opposite(he, mesh), mesh), mesh);
			auto cota = cotangent(vpmap[vi], vpmap[va], vpmap[vj]);
			auto cotb = cotangent(vpmap[vi], vpmap[vb], vpmap[vj]);
			int j = vimap[vj];
			T value = (cota + cotb) * 0.5;
			values.emplace_back(i, j, value);
			row_sum += value;
		}
		values.emplace_back(i, i, -row_sum);
	}

	cotmat.setFromTriplets(values.begin(), values.end());
	cotmat.makeCompressed();
	return cotmat;
}

template<typename Mesh>
inline Eigen::SparseMatrix<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT>
mass_matrix(const Mesh& mesh, const VertexArea& method)
{
	using T = typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT;
	auto vimap = get(boost::vertex_index, mesh);
	const auto nv = num_vertices(mesh);
	Eigen::SparseMatrix<T> mass(nv, nv);
	std::vector<Eigen::Triplet<T>> values;

	for (const auto& v : vertices(mesh)) {
		auto i = vimap[v];
		values.emplace_back(i, i, vertex_area(v, mesh, method));
	}

	mass.setFromTriplets(values.begin(), values.end());
	mass.makeCompressed();
	return mass;
}

template<typename Mesh>
inline typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
gaussian_curvature(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh)
{
	using T = typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT;
	auto vpmap = get(boost::vertex_point, mesh);

	T angle_defect = 2.0 * M_PI;
	for (const auto& he : halfedges_around_target(v, mesh)) {
		auto vp = source(he, mesh);
		auto vq = target(next(he, mesh), mesh);
		angle_defect -= acos(cosine(vpmap[vp], vpmap[v], vpmap[vq]));
	}
	return angle_defect / vertex_area(v, mesh);
}

template<typename Mesh>
inline typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
mean_curvature(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh)
{
	return 0.5 * length(laplace_beltrami(v, mesh));
}

} // namespace Euclid


#undef _USE_MATH_DEFINES
