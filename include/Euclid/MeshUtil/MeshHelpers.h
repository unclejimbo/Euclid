/**Helper functions for mesh.
 *
 * This package provides functions to process several common mesh data
 * structures.
 *
 * Conceptually, the Mesh type should conform to the concepts defined in the
 * Boost Graph Library, including CGAL::Surface_mesh, CGAL::Polyhedron_3 and
 * OpenMesh. For more details, see https://doc.cgal.org/latest/BGL/index.html.
 *
 * And the matrix representation should primarily be used along with libigl.
 *
 * @defgroup PkgMeshHelpers MeshHelpers
 * @ingroup PkgMeshUtil
 */
#pragma once

#include <type_traits>
#include <utility>
#include <vector>
#include <Eigen/Core>
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

/** Create a matrix representation of mesh from positions and indices.
 *
 */
template<int N, typename DerivedV, typename DerivedF, typename FT, typename IT>
void make_mesh(Eigen::PlainObjectBase<DerivedV>& V,
               Eigen::PlainObjectBase<DerivedF>& F,
               const std::vector<FT>& positions,
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

/** Extract positions and indices from mesh matrices.
 *
 */
template<int N, typename DerivedV, typename DerivedF, typename FT, typename IT>
void extract_mesh(const Eigen::MatrixBase<DerivedV>& V,
                  const Eigen::MatrixBase<DerivedF>& F,
                  std::vector<FT>& positions,
                  std::vector<IT>& indices);

/** Extract positions from position matrix.
 *
 *  This function is helpful when the mesh positions are changed but topology
 *  remains the same.
 */
template<typename DerivedV, typename FT>
void extract_mesh(const Eigen::MatrixBase<DerivedV>& V,
                  std::vector<FT>& positions);

/** Find the n-ring vertices given a target vertex.
 *
 */
template<typename Mesh>
std::vector<vertex_t<Mesh>> nring_vertices(vertex_t<Mesh> target,
                                           const Mesh& mesh,
                                           unsigned n = 1);

/** Find the common edge between two faces.
 *
 *  Return the halfedges of the common edge. The halfedges are invalid handles
 *  if no common edge found.
 */
template<typename Mesh>
std::pair<halfedge_t<Mesh>, halfedge_t<Mesh>> find_common_edge(const Mesh& mesh,
                                                               face_t<Mesh> f1,
                                                               face_t<Mesh> f2);

/** @}*/
} // namespace Euclid

#include "src/MeshHelpers.cpp"
