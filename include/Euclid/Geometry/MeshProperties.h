/******************************************************************
* Package Overview:                                               *
* This package contains functions to compute properties of a mesh *
******************************************************************/
#pragma once

namespace Euclid
{

template<typename Mesh>
decltype(auto) edge_length(
	const typename boost::graph_traits<const Mesh>::halfedge_descriptor& he,
	const Mesh& mesh);

template<typename Mesh>
decltype(auto) edge_length(
	const typename boost::graph_traits<const Mesh>::edge_descriptor& e,
	const Mesh& mesh);

template<typename Mesh>
decltype(auto) facet_normal(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh);

template<typename Mesh>
decltype(auto) facet_area(
	const typename boost::graph_traits<const Mesh>::face_descriptor& f,
	const Mesh& mesh);

} // namespace Euclid

#include "src/MeshProperties.cpp"
