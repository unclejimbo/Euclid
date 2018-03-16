/** Helper functions for mesh.
 *
 *  This package provides functions to process several common mesh
 *  data structures conforming to the concepts defined in the
 *  Boost Graph Library, including CGAL::Surface_mesh and CGAL::Polyhedron_3.
 *  For more details, see https://doc.cgal.org/latest/BGL/index.html.
 *
 *  @defgroup PkgMeshHelpers MeshHelpers
 *  @ingroup PkgGeometry
 */
#pragma once

#include <type_traits>
#include <vector>

namespace Euclid
{
/** @{*/

/** Create a mesh from raw positions and indices.
 *
 */
template<int N, typename Mesh, typename FT, typename IT>
std::enable_if_t<std::is_arithmetic_v<FT>, void> make_mesh(
    Mesh& mesh,
    const std::vector<FT>& positions,
    const std::vector<IT>& indices);

/** Create a mesh from points and indices.
 *
 */
template<int N, typename Mesh, typename Point_3, typename IT>
std::enable_if_t<!std::is_arithmetic_v<Point_3>, void> make_mesh(
    Mesh& mesh,
    const std::vector<Point_3>& points,
    const std::vector<IT>& indices);

/** Extract raw positions and indices from a mesh.
 *
 */
template<int N, typename Mesh, typename FT, typename IT>
std::enable_if_t<std::is_arithmetic_v<FT>, void>
extract_mesh(Mesh& mesh, std::vector<FT>& positions, std::vector<IT>& indices);

/** Extract points and indices from a mesh.
 *
 */
template<int N, typename Mesh, typename Point_3, typename IT>
std::enable_if_t<!std::is_arithmetic_v<Point_3>, void> extract_mesh(
    Mesh& mesh,
    std::vector<Point_3>& points,
    std::vector<IT>& indices);

/** @}*/
} // namespace Euclid

#include "src/MeshHelpers.cpp"
