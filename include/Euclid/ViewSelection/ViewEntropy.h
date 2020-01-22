#pragma once

#include <vector>
#include <CGAL/boost/graph/properties.h>
#include <Euclid/ViewSelection/ViewSphere.h>

namespace Euclid
{
/**@{ @ingroup PkgViewSelection*/

/** View selection using view entropy.
 *
 *  View entropy uses the solid angle of each face as the probability of its
 *  visibility and computes entropy based on it. It encourages all faces to have
 *  the same projected area.
 *
 *  @param mesh The target mesh model.
 *  @param view_sphere The viewing sphere.
 *  @param view_scores The corresponding view scores.
 *
 *  **Reference**
 *
 *  Vazquez P, Feixas M, Sbert M, and Heidrich W.
 *  Viewpoint Selection using View Entropy.
 *  Proceedings of the Vision Modeling and Visualization Conference 2001.
 */
template<typename Mesh, typename T>
void view_entropy(const Mesh& mesh,
                  const ViewSphere<Mesh>& view_sphere,
                  std::vector<T>& view_scores);

/** @}*/
} // namespace Euclid

#include "src/ViewEntropy.cpp"
