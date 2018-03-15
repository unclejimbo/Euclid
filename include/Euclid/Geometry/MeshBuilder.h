/** Build a surface mesh.
 *
 *  This package provides functions to build a
 *  surface mesh for several common mesh structures.
 *  @defgroup PkgMeshBuilder MeshBuilder
 *  @ingroup PkgGeometry
 */
#pragma once

#include <type_traits>
#include <vector>

namespace Euclid
{
/** @{*/

template<int N, typename Mesh, typename FT, typename IT>
std::enable_if_t<std::is_arithmetic_v<FT>, void> build_mesh(
    Mesh& mesh,
    const std::vector<FT>& positions,
    const std::vector<IT>& indices);

template<int N, typename Mesh, typename Point_3, typename IT>
std::enable_if_t<!std::is_arithmetic_v<Point_3>, void> build_mesh(
    Mesh& mesh,
    const std::vector<Point_3>& points,
    const std::vector<IT>& indices);

/** @}*/
} // namespace Euclid

#include "src/MeshBuilder.cpp"
