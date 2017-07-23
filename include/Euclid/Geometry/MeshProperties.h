/**************************************************
* Package Overview:                               *
* This package contains functions to compute      *
* geometric and differential properties of a mesh *
**************************************************/
#pragma once
#include <Eigen/SparseCore>

namespace Euclid
{

enum class VertexNormal
{
	constant,
	face_area,
	incident_angle
};

template<typename Mesh, typename FaceNormalMap>
typename boost::property_traits<FaceNormalMap>::value_type
vertex_normal(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	const FaceNormalMap& fnmap,
	const VertexNormal& weight = VertexNormal::incident_angle);

enum class VertexArea
{
	barycentric,
	voronoi,
	mixed
};

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
vertex_area(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	const VertexArea& method = VertexArea::mixed);

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
edge_length(
	const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
	const Mesh& mesh);

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
edge_length(
	const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
	const Mesh& mesh);

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
squared_edge_length(
	const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
	const Mesh& mesh);

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
squared_edge_length(
	const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
	const Mesh& mesh);

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::Vector_3
face_normal(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh);

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
face_area(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh);

template<typename Mesh>
CGAL::Vector_3<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel>
laplace_beltrami(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh);

// LaplaceMatrix = MassMatrix^-1 * CotangentMatrix
template<typename Mesh>
Eigen::SparseMatrix<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT>
cotangent_matrix(const Mesh& mesh);

template<typename Mesh>
Eigen::SparseMatrix<typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT>
mass_matrix(const Mesh& mesh, const VertexArea& method = VertexArea::mixed);

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
gaussian_curvature(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh);

template<typename Mesh>
typename CGAL::Kernel_traits<typename boost::property_traits<
	typename boost::property_map<Mesh, boost::vertex_point_t>::type>::value_type>::Kernel::FT
mean_curvature(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh);

} // namespace Euclid

#include "src/MeshProperties.cpp"
