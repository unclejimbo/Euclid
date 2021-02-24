#pragma once

#include <Euclid/Topology/Chain.h>
#include <Euclid/MeshUtil/MeshDefs.h>

namespace Euclid
{
/**@name Generator.
 *
 */
/**@{ @ingroup PkgTopology*/

/**Greedy homotopy generators.
 *
 * **Reference**
 *
 * [1] Erickson, J., Whittlesey, K.
 * Greedy Optimal Homotopy and Homology Generators.
 */
template<typename Mesh>
VertexChains<Mesh> greedy_homotopy_generators(const Mesh& mesh,
                                              vertex_t<Mesh> v);

/**@}*/
} // namespace Euclid

#include "src/HomotopyGenerator.cpp"
