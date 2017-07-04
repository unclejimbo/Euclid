/*****************************************
* Package Overview:                      *
* 3D shapediscriptors for shape analysis *
*****************************************/
#pragma once
#include <vector>

namespace Euclid
{

// Spin image discriptor for mesh
template<typename Mesh, typename VertexNormalMap, typename T>
void spin_image(
	const Mesh& mesh,
	const VertexNormalMap& vnmap,
	std::vector<std::vector<T>>& spin_imgs,
	unsigned bin_size = 1, // multiples of median edge-length
	unsigned image_width = 15,
	float support_angle = 60.0f);

} // namespace Euclid

#include "src/SpinImage.cpp"
