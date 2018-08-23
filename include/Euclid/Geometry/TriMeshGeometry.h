/** Discrete (differential) geometry on a triangle mesh.
 *
 *  This package contains functions to compute geometric properties and
 *  differential operators on a triangle mesh.
 *
 *  **References**
 *
 *  [1] Botsch M, Kobbelt L, Pauly M, et al.
 *  Polygon Mesh Processing.
 *  CRC press, 2010.
 *
 *  [2] Crane K.
 *  Discrete Differential Geometry: an Applied Introduction.
 *
 *  [3] Meyer M, Schroder P, Barrr A H.
 *  Discrete Differential-Geometry Operators for Triangulated 2-Manifolds.
 *  Visualization and Mathematics III, 2003.
 *
 *  [4] Zhang H, Van Kaick O, Dyer R.
 *  Spectral Mesh Processing.
 *  Computer Graphics Forum, 2010.
 *
 *  @defgroup PkgTriMeshGeometry TriMeshGeometry
 *  @ingroup PkgGeometry
 */
#pragma once

#include <Eigen/SparseCore>
#include <CGAL/boost/graph/properties.h>

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
 *  @tparam Mesh Mesh type.
 *  @tparam Vector_3 Optional, derived from Mesh.
 *
 *  **Note**
 *
 *  This function will compute face normals for all the incident triangles
 *  of vertex v. To avoid wasting computation, provide face normals using the
 *  other overload method.
 *
 *  @sa VertexNormal
 */
template<typename Mesh,
         typename Vector_3 =
             typename CGAL::Kernel_traits<typename boost::property_traits<
                 typename boost::property_map<Mesh, boost::vertex_point_t>::
                     type>::value_type>::Kernel::Vector_3>
Vector_3 vertex_normal(
    typename boost::graph_traits<const Mesh>::vertex_descriptor v,
    const Mesh& mesh,
    const VertexNormal& weight = VertexNormal::incident_angle);

/** Normal vector of a vertex on the mesh.
 *
 *  Compute vertex normal from the incident face normals.
 *  Choose a weighting strategy of face normals from VertexNormal.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam FaceNormalMap A property map.
 *  @tparam Vector_3 Optional, derived from Mesh.
 *
 *  @sa VertexNormal
 */
template<typename Mesh,
         typename FaceNormalMap,
         typename Vector_3 =
             typename boost::property_traits<FaceNormalMap>::value_type>
