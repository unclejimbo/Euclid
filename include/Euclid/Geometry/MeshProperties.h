/******************************************************************
* Package Overview:                                               *
* This package contains functions to compute properties of a mesh *
******************************************************************/
#pragma once

namespace Euclid
{

enum class NormalWeighting
{
	constant,
	face_area,
	incident_angle
};

template<typename Mesh, typename FaceNormalMap>
decltype(auto) vertex_normal(
	const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
	const Mesh& mesh,
	const FaceNormalMap& fnmap,
	const NormalWeighting& weight = NormalWeighting::incident_angle);

template<typename Mesh>
decltype(auto) edge_length(
	const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
	const Mesh& mesh);

template<typename Mesh>
decltype(auto) edge_length(
	const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
	const Mesh& mesh);

template<typename Mesh>
decltype(auto) face_normal(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh);

template<typename Mesh>
decltype(auto) face_area(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh);

} // namespace Euclid

#include "src/MeshProperties.cpp"
