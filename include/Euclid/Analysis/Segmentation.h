#pragma once
/**********************************************************
* Package Overview:                                       *
* Segmentation of 3D shape including mesh and point cloud *
**********************************************************/
namespace Euclid
{

// Random walk segmentation for Polyhedron_3
template<typename Polyhedron_3>
void random_walk_segmentation(
	const Polyhedron_3& mesh,
	std::vector<int>& seed_indices,
	std::vector<int>& facet_class);

// Random walk segmentation for point cloud with normals
template<typename FT>
void random_walk_segmentation(
	const std::vector<Eigen::Matrix<FT, 3, 1>>& vertices,
	const std::vector<Eigen::Matrix<FT, 3, 1>>& normals,
	std::vector<int>& seed_indices,
	std::vector<int>& point_class);

} // namespace Euclid

#include "src/RandWalkSeg.cpp"
