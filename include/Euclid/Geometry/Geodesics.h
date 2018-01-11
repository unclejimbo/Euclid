/** Geodesic distance.
 *
 *  This package contains algorithms for computing geodesic distance.
 *  @defgroup PkgGeodesics Geodesics
 *  @ingroup PkgGeometry
 */
#pragma once
#include <vector>

namespace Euclid
{
/** @{*/

/** Single source approximate geodesic distance using the heat method.
 *
 *  #### Reference
 *  Crane K, Weischedel C, Wardetzky M.
 *  Geodesics in heat: A new approach to computing distance based on heat flow[J].
 *  ACM Transactions on Graphics (TOG), 2013, 32(5): 152.
 */
template<typename Mesh>
bool heat_method(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	std::vector<typename CGAL::Kernel_traits<typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>::type>
		::value_type>::Kernel::FT>& geodesics);

/** @}*/
} // namespace Euclid

#include "src/HeatMethod.cpp"
