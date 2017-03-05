#pragma once

#include "OBB.h"
#include "PrimitiveGenerator.h"
#include "Polyhedron_3.h"
#include <Eigen/Dense>
#include <array>
#include <vector>
#include <cmath>
#include <algorithm>

namespace euclid
{

/************************************************************************
* Reference: "Efficient View Selection by Measuring Proxy Information", *
*            (Tianhao Gao, etc., CASA2016)                               *
*************************************************************************/
template<typename Polyhedron_3>
void proxy_view_selection(
	const Polyhedron_3& mesh,
	const std::array<Eigen::MatrixXf, 6>& zbuffers,
	std::vector<Eigen::Vector3f>& view_points,
	std::vector<float>& view_scores,
	int subdiv_level = 4,
	std::vector<size_t>* sphere_indices = nullptr,
	OBB<Polyhedron_3>* precomputed_obb = nullptr)
{
	// Generate PCA if not provided
	OBB<Polyhedron_3>* pca = nullptr;
	if (precomputed_obb != nullptr) {
		pca = precomputed_obb;
	}
	else {
		pca = new OBB<Polyhedron_3>(mesh);
	}

	// Generate sample sphere
	auto radius = pca->radius() * 3.0f;
	auto sphere = euclid::subdivision_sphere<Polyhedron_3>(radius, pca->center(), subdiv_level);
	view_points.clear();
	std::transform(sphere.points_begin(), sphere.points_end(), std::back_inserter(view_points),
		[](const decltype(sphere.points_begin())::value_type& p) {
			return Eigen::Vector3f(p.x(), p.y(), p.z());
		}
	);
	if (sphere_indices != nullptr) {
		size_t idx = 0;
		for (auto v = sphere.vertices_begin(); v != sphere.vertices_end(); ++v) {
			v->id() = idx++;
		}

		sphere_indices->clear();
		sphere_indices->reserve(sphere.size_of_facets() * 3);
		for (auto f = sphere.facets_begin(); f != sphere.facets_end(); ++f) {
			auto v = f->facet_begin();
			do {
				sphere_indices->push_back(v->vertex()->id());
			} while (++v != f->facet_begin());
		}
	}

	// Compute projected area
	std::vector<float> projected_areas(zbuffers.size());
	for (size_t i = 0; i < projected_areas.size(); ++i) {
		auto proj = 0.0f;
		for (size_t j = 0; j < zbuffers[i].size(); ++j) {
			auto z = zbuffers[i](j);
			if (z < 1.0f) {
				proj += 1;
			}
		}
		projected_areas[i] = proj / zbuffers[i].size();
	}
	auto max_proj_area = *std::max_element(projected_areas.begin(), projected_areas.end());
	for (auto& proj_area : projected_areas) {
		proj_area /= max_proj_area;
	}

	// Compute visible ratio
	const float least_visible = std::cosf(3.14159265359 / 3.0f);
	std::vector<float> visible_ratios(zbuffers.size());
	std::vector<int> n_visible_facets(zbuffers.size(), 0);
	for (auto f = mesh.facets_begin(); f != mesh.facets_end(); ++f) {
		Eigen::Vector3f normal_dir;
		try { // Denerate face should be non-visible
			auto norm = compute_facet_normal<Polyhedron_3>(*f);
			normal_dir = norm;
		}
		catch (...) {
			normal_dir.setZero();
		}
		auto principal_dirs = pca->directions();
		if (principal_dirs[0].dot(normal_dir) > least_visible) {
			++n_visible_facets[0];
		}
		if (principal_dirs[1].dot(normal_dir) > least_visible) {
			++n_visible_facets[1];
		}
		if (principal_dirs[2].dot(normal_dir) > least_visible) {
			++n_visible_facets[2];
		}
		if ((-principal_dirs[0]).dot(normal_dir) > least_visible) {
			++n_visible_facets[3];
		}
		if ((-principal_dirs[1]).dot(normal_dir) > least_visible) {
			++n_visible_facets[4];
		}
		if ((-principal_dirs[2]).dot(normal_dir) > least_visible) {
			++n_visible_facets[5];
		}
	}
	float inv_nf = 1.0f / mesh.size_of_facets();
	std::transform(n_visible_facets.begin(), n_visible_facets.end(), visible_ratios.begin(),
		[inv_nf](const int& nf) { return nf * inv_nf; });
	auto max_visible_ratio = *std::max_element(visible_ratios.begin(), visible_ratios.end());
	for (auto& visible_ratio : visible_ratios) {
		visible_ratio /= max_visible_ratio;
	}

	// Compute final score
	view_scores.clear();
	view_scores.resize(view_points.size(), 0.0f);
	const float w = 0.9f;
	const float v = 1.0f - w;
	for (size_t i = 0; i < view_scores.size(); ++i) {
		auto principal_dirs = pca->directions();
		auto view_dir = (view_points[i] - pca->center()).normalized();
		view_scores[i] += (w * projected_areas[0] + v * visible_ratios[0]) *
			std::max(view_dir.dot(principal_dirs[0]), 0.0f);
		view_scores[i] += (w * projected_areas[1] + v * visible_ratios[1]) *
			std::max(view_dir.dot(principal_dirs[1]), 0.0f);
		view_scores[i] += (w * projected_areas[2] + v * visible_ratios[2]) *
			std::max(view_dir.dot(principal_dirs[2]), 0.0f);
		view_scores[i] += (w * projected_areas[3] + v * visible_ratios[3]) *
			std::max(view_dir.dot(-principal_dirs[0]), 0.0f);
		view_scores[i] += (w * projected_areas[4] + v * visible_ratios[4]) *
			std::max(view_dir.dot(-principal_dirs[1]), 0.0f);
		view_scores[i] += (w * projected_areas[5] + v * visible_ratios[5]) *
			std::max(view_dir.dot(-principal_dirs[2]), 0.0f);
	}

	if (precomputed_obb == nullptr) {
		delete pca;
	}
}

} // namespace euclid
