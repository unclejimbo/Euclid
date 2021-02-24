/**Geometric primitive generator.
 *
 * This package contains functions to generate geometric primitives.
 * @defgroup PkgPrimGen Primitive Generator
 * @ingroup PkgMeshUtil
 */
#pragma once

#include <CGAL/boost/graph/helpers.h>
#include <Euclid/MeshUtil/MeshDefs.h>

namespace Euclid
{
/** @{*/

/** Create a subdivision sphere.
 *
 *  Create a subdivision sphere from icosahedron
 *  using Loop subdivision method.
 */
template<typename Mesh>
void make_subdivision_sphere(Mesh& mesh,
                             const Point_3_t<Mesh>& center = { 0.0, 0.0, 0.0 },
                             typename Kernel_t<Mesh>::FT radius = 1.0,
                             int iterations = 4);

/** @}*/
} // namespace Euclid

#include "src/PrimitiveGenerator.cpp"
