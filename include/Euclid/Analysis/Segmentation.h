/**********************************************************
* Package Overview:                                       *
* Segmentation of 3D shape including mesh and point cloud *
**********************************************************/
#pragma once

namespace Euclid
{

// Random walk segmentation for mesh
template<typename Mesh>
void random_walk_segmentation(
	const Mesh& mesh,
	std::vector<int>& seed_indices,
	std::vector<int>& face_class);

// Random walk segmentation for point cloud with normals
template<typename ForwardIterator, typename PPMap, typename NPMap>
void random_walk_segmentation(
	ForwardIterator first,
	ForwardIterator beyond,
	PPMap point_pmap,
	NPMap normal_pmap,
	std::vector<int>& seed_indices,
	std::vector<int>& point_class);

} // namespace Euclid

#include "src/RandWalkSeg.cpp"
