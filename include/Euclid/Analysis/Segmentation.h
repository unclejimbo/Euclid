/** Geometric shape segmentation.
 *
 *  This package contains functions to segment 3D shapes into
 *  meaningful parts.
 *  @defgroup PkgSegmentation Segmentation
 *  @ingroup PkgAnalysis
 */
#pragma once

namespace Euclid
{
/** @{*/

/** Mesh segmentation using random walk.
 *
 *  #### Reference
 *  Lai Y K, Hu S M, Martin R R, et al.
 *  Rapid and effective segmentation of 3D models using random walks[J].
 *  Computer Aided Geometric Design, 2009, 26(6): 665-679.
 */
template<typename Mesh>
void random_walk_segmentation(
	const Mesh& mesh,
	std::vector<int>& seed_indices,
	std::vector<int>& face_class);


/** Point set segmentation using random walk.
 *
 *  #### Reference
 *  Lai Y K, Hu S M, Martin R R, et al.
 *  Rapid and effective segmentation of 3D models using random walks[J].
 *  Computer Aided Geometric Design, 2009, 26(6): 665-679.
 */
template<typename ForwardIterator, typename PPMap, typename NPMap>
void random_walk_segmentation(
	ForwardIterator first,
	ForwardIterator beyond,
	PPMap point_pmap,
	NPMap normal_pmap,
	std::vector<int>& seed_indices,
	std::vector<int>& point_class);

/** @}*/
} // namespace Euclid

#include "src/RandWalkSeg.cpp"
