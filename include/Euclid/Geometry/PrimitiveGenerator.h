/** Geometric primitive generator.
 *
 *  This package contains functions to generate geometric primitives.
 *  @defgroup PkgPrimGen Primitive Generator
 *  @ingroup PkgGeometry
 */
#pragma once

#include <CGAL/boost/graph/helpers.h>

namespace Euclid
{
/** @{*/

/** Create a subdivision sphere.
 *
 *  Create a subdivision sphere from icosahedron
 *  using Loop subdivision method.
 */
template<typename Mesh,
         typename Point_3 = typename boost::property_traits<
             typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
             value_type>
void make_subdivision_sphere(
    Mesh& mesh,
    const Point_3& center = { 0.0, 0.0, 0.0 },
    typename CGAL::Kernel_traits<Point_3>::Kernel::FT radius = 1.0,
    int iterations = 4);

/** @}*/
} // namespace Euclid

#include "src/PrimitiveGenerator.cpp"
