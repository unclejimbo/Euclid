#include <Eigen/Dense>
#include <ostream>
#include <cmath>

namespace Euclid
{

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
	auto p = p1 - p2;
	return std::sqrt(p.x() * p.x() + p.y() * p.y() + p.z() * p.z());
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
decltype(auto) inline facet_normal(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh)
{
	using VPMap = boost::property_map<Mesh, boost::vertex_point_t>::type;
	using Point_3 = boost::property_traits<VPMap>::value_type;
	using FT = CGAL::Kernel_traits<Point_3>::Kernel::FT;
	using Vec3 = Eigen::Matrix<FT, 3, 1>;

	auto he = halfedge(f, mesh);
	auto v1 = source(he, mesh);
	auto v2 = target(he, mesh);
	auto v3 = target(next(he, mesh), mesh);
	auto vpmap = get(CGAL::vertex_point, mesh);
	auto p1 = vpmap[v1];
	auto p2 = vpmap[v2];
	auto p3 = vpmap[v3];
	auto pp1 = Vec3(p1.x(), p1.y(), p1.z());
	auto pp2 = Vec3(p2.x(), p2.y(), p2.z());
	auto pp3 = Vec3(p3.x(), p3.y(), p3.z());
	auto e1 = (pp2 - pp1).normalized();
	auto e2 = (pp3 - pp2).normalized();

	Vec3 result;
	if (e1.dot(e2) == 1.0 || e1.dot(e2) == -1.0) {
		std::cerr << "Degenerate facet, normal is set to zero!" << std::endl;
		result = Vec3(0.0, 0.0, 0.0);
	}
	else {
		result = e1.cross(e2).normalized();
	}
	return result;
}

template<typename Mesh>
decltype(auto) inline facet_area(
	const typename boost::graph_traits<const Mesh>::facet_descriptor& f,
	const Mesh& mesh)
{
	using VPMap = boost::property_map<Mesh, boost::vertex_point_t>::type;
	using Point_3 = boost::property_traits<VPMap>::value_type;
	using FT = CGAL::Kernel_traits<Point_3>::Kernel::FT;
	using Vec3 = Eigen::Matrix<FT, 3, 1>;

	auto he = halfedge(f, mesh);
	auto v1 = source(he, mesh);
	auto v2 = target(he, mesh);
	auto v3 = target(next(he, mesh), mesh);
	auto vpmap = get(CGAL::vertex_point, mesh);
	auto p1 = vpmap[v1];
	auto p2 = vpmap[v2];
	auto p3 = vpmap[v3];
	auto pp1 = Vec3(p1.x(), p1.y(), p1.z());
	auto pp2 = Vec3(p2.x(), p2.y(), p2.z());
	auto pp3 = Vec3(p3.x(), p3.y(), p3.z());
	auto e1 = pp2 - pp1;
	auto e2 = pp3 - pp2;
	return e1.cross(e2) * 0.5;
}

} // namespace Euclid