Vector_3 vertex_normal(
    typename boost::graph_traits<const Mesh>::vertex_descriptor v,
    const Mesh& mesh,
    const FaceNormalMap& fnmap,
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
 *  @tparam Mesh Mesh type.
 *  @tparam T Optional, derived from Mesh.
 *
 *  @sa VertexArea
 */
template<
    typename Mesh,
    typename T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                  value_type>::Kernel::FT>
T vertex_area(typename boost::graph_traits<const Mesh>::vertex_descriptor v,
              const Mesh& mesh,
              const VertexArea& method = VertexArea::mixed_voronoi);

/** Mass matrix of the mesh.
 *
 *  The mass matrix is simply the vertex areas of all the vertices of a mesh
 *  written in a diagonal matrix, which serves as the local averaging region
 *  commonly used in discrete differential geometry.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam T Optional, derived from Mesh.
 *
 *  @sa VertexArea, vertex_area(), laplacian_matrix()
 */
template<
    typename Mesh,
    typename T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                  value_type>::Kernel::FT>
Eigen::SparseMatrix<T> mass_matrix(
    const Mesh& mesh,
    const VertexArea& method = VertexArea::mixed_voronoi);

/** Edge length.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam T Optional, derived from Mesh.
 */
template<
    typename Mesh,
    typename T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                  value_type>::Kernel::FT>
T edge_length(typename boost::graph_traits<const Mesh>::halfedge_descriptor he,
              const Mesh& mesh);

/** Edge length.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam T Optional, derived from Mesh.
 */
template<
    typename Mesh,
    typename T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                  value_type>::Kernel::FT>
T edge_length(typename boost::graph_traits<const Mesh>::edge_descriptor e,
              const Mesh& mesh);

/** Squared edge length.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam T Optional, derived from Mesh.
 */
template<
    typename Mesh,
    typename T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                  value_type>::Kernel::FT>
T squared_edge_length(
    typename boost::graph_traits<const Mesh>::halfedge_descriptor he,
    const Mesh& mesh);

/** Squared edge length.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam T Optional, derived from Mesh.
 */
template<
    typename Mesh,
    typename T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                  value_type>::Kernel::FT>
T squared_edge_length(
    typename boost::graph_traits<const Mesh>::edge_descriptor e,
    const Mesh& mesh);

/** Normal of a face on the mesh.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam Vector_3 Optional, derived from Mesh.
 */
template<typename Mesh,
         typename Vector_3 =
             typename CGAL::Kernel_traits<typename boost::property_traits<
                 typename boost::property_map<Mesh, boost::vertex_point_t>::
                     type>::value_type>::Kernel::Vector_3>
Vector_3 face_normal(
    typename boost::graph_traits<const Mesh>::face_descriptor f,
    const Mesh& mesh);

/** Area of a face on the mesh.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam T Optional, derived from Mesh.
 */
template<
    typename Mesh,
    typename T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                  value_type>::Kernel::FT>
T face_area(typename boost::graph_traits<const Mesh>::face_descriptor f,
            const Mesh& mesh);

/** Barycenter/centroid of a face on the mesh.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam Point_3 Optional, derived from Mesh.
 */
template<typename Mesh,
         typename Point_3 = typename boost::property_traits<
             typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
             value_type>
Point_3 barycenter(typename boost::graph_traits<const Mesh>::face_descriptor,
                   const Mesh& mesh);

/** Strategies to compute Laplacian.
 *
 *  @sa laplacian_matrix()
 */
enum class Laplacian
{
    uniform,  /**< Simple graph Laplacian.*/
    cotangent /**< Cotangent formula.*/
};

/** Laplacian operator on a mesh.
 *
 *  The Laplacian matrix forms the basis for all kinds of geometry
 *  processing algorithms, you name it. There exist many discretizations for
 *  the Laplace-Beltrami operator, and here you can choose between graph
 *  Laplacian and geometric Laplacian which are specified in Laplacian.
 *
 *  Other than that, you can further choose to multiply the pure Laplacian
 *  matrix with the mass matrix, which serves as a local average. Now suppose we
 *  use $L$ for the Laplacian matrix, and $M$ for the mass matrix.
 *
 *  - @f$L@f$ alone is symmetric but not invariant under meshing.
 *  - @f$M^{-1}L@f$ is robust to meshing quality but breaks the symmetric
 *  property.
 *  - @f$M^{-1/2}LM^{-1/2}@f$ is both symmetric and robust.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam T Optional, derived from Mesh.
 *
 *  @sa Laplacian, mass_matrix()
 */
template<
    typename Mesh,
    typename T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                  value_type>::Kernel::FT>
Eigen::SparseMatrix<T> laplacian_matrix(
    const Mesh& mesh,
    const Laplacian& method = Laplacian::cotangent);

/** Gaussian curvature of a vertex on the mesh.
 *
 *  Discrete Gaussian curvature using the angle deficit method.
 *
 *  @tparam Mesh Mesh type.
 *  @tparam T Optional, derived from Mesh.
 */
template<
    typename Mesh,
    typename T = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                  value_type>::Kernel::FT>
T gaussian_curvature(
    typename boost::graph_traits<const Mesh>::vertex_descriptor v,
    const Mesh& mesh);

/** @}*/
} // namespace Euclid

#include "src/TriMeshGeometry.cpp"
