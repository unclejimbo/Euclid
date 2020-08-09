#pragma once

#include <boost/graph/graph_traits.hpp>
#include <Euclid/Topology/Chain.h>

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
VertexChains<Mesh> greedy_homotopy_generators(
    const Mesh& mesh,
    typename boost::graph_traits<const Mesh>::vertex_descriptor v);

/**@}*/
} // namespace Euclid

#include "src/HomotopyGenerator.cpp"
