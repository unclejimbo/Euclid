/**Convert to and from CGAL mesh.
 *
 * This package provides functions to make CGAL mesh from raw buffers and vice
 * versa.
 *
 * Conceptually, the CGAL mesh type should conform to the concepts defined in
 * the Boost Graph Library, including CGAL::Surface_mesh, CGAL::Polyhedron_3 and
 * OpenMesh. For more details, see https://doc.cgal.org/latest/BGL/index.html.
 *
 * @defgroup PkgCGALMesh CGAL Mesh
 * @ingroup PkgMeshUtil
 */
#pragma once

#include <type_traits>
#include <vector>
#include <Euclid/MeshUtil/MeshDefs.h>

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
std::enable_if_t<std::is_arithmetic_v<FT>, void> extract_mesh(
    const Mesh& mesh,
    std::vector<FT>& positions,
    std::vector<IT>& indices);

/** Extract points and indices from a mesh.
 *
 */
template<int N, typename Mesh, typename Point_3, typename IT>
std::enable_if_t<!std::is_arithmetic_v<Point_3>, void> extract_mesh(
    const Mesh& mesh,
    std::vector<Point_3>& points,
    std::vector<IT>& indices);

/** @}*/
} // namespace Euclid

#include "src/CGALMesh.cpp"
