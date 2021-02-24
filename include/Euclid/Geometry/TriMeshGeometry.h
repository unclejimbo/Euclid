/** Discrete (differential) geometry on a triangle mesh.
 *
 *  This package contains functions to compute geometric properties and
 *  differential operators on a triangle mesh.
 *
 *  Note that, edge_length() and squared_edge_length() are partial specialized
 *  for CGAL::Dual<Mesh> which approximate the length with barycenters.
 *
 *  **References**
 *
 *  [1] Botsch, M., Kobbelt L., Pauly M., et al.
 *  Polygon Mesh Processing.
 *  CRC press, 2010.
 *
 *  [2] Crane, K.
 *  Discrete Differential Geometry: an Applied Introduction.
 *
 *  [3] Meyer, M., Schroder, P., Barrr, A. H.
 *  Discrete Differential-Geometry Operators for Triangulated 2-Manifolds.
 *  Visualization and Mathematics III, 2003.
 *
 *  [4] Zhang, H., Van Kaick, O., Dyer, R.
 *  Spectral Mesh Processing.
 *  Computer Graphics Forum, 2010.
 *
 *  @defgroup PkgTriMeshGeometry TriMeshGeometry
 *  @ingroup PkgGeometry
 */
#pragma once

#include <tuple>
#include <Eigen/SparseCore>
#include <Euclid/MeshUtil/MeshDefs.h>

