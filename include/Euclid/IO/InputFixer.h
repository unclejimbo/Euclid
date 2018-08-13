/** Input fixer.
 *
 *  There is no quality assurance for 3D contents in the wild.
 *  This package provides some utilities to detect and fix
 *  some types of degeneracies.
 *  @defgroup PkgInputFixer Input Fixer
 *  @ingroup PkgIO
 */
#pragma once

#include <vector>

namespace Euclid
{
/** @{*/

/** Remove duplicate vertices.
 *
 *  @param positions A vector of point positions.
 *  @return Number of duplicate vertices.
 *
 *  **Note**
 *
 *  If you want to remove duplicate vertices of a mesh,
 *  use the other overload to fix indices.
 */
template<typename T>
size_t remove_duplicate_vertices(std::vector<T>& positions);

/** Remove duplicate vertices and fix indices.
 *
 *  @param positions A vector of point positions.
 *  @param indices A vector of point indices.
 *  @return Number of duplicate vertices.
 */
template<int N, typename T1, typename T2>
size_t remove_duplicate_vertices(std::vector<T1>& positions,
                                 std::vector<T2>& indices);

/** Remove duplicate faces of a mesh.
 *
 *  @param indices A vector point indices.
 *  @return Number of duplicate faces.
 */
template<int N, typename T>
size_t remove_duplicate_faces(std::vector<T>& indices);

/** Remove unreferenced vertices and fix indices.
 *
 *  @param positions A vector of point positions.
 *  @param indices A vector of point indices.
 *  @return Number of unreferenced vertices.
 */
template<int N, typename T1, typename T2>
size_t remove_unreferenced_vertices(std::vector<T1>& positions,
                                    std::vector<T2>& indices);

/** Remove degenerate faces of a mesh.
 *
 *  A face is degenerate when two consecutive edges are collinear.
 *
 *  @param positions A vector of point positions.
 *  @param indices A vector of point indices.
 *  @return Number of degenerate faces.
 */
template<int N, typename T1, typename T2>
size_t remove_degenerate_faces(const std::vector<T1>& positions,
                               std::vector<T2>& indices);

/** @}*/
} // namespace Euclid

#include "src/InputFixer.cpp"
