#include <ostream>

namespace Euclid
{

template<typename Mesh>
Eigen::Vector3d inline facet_normal(const Mesh& mesh,
	const typename boost::graph_traits<const Mesh>::face_descriptor& f)
{
	auto he = halfedge(f, mesh);
	auto v1 = source(he, mesh);
	auto v2 = target(he, mesh);
	auto v3 = target(next(he, mesh), mesh);
	auto vpmap = get(CGAL::vertex_point, mesh);
	auto p1 = vpmap[v1];
	auto p2 = vpmap[v2];
	auto p3 = vpmap[v3];
	auto pp1 = Eigen::Vector3d(p1.x(), p1.y(), p1.z());
	auto pp2 = Eigen::Vector3d(p2.x(), p2.y(), p2.z());
	auto pp3 = Eigen::Vector3d(p3.x(), p3.y(), p3.z());
	auto e1 = (pp2 - pp1).normalized();
	auto e2 = (pp3 - pp2).normalized();

	if (e1.dot(e2) == 1.0 || e1.dot(e2) == -1.0) {
		std::cerr << "Degenerate facet, normal is set to zero!" << std::endl;
		return Eigen::Vector3d(0.0, 0.0, 0.0);
	}
	else {
		return e1.cross(e2).normalized();
	}
}

template<typename Mesh>
double inline facet_area(const Mesh& mesh,
	const typename boost::graph_traits<const Mesh>::facet_descriptor& f)
{
	auto he = halfedge(f, mesh);
	auto v1 = source(he, mesh);
	auto v2 = target(he, mesh);
	auto v3 = target(next(he, mesh), mesh);
	auto vpmap = get(CGAL::vertex_point, mesh);
	auto p1 = vpmap[v1];
	auto p2 = vpmap[v2];
	auto p3 = vpmap[v3];
	auto pp1 = Eigen::Vector3d(p1.x(), p1.y(), p1.z());
	auto pp2 = Eigen::Vector3d(p2.x(), p2.y(), p2.z());
	auto pp3 = Eigen::Vector3d(p3.x(), p3.y(), p3.z());
	auto e1 = pp2 - pp1;
	auto e2 = pp3 - pp2;
	return e1.cross(e2) * 0.5;
}

} // namespace Euclid
