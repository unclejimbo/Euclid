#pragma once

#include <boost/graph/graph_traits.hpp>
#include <Euclid/Topology/Chain.h>

namespace Euclid
{
/**@name Generator.
 *
 */
/**@{ @ingroup PkgTopology*/

/**Greedy homology generators.
 *
 * **Reference**
 *
 * [1] Erickson, J., Whittlesey, K.
 * Greedy Optimal Homotopy and Homology Generators.
 *
 * [2] Crane, K.
 * https://www.cs.cmu.edu/~kmcrane/Projects/LoopsOnSurfaces/
 */
template<typename Mesh>
VertexChains<Mesh> greedy_homology_generators(const Mesh& mesh,
                                              double accept = 0.05);

/**@}*/
} // namespace Euclid

#include "src/HomologyGenerator.cpp"
