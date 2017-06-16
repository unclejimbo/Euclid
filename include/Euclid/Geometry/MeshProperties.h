/******************************************************************
* Package Overview:                                               *
* This package contains functions to compute properties of a mesh *
******************************************************************/
#pragma once
#include <Eigen/Dense>

namespace Euclid
{

template<typename Mesh>
Eigen::Vector3d facet_normal(const Mesh& mesh,
	const typename boost::graph_traits<const Mesh>::face_descriptor& f);

template<typename Mesh>
double facet_area(const Mesh& mesh,
	const typename boost::graph_traits<const Mesh>::face_descriptor& f);

} // namespace Euclid

#include "src/MeshProperties.cpp"