namespace Euclid
{
/** @{*/

/** Strategies to compute vertex normal.
 *
 *  @sa vertex_normal()
 */
enum class VertexNormal
{
    /** Assign equal weights to neighboring face normals.
     *  This is the fastest yet most inaccurate strategy.
     */
    uniform,
    /** Uses face area as weight for face normals.
     *  This gives good balance of speed and quality.
     */
    face_area,
    /** Uses the incident angle of a face to the
     *  vertex as weight for the face normal. It often gives the best result
     *  but is computationally expensive.
     */
    incident_angle
};

/** Normal vector of a vertex on the mesh.
 *
 *  Compute vertex normal from the incident face normals.
 *  Choose a weighting strategy of face normals from VertexNormal.
 *
 *  **Note**
 *
 *  This function will compute face normals for all the incident triangles
 *  of vertex v. To avoid wasting computation, provide face normals using the
 *  other overload method.
 *
 *  @sa VertexNormal
 */
template<typename Mesh>
Vector_3_t<Mesh> vertex_normal(
    vertex_t<Mesh> v,
    const Mesh& mesh,
    const VertexNormal& weight = VertexNormal::incident_angle);

/** Normal vector of a vertex on the mesh.
 *
 *  Compute vertex normal from the incident face normals.
 *  Choose a weighting strategy of face normals from VertexNormal.
 *
 *  @sa VertexNormal
 */
template<typename Mesh>
Vector_3_t<Mesh> vertex_normal(
    vertex_t<Mesh> v,
    const Mesh& mesh,
    const std::vector<Vector_3_t<Mesh>>& face_normals,
    const VertexNormal& weight = VertexNormal::incident_angle);

/** Normal vectors of all vertices on the mesh.
 *
 *  Compute vertex normal from the incident face normals.
 *  Choose a weighting strategy of face normals from VertexNormal.
 *
 *  @sa VertexNormal
 */
template<typename Mesh>
std::vector<Vector_3_t<Mesh>> vertex_normals(
    const Mesh& mesh,
    const std::vector<Vector_3_t<Mesh>>& face_normals,
    const VertexNormal& weight = VertexNormal::incident_angle);

/** Strategies to compute vertex area.
 *
 *  @sa vertex_area()
 *  @sa http://www.alecjacobson.com/weblog/?p=1146
 */
enum class VertexArea
{
    /** Barycentric cell.
     *  Uses the barycenters of incident triangles as the vertices of the local
     *  cell. In other words the area is 1/3 of the area of the incident
     *  triangles. This area is fast and positive but sensitive to mesh
     *  triangulations.
     */
    barycentric,
    /** Voronoi cell.
     *  Uses the circumcenters of incident triangles as the vertices of the
     *  local cell, which gives tighter error bounds. However the circumcenters
     *  might appear outside the triangles, and the signed area might be
     *  negative.
     */
    voronoi,
    /** A Mixed voronoi cell.
     *  Ensures the vertices of the local voronoi cell are within the triangles
     *  by replacing circumcenters which are outside of the triangles by the
     *  midpoints of the opposite edges of the obtuse vertices. The signed area
     *  is guaranteed to be positive, but not as smooth as pure voronoi cells.
     */
    mixed_voronoi
};

/** Area of a vertex on the mesh.
 *
 *  Compute a small area around the vertex as the local average of the
 *  integral of the differential area dA.
 *  In the discrete settings, this involves constructing a small cell
 *  around the vertex and use its area as the local averaging region.
 *  Choose one type of region as specified in VertexArea.
 *  Only 1-ring neighborhood is considered in this function.
 *
 *  @sa VertexArea
 */
template<typename Mesh>
FT_t<Mesh> vertex_area(vertex_t<Mesh> v,
                       const Mesh& mesh,
                       const VertexArea& method = VertexArea::mixed_voronoi);

/** Areas of all vertices on the mesh.
 *
 *  Compute a small area around the vertex as the local average of the
 *  integral of the differential area dA.
 *  In the discrete settings, this involves constructing a small cell
 *  around the vertex and use its area as the local averaging region.
 *  Choose one type of region as specified in VertexArea.
 *  Only 1-ring neighborhood is considered in this function.
 *
 *  @sa VertexArea
 */
template<typename Mesh>
std::vector<FT_t<Mesh>> vertex_areas(
    const Mesh& mesh,
    const VertexArea& method = VertexArea::mixed_voronoi);

/** Edge length.
 *
 */
template<typename Mesh>
FT_t<Mesh> edge_length(halfedge_t<Mesh> he, const Mesh& mesh);

/** Edge length.
 *
 */
template<typename Mesh>
FT_t<Mesh> edge_length(edge_t<Mesh> e, const Mesh& mesh);

/** Edge lengths.
 *
 */
template<typename Mesh>
std::vector<FT_t<Mesh>> edge_lengths(const Mesh& mesh);

/** Squared edge length.
 *
 */
template<typename Mesh>
FT_t<Mesh> squared_edge_length(halfedge_t<Mesh> he, const Mesh& mesh);

/** Squared edge length.
 *
 */
template<typename Mesh>
FT_t<Mesh> squared_edge_length(edge_t<Mesh> e, const Mesh& mesh);

/** Squared edge lengths.
 *
 */
template<typename Mesh>
std::vector<FT_t<Mesh>> squared_edge_lengths(const Mesh& mesh);

/** Normal of a face on the mesh.
 *
 */
template<typename Mesh>
Vector_3_t<Mesh> face_normal(face_t<Mesh> f, const Mesh& mesh);

/** Normals of all faces on the mesh.
 *
 */
template<typename Mesh>
std::vector<Vector_3_t<Mesh>> face_normals(const Mesh& mesh);

/** Area of a face on the mesh.
 *
 */
template<typename Mesh>
FT_t<Mesh> face_area(face_t<Mesh> f, const Mesh& mesh);

/** Areas of all faces on the mesh.
 *
 */
template<typename Mesh>
std::vector<FT_t<Mesh>> face_areas(const Mesh& mesh);

/** Barycenter/centroid of a face on the mesh.
 *
 */
template<typename Mesh>
Point_3_t<Mesh> barycenter(face_t<Mesh> f, const Mesh& mesh);

/** Barycenters/centroids of all faces on the mesh.
 *
 */
template<typename Mesh>
std::vector<Point_3_t<Mesh>> barycenters(const Mesh& mesh);

/** Gaussian curvature of a vertex on the mesh.
 *
 *  Discrete Gaussian curvature using the angle deficit method.
 */
template<typename Mesh>
FT_t<Mesh> gaussian_curvature(vertex_t<Mesh> v, const Mesh& mesh);

/** Gaussian curvatures of all vertices on the mesh.
 *
 *  Discrete Gaussian curvature using the angle deficit method.
 */
template<typename Mesh>
std::vector<FT_t<Mesh>> gaussian_curvatures(const Mesh& mesh);

/** Adjacency matrix of the mesh.
 *
 *  Return the unweighted adjacency matrix as well as the degree matrix of a
 *  mesh.
 */
template<typename Mesh>
std::tuple<Eigen::SparseMatrix<FT_t<Mesh>>, Eigen::SparseMatrix<FT_t<Mesh>>>
adjacency_matrix(const Mesh& mesh);

/**Graph laplacian matrix of the mesh.
 *
 */
template<typename Mesh>
Eigen::SparseMatrix<FT_t<Mesh>> graph_laplacian_matrix(const Mesh& mesh);

/**Cotangent weight associated with an edge.
 *
 */
template<typename Mesh>
FT_t<Mesh> cotangent_weight(halfedge_t<Mesh> he, const Mesh& mesh);

/**Cotangent weight associated with an edge.
 *
 */
template<typename Mesh>
FT_t<Mesh> cotangent_weight(edge_t<Mesh> e, const Mesh& mesh);

/** Cotangent matrix of the mesh.
 *
 *  The cotangent matrix is a discretization of the Laplacian-Beltrami operator.
 *  It is a symemtric, positive semi-definitive matrix. Assume that the
 *  cotangent matrix is @f$L@f$, and the mass matrix is @f$A@f$, here're some
 *  variants of discrete LB operator.
 *
 *  - @f$L@f$, alone is not invariant to meshing
 *  - @f$A^{-1}L@f$, accurate discretization but not symmetric
 *  - @f$A^{-1/2}LA^{-1/2}@f$, symmetric and mesh independent
 *
 *  **Note**
 *
 *  It's kinda arbitary in the literature when people refer to the Laplacian
 *  matrix, especially the sign. In this particular implementation, the diagonal
 *  elements are positive and the others are negative, thus forming a positive
 *  smei-definitive matrix.
 *
 *  @sa mass_matrix
 */
template<typename Mesh>
Eigen::SparseMatrix<FT_t<Mesh>> cotangent_matrix(const Mesh& mesh);

/** Mass matrix of the mesh.
 *
 *  The mass matrix is simply the vertex areas of all the vertices of a mesh
 *  written in a diagonal matrix, which serves as the local averaging region
 *  commonly used in discrete differential geometry.
 *
 *  @sa VertexArea, vertex_area, cotangent_matrix
 */
template<typename Mesh>
Eigen::SparseMatrix<FT_t<Mesh>> mass_matrix(
    const Mesh& mesh,
    const VertexArea& method = VertexArea::mixed_voronoi);

/** @}*/
} // namespace Euclid

#include "src/TriMeshGeometry.cpp"
