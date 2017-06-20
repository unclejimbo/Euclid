/**********************************************************
* Package Overview:                                       *
* This package contains helper functions for Surface_mesh *
**********************************************************/
#pragma once
#include <CGAL/Surface_mesh.h>
#include <Eigen/Dense>
#include <vector>

namespace Euclid
{

template<typename Point_3>
bool build_surface_mesh(
	const std::vector<Point_3>& vertices,
	const std::vector<unsigned>& indices,
	CGAL::Surface_mesh<Point_3>& mesh);

template<typename Point_3, typename FT>
bool build_surface_mesh(
	const std::vector<Eigen::Matrix<FT, 3, 1>>& vertices,
	const std::vector<unsigned>& indices,
	CGAL::Surface_mesh<Point_3>& mesh);

} // namespace Euclid

#include "src/Surface_mesh.cpp"
