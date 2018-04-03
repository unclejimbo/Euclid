/** View selection.
 *
 *  View selection methods try to find a proper viewpoint to observe the shape.
 *  @defgroup PkgView View Selection
 *  @ingroup PkgAnalysis
 */
#pragma once

#include <vector>
#include <Eigen/Dense>

namespace Euclid
{
/** @{*/

/** View selection using proxy information.
 *
 *  @param mesh The input mesh.
 *  @param view_sphere The output view sphere.
 *  @param view_scores The corresponding view scores.
 *  @param subdiv_level The subdivision level of the subdivision viewing sphere.
 *  @param weight The weighting of the projected area term, (1 - weight) is the
 *  weighting for the visible ratio term.
 *  @param up The up vector of the mesh model.
 *
 *  #### Reference
 *  Gao T, Wang W, Han H.
 *  Efficient view selection by measuring proxy information[J].
 *  Computer Animation and Virtual Worlds, 2016, 27(3-4): 351-357.
 */
template<typename Mesh, typename FT>
void proxy_view_selection(const Mesh& mesh,
                          Mesh& view_sphere,
                          std::vector<FT>& view_scores,
                          int subdiv_level = 4,
                          float weight = 0.5f,
                          const Eigen::Vector3f& up = Eigen::Vector3f(0.0f,
                                                                      1.0f,
                                                                      0.0f));

/** @}*/
} // namespace Euclid

#include "src/ProxyViewSelection.cpp"
