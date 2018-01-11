#include <Euclid/Geometry/MeshProperties.h>
#include <Euclid/Math/Vector.h>
#include <Euclid/Util/Assert.h>
#include <Eigen/Dense>
#include <Eigen/SparseCholesky>
#include <unordered_map>
#include <iostream>

namespace Euclid
{

template<typename Mesh>
bool heat_method(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	std::vector<typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>
		::value_type>::Kernel::FT>& geodesics)
{
	using FT = typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT;
	using SpMat = Eigen::SparseMatrix<FT>;
	using VVMap = std::unordered_map<boost::graph_traits<Mesh>::vertex_descriptor, FT>;
	auto vpmap = get(boost::vertex_point, mesh);
	auto vimap = get(boost::vertex_index, mesh);
	auto fimap = get(boost::face_index, mesh);
	geodesics.resize(num_vertices(mesh));

	// Construct the heat equation
	FT time_interval = 0.0;
	for (const auto& e : edges(mesh)) {
		time_interval += edge_length(e, mesh);
	}
	time_interval /= num_edges(mesh);
	SpMat cot_mat = cotangent_matrix(mesh);
	SpMat mass_mat = mass_matrix(mesh);
	SpMat heat_mat = mass_mat - time_interval * cot_mat;
	Eigen::Matrix<FT, Eigen::Dynamic, 1> delta =
		Eigen::Matrix<FT, Eigen::Dynamic, 1>::Zero(num_vertices(mesh));
	delta(vimap[v], 0) = vertex_area(v, mesh);

	// Solve the heat flow
	Eigen::SimplicialLDLT<SpMat> heat_solver;
	heat_solver.compute(heat_mat);
	if (heat_solver.info() != Eigen::Success) {
		std::cerr << "Unable to solve the heat flow" << std::endl;
		return false;
	}
	Eigen::Matrix<FT, Eigen::Dynamic, 1> heat = heat_solver.solve(delta);
	if (heat_solver.info() != Eigen::Success) {
		std::cerr << "Unable to solve the heat flow" << std::endl;
		return false;
	}

	// Evaluate the gradient field
	VVMap vertex_values;
	int idx = 0;
	FT max_heat = 0.0;
	for (const auto& v : vertices(mesh)) {
		EASSERT(heat(idx) <= 1.0 && heat(idx) >= 0.0);
		max_heat = std::max(max_heat, heat(idx));
		vertex_values.emplace(v, heat(idx++));
	}
	EASSERT(max_heat == heat(vimap[v]));
	auto gradients = gradient_field(mesh, boost::const_associative_property_map<VVMap>(vertex_values));
	for (auto& g : gradients) {
		normalize(g);
		g = -g;
	}

	// Construct the poisson equation
	Eigen::Matrix<FT, Eigen::Dynamic, 1> divs(num_vertices(mesh));
	for (const auto& v : vertices(mesh)) {
		FT divergence = 0.0;
		auto p = vpmap[v];
		for (const auto& he : halfedges_around_target(v, mesh)) {
			auto f = face(he, mesh);
			auto g = gradients[fimap[f]];
			auto vi = source(he, mesh);
			auto vj = target(next(he, mesh), mesh);
			auto pi = vpmap[vi];
			auto pj = vpmap[vj];
			divergence += cotangent(p, pi, pj) * ((pj - p) * g) +
				cotangent(p, pj, pi) * ((pi - p) * g);
		}
		divs(vimap[v], 0) = divergence * 0.5;
	}

	// Solve the poisson equation
	Eigen::SimplicialLDLT<SpMat> poisson_solver;
	poisson_solver.compute(cot_mat);
	if (poisson_solver.info() != Eigen::Success) {
		std::cerr << "Unable to solve the poisson equation" << std::endl;
		return false;
	}
	Eigen::Matrix<FT, Eigen::Dynamic, 1> geod = poisson_solver.solve(divs);
	if (poisson_solver.info() != Eigen::Success) {
		std::cerr << "Unable to solve the poisson equation" << std::endl;
		return false;
	}

	for (size_t i = 0; i < num_vertices(mesh); ++i) {
		geodesics[i] = geod(i, 0) - geod(vimap[v], 0);
		EASSERT(geodesics[i] >= 0.0);
	}
	return true;
}

} // namespace Euclid
