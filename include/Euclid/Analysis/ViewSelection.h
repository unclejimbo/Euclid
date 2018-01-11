/** View selection.
 *
 *  View selection methods try to find a proper
 *  viewpoint to observe the shape.
 *  @defgroup PkgView View Selection
 *  @ingroup PkgAnalysis
 */
#pragma once
namespace Euclid
{
/** @{*/

/** View selection using proxy information.
 *
 *  #### Reference
 *  Gao T, Wang W, Han H.
 *  Efficient view selection by measuring proxy information[J].
 *  Computer Animation and Virtual Worlds, 2016, 27(3-4): 351-357.
 */
template<typename Polyhedron_3>
void proxy_view_selection(
	const Polyhedron_3& mesh,
	const std::array<Eigen::MatrixXf, 6>& zbuffers,
	std::vector<Eigen::Vector3f>& view_points,
	std::vector<float>& view_scores,
	int subdiv_level = 4,
	std::vector<size_t>* sphere_indices = nullptr,
	OBB<Polyhedron_3>* precomputed_obb = nullptr);

/** @}*/
} // namespace Euclid

#include "src/ProxyViewSelection.cpp"
