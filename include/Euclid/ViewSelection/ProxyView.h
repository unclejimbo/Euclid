#pragma once

#include <vector>
#include <CGAL/boost/graph/properties.h>
#include <Euclid/ViewSelection/ViewSphere.h>

namespace Euclid
{
/**@{ @ingroup PkgViewSelection*/

/** View selection using proxy information.
 *
 *  Using the object oriented bounding box as proxies to measure view saliency
 *  and interpolate results to other view samples, which is very fast.
 *
 *  @param mesh The target mesh model.
 *  @param view_sphere The viewing sphere.
 *  @param view_scores The corresponding view scores.
 *  @param weight The weighting of the projected area term, (1 - weight) is the
 *  weighting for the visible ratio term.
 *
 *  **Reference**
 *
 *  Gao T, Wang W, Han H.
 *  Efficient view selection by measuring proxy information[J].
 *  Computer Animation and Virtual Worlds, 2016, 27(3-4): 351-357.
 */
template<typename Mesh, typename T>
void proxy_view(const Mesh& mesh,
                const ViewSphere<Mesh>& view_sphere,
                std::vector<T>& view_scores,
                float weight = 0.5f);

/** @}*/
} // namespace Euclid

#include "src/ProxyView.cpp"
