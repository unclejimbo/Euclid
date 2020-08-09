
#pragma once

#include <vector>
#include <boost/graph/graph_traits.hpp>

namespace Euclid
{
/**@name Chain complex.
 *
 * Vertex paths and loops.
 */
/**@{ @ingroup PkgTopology*/

template<typename Mesh>
using VertexChain =
    std::vector<typename boost::graph_traits<Mesh>::vertex_descriptor>;

template<typename Mesh>
using VertexChains = std::vector<VertexChain<Mesh>>;

/**Test if the target is a valid chain.
 *
 * Consecutive vertices should be connected by a valid edge.
 */
template<typename Mesh>
bool is_chain(const Mesh& mesh, const VertexChain<Mesh>& chain);

/**Test if the target is a valid loop.
 *
 * A loop is a chain whose head and tail are connected.
 */
template<typename Mesh>
bool is_loop(const Mesh& mesh, const VertexChain<Mesh>& chain);

/**@}*/
} // namespace Euclid

#include "src/Chain.cpp"
