#pragma once

#include <vector>
#include <Eigen/Core>
#include <Euclid/MeshUtil/MeshDefs.h>

namespace Euclid
{
/**@{ @ingroup PkgFeature*/

/**Feature detection using heat kernel signature.
 *
 * Points whose hks are local maximum are treated as feature points.
 *
 *  **Reference**
 *
 *  Sun J., Ovsjanikov M., Guibas L..
 *  A concise and provably informative multi-scale signature based on heat
 *  diffusion.
 *  Proceedings of the Symposium on Geometry Processing, 2009.
 */
template<typename Mesh, typename Derived>
std::vector<vertex_t<Mesh>> native_hks_features(
    const Mesh& mesh,
    const Eigen::ArrayBase<Derived>& hks,
    int tscale);

/**@}*/
} // namespace Euclid

#include "src/NativeHKS.cpp"
