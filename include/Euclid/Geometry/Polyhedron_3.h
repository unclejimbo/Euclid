/** CGAL::Polyhedron_3 utilities.
 *
 *  This package contains utilities to work with the
 *  CGAL::Polyhedron_3 package.
 *  @defgroup PkgPolyhedron Polyhedron_3
 *  @ingroup PkgGeometry
 */
#pragma once
#include <type_traits>
#include <CGAL/Polyhedron_3.h>

namespace Euclid
{
/** @{*/

/** Build CGAL::Polyhedron_3 from positions and indices.
 *
 */
template<int N,
         typename Kernel,
         typename PT,
         typename IT,
         typename Enable = std::enable_if_t<std::is_arithmetic_v<PT>, void>>
void build_polyhedron_3(CGAL::Polyhedron_3<Kernel>& mesh,
                        const std::vector<PT>& positions,
                        const std::vector<IT>& indices);

/** Build CGAL::Polyhedron_3<Kernel> from points and indices.
 *
 */
template<int N, typename Kernel, typename IT>
void build_polyhedron_3(CGAL::Polyhedron_3<Kernel>& mesh,
                        const std::vector<typename Kernel::Point_3>& points,
                        const std::vector<IT>& indices);

/** @}*/
} // namespace Euclid

#include "src/Polyhedron_3.cpp"
