/**Type helpers for mesh related types.
 *
 * @defgroup PkgMeshDefs MeshDefs
 * @ingroup PkgMeshUtil
 */
#pragma once

#include <boost/graph/graph_traits.hpp>
#include <CGAL/boost/graph/properties.h>
#include <CGAL/Kernel_traits.h>

namespace Euclid
{
/** @{*/

template<typename Mesh>
using vpmap_t = typename boost::property_map<Mesh, CGAL::vertex_point_t>::type;

template<typename Mesh>
using vimap_t = typename boost::property_map<Mesh, CGAL::vertex_index_t>::type;

template<typename PMap>
using pmap_vt = typename boost::property_traits<PMap>::value_type;

template<typename Mesh>
using Kernel_t = typename CGAL::Kernel_traits<pmap_vt<vpmap_t<Mesh>>>::Kernel;

template<typename Mesh>
using Point_3_t = typename Kernel_t<Mesh>::Point_3;

template<typename Mesh>
using Vector_3_t = typename Kernel_t<Mesh>::Vector_3;

template<typename Mesh>
using FT_t = typename Kernel_t<Mesh>::FT;

template<typename Mesh>
using vertex_t = typename boost::graph_traits<Mesh>::vertex_descriptor;

template<typename Mesh>
using halfedge_t = typename boost::graph_traits<Mesh>::halfedge_descriptor;

template<typename Mesh>
using edge_t = typename boost::graph_traits<Mesh>::edge_descriptor;

template<typename Mesh>
using face_t = typename boost::graph_traits<Mesh>::face_descriptor;

/** @}*/
} // namespace Euclid
