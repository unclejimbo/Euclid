#pragma once

#include <vector>

namespace Euclid
{
/**@{ @ingroup PkgSegmentation*/

/** Mesh segmentation using random walk.
 *
 *  @param mesh Input mesh.
 *  @param seeds The indices of seed faces.
 *  @param segments The output segments for each face.
 *
 *  **Reference**
 *
 *  Lai Y K, Hu S M, Martin R R, et al.
 *  Rapid and effective segmentation of 3D models using random walks[J].
 *  Computer Aided Geometric Design, 2009, 26(6): 665-679.
 */
template<typename Mesh>
void random_walk_segmentation(const Mesh& mesh,
                              const std::vector<unsigned>& seeds,
                              std::vector<unsigned>& segments);

// TODO: need to be updated
// /** Point set segmentation using random walk.
//  *
//  *  @param first The beginning of the points.
//  *  @param beyond The end of the points.
//  *  @param point_pmap The point property map.
//  *  @param nomrmal_pmap The normal property map.
//  *  @param seeds The indices of seed points.
//  *  @param segments The output segments for each point.
//  *
//  *  **Reference**
//  *
//  *  Lai Y K, Hu S M, Martin R R, et al.
//  *  Rapid and effective segmentation of 3D models using random walks[J].
//  *  Computer Aided Geometric Design, 2009, 26(6): 665-679.
//  */
// template<typename ForwardIterator, typename PPMap, typename NPMap>
// void random_walk_segmentation(ForwardIterator first,
//                               ForwardIterator beyond,
//                               PPMap point_pmap,
//                               NPMap normal_pmap,
//                               const std::vector<unsigned>& seeds,
//                               std::vector<unsigned>& segments);

/** @}*/
} // namespace Euclid

#include "src/RandomWalk.cpp"
