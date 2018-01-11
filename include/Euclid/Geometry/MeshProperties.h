/** Geometric and differential properties of a mesh.
 *
 *  This package contains functions to compute mesh properties.
 *  @defgroup PkgMeshProperties Mesh Properties
 *  @ingroup PkgGeometry
 */
#pragma once
#include <Eigen/SparseCore>

namespace Euclid
{
/** @{*/


/** Strategies to compute vertex normal.
 *
 */
enum class VertexNormal
{
	constant,
	face_area,
	incident_angle
};


/** Normal vector of a vertex on the mesh.
 *
 */
template<typename Mesh, typename FaceNormalMap>
typename boost::property_traits<FaceNormalMap>::value_type
vertex_normal(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	const FaceNormalMap& fnmap,
	const VertexNormal& weight = VertexNormal::incident_angle);


/** Strategies to compute vertex area.
 *
 */
enum class VertexArea
{
	barycentric,
	voronoi,
	mixed
};


/** Differential area of a vertex on the mesh.
 *
 */
template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
vertex_area(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	const VertexArea& method = VertexArea::mixed);


/** Edge length.
 *
 */
template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
edge_length(
	const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
	const Mesh& mesh);


/** Edge length.
 *
 */
template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
edge_length(
	const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
	const Mesh& mesh);


/** Squared edge length.
 *
 */
template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
squared_edge_length(
	const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
	const Mesh& mesh);


/** Squared edge length.
 *
 */
template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
squared_edge_length(
	const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
	const Mesh& mesh);


/** Normal of a face on the mesh.
 *
 */
template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::Vector_3
face_normal(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh);


/** Area of a face on the mesh.
 *
 */
template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
face_area(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh);


/** Gradient vector of a face on the mesh.
 *
 */
template<typename Mesh, typename VertexValueMap>
CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel>
gradient(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh,
	const VertexValueMap& vvmap);


/** Gradient filed of a mesh.
 *
 */
template<typename Mesh, typename VertexValueMap>
std::vector<CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel>>
gradient_field(
	const Mesh& mesh,
	const VertexValueMap& vvmap);


/** Laplace-beltrami vector of a vertex on the mesh.
 *
 */
template<typename Mesh>
CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel>
laplace_beltrami(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh);


/** Cotangent matrix of the mesh.
 *
 *  Laplace matrix = Mass matrix^-1 * Cotangent matrix.
 */
template<typename Mesh>
Eigen::SparseMatrix<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT>
cotangent_matrix(const Mesh& mesh);


/** Mass matrix of the mesh.
 *
 *  Laplace matrix = Mass matrix^-1 * Cotangent matrix.
 */
template<typename Mesh>
Eigen::SparseMatrix<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT>
mass_matrix(const Mesh& mesh, const VertexArea& method = VertexArea::mixed);


/** Gaussian curvature of a vertex on the mesh.
 *
 */
template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
gaussian_curvature(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh);


/** Mean curvature of a vertex on the mesh.
 *
 */
template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
mean_curvature(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh);

/** @}*/
} // namespace Euclid

#include "src/MeshProperties.cpp"
