#include <ostream>
#include <cmath>

namespace Euclid
{

template<typename Mesh, typename FaceNormalMap>
decltype(auto) vertex_normal(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	const FaceNormalMap& fnmap,
	const NormalWeighting& weight)
{
	using Vec3 = boost::property_traits<FaceNormalMap>::value_type;
	auto vpmap = get(boost::vertex_point, mesh);

	auto normal = Vec3(0.0, 0.0, 0.0);
	auto he = halfedge(v, mesh);
	CGAL::Halfedge_around_target_circulator hetc(he);
	while (*++hetc != he) {
		auto f = face(*hetc, mesh);
		auto fn = fnmap[f];

		if (weight == NormalWeighting::constant) {
			normal += fn;
		}
		else if (weight == NormalWeighting::face_area) {
			auto area = facet_area(f, mesh);
			normal += area * fn;
		}
		else { // incident_angle
			auto he_next = opposite(next(*hetc, mesh), mesh);
			auto t = target(*hetc, mesh);
			auto s1 = source(*hetc, mesh);
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
decltype(auto) inline edge_length(
	const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
	const Mesh& mesh)
{
	auto v1 = source(he, mesh);
	auto v2 = target(he, mesh);
	auto vpmap = get(CGAL::vertex_point, mesh);
	auto p1 = vpmap[v1];
	auto p2 = vpmap[v2];
	auto e = p1 - p2;
	return std::sqrt(e.squared_length());
}

template<typename Mesh>
decltype(auto) inline edge_length(
	const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
	const Mesh& mesh)
{
	auto he = halfedge(e, mesh);
	return edge_length(he, mesh);
}

template<typename Mesh>
decltype(auto) inline face_normal(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh)
{
	using VPMap = boost::property_map<Mesh, boost::vertex_point_t>::type;
	using Point_3 = boost::property_traits<VPMap>::value_type;
	using Vector_3 = CGAL::Kernel_traits<Point_3>::Kernel::Vector_3;

	auto he = halfedge(f, mesh);
	auto v1 = source(he, mesh);
	auto v2 = target(he, mesh);
	auto v3 = target(next(he, mesh), mesh);
	auto vpmap = get(CGAL::vertex_point, mesh);
	auto p1 = vpmap[v1];
	auto p2 = vpmap[v2];
	auto p3 = vpmap[v3];
	auto e1 = p2 - p1;
	auto e2 = p3 - p2;
	e1 /= std::sqrt(e1.squared_length());
	e2 /= std::sqrt(e2.squared_length());

	Vector_3 result;
	if (e1 * e2 == 1.0 || e1 * e2 == -1.0) {
		std::cerr << "Degenerate facet, normal is set to zero!" << std::endl;
		result = Vector_3(0.0, 0.0, 0.0);
	}
	else {
		result = CGAL::cross_product(e1, e2);
	}
	return result;
}

template<typename Mesh>
decltype(auto) inline face_area(
	const typename boost::graph_traits<const Mesh>::facet_descriptor& f,
	const Mesh& mesh)
{
	auto he = halfedge(f, mesh);
	auto v1 = source(he, mesh);
	auto v2 = target(he, mesh);
	auto v3 = target(next(he, mesh), mesh);
	auto vpmap = get(CGAL::vertex_point, mesh);
	auto p1 = vpmap[v1];
	auto p2 = vpmap[v2];
	auto p3 = vpmap[v3];
	auto e1 = p1 - p2;
	auto e2 = p3 - p2;
	return CGAL::cross_product(e1, e2) * 0.5;
}

} // namespace Euclid
