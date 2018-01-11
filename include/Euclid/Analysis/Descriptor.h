/** Shape descriptors.
 *
 *  This package contain functions to compute shape descriptors.
 *  @defgroup PkgDescriptor Descriptor
 *  @ingroup PkgAnalysis
 */
#pragma once
#include <vector>

namespace Euclid
{
/** @{*/

/** The spin image descriptor.
 *
 *  @param image_width Multiples of median edge length.
 *
 *  #### Reference
 *  Johnson A E, Hebert M.
 *  Using spin images for efficient object recognition in cluttered 3D scenes[J].
 *  IEEE Transactions on pattern analysis and machine intelligence, 1999, 21(5): 433-449.
 */
template<typename Mesh, typename VertexNormalMap, typename T>
void spin_image(
	const Mesh& mesh,
	const VertexNormalMap& vnmap,
	std::vector<std::vector<T>>& spin_imgs,
	unsigned bin_size = 1,
	unsigned image_width = 15,
	float support_angle = 60.0f);

/** @}*/
} // namespace Euclid

#include "src/SpinImage.cpp"
