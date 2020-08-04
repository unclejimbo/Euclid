#pragma once

namespace Euclid
{
/**@name Mesh topology.
 *
 * Basic topological properties and operations.
 */
/**@{ @ingroup PkgTopology*/

/**The number of boundaries/holes.
 *
 */
template<typename Mesh>
int num_bondaries(const Mesh& mesh);

/**Euler characteristic.
 *
 * @f$X = V - E + F@f$
 */
template<typename Mesh>
int euler_characteristic(const Mesh& mesh);

/**Genus.
 *
 * @f$2G = 2 - X - B@f$
 */
template<typename Mesh>
int genus(const Mesh& mesh);

/**@}*/
} // namespace Euclid

#include "src/MeshTopology.cpp"
