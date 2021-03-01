/**Helper functions for mesh.
 *
 * @defgroup PkgMeshHelpers MeshHelpers
 * @ingroup PkgMeshUtil
 */
#pragma once

#include <utility>
#include <vector>
#include <Euclid/MeshUtil/MeshDefs.h>

namespace Euclid
{
/** @{*/

/** Find the n-ring vertices given a target vertex.
 *
 */
template<typename Mesh>
std::vector<vertex_t<Mesh>> nring_vertices(vertex_t<Mesh> target,
                                           const Mesh& mesh,
                                           unsigned n = 1);

/** Find the common edge between two faces.
 *
 *  Return the halfedges of the common edge. The halfedges are invalid handles
 *  if no common edge found.
 */
template<typename Mesh>
std::pair<halfedge_t<Mesh>, halfedge_t<Mesh>> find_common_edge(const Mesh& mesh,
                                                               face_t<Mesh> f1,
                                                               face_t<Mesh> f2);

/** @}*/
} // namespace Euclid

#include "src/MeshHelpers.cpp"
