#pragma once
/********************************************************
* Package Overview:                                     *
* View selection methods help generate good view points *
* to observe a 3d model by measuring certain attributes *
********************************************************/
namespace Euclid
{

template<typename Polyhedron_3>
void proxy_view_selection(
	const Polyhedron_3& mesh,
	const std::array<Eigen::MatrixXf, 6>& zbuffers,
	std::vector<Eigen::Vector3f>& view_points,
	std::vector<float>& view_scores,
	int subdiv_level = 4,
	std::vector<size_t>* sphere_indices = nullptr,
	OBB<Polyhedron_3>* precomputed_obb = nullptr);

} // namespace Euclid

#include "src/ProxyViewSelection.cpp"
