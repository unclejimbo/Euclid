/***********************************************
* Package Overview:                            *
* Discrete geodesic shortest path and distance *
***********************************************/
#pragma once
#include <vector>

namespace Euclid
{

// Single-source approximated geodesic distance using heat method
template<typename Mesh>
std::vector<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT>
heat_method(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh);

} // namespace Euclid

#include "src/HeatMethod.cpp"
