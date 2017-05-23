#pragma once

#include <Euclid/Geometry/Polyhedron_3.h>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <Eigen/Dense>
#include <vector>
#include <algorithm>
#include <cmath>

/**********************************************************
* Package Overview:                                       *
* Segmentation of 3D shape including mesh and point cloud *
**********************************************************/
namespace Euclid
{

// Reference:
// [1] Yu-Kun Lai, Shi-Min Hu, Ralph R. Martin, Paul L. Rosin.
//     Rapid and Effective Segmentation of 3D Models Using Random Walks.
//     CAGD'09.
template<typename Polyhedron_3>
inline void _construct_equation(const Polyhedron_3& mesh, std::vector<int>& seed_indices,
	Eigen::SparseMatrix<typename Polyhedron_3::Traits::Kernel::FT, Eigen::RowMajor>& A,
	Eigen::SparseMatrix<typename Polyhedron_3::Traits::Kernel::FT, Eigen::RowMajor>& B)
{
	using FT = typename Polyhedron_3::Traits::Kernel::FT;
	using SpMat = Eigen::SparseMatrix<FT, Eigen::RowMajor>;

	// Construct the laplacian matrix
	auto m = B.cols();
	auto n = B.rows();
	SpMat L(m + n, m + n);
	auto sum = 0.0;
	std::vector<FT> edge_len;
	edge_len.reserve(3 * mesh.size_of_facets());
	
	std::vector<int> ids(m + n); // MatrixIndex[FacetID]
	for (auto i = 0; i < m + n; ++i) {
		ids[i] = i;
	}
	for (auto i = 0; i < seed_indices.size(); ++i) {
		auto temp = ids[i];
		ids[i] = ids[seed_indices[i]];
		ids[seed_indices[i]] = temp;
	}

	for (auto fa = mesh.facets_begin(); fa != mesh.facets_end(); ++fa) {
		auto fa_id = ids[fa->id()];
		auto facet_sum = 0.0;
		auto na = Euclid::compute_facet_normal<Polyhedron_3>(*fa);
		auto fit = fa->facet_begin();
		do {
			if (fit->opposite()->facet() != nullptr) { // Non-boundary
				auto p1 = fit->vertex()->point();
				auto p2 = fit->prev()->vertex()->point();
				auto edge = p2 - p1;
				auto len = std::sqrt(edge.x() * edge.x() + edge.y() * edge.y() + edge.z() * edge.z());
				edge_len.push_back(len);

				auto fb = fit->opposite()->facet();
				auto fb_id = ids[fb->id()];
				auto sym = L.coeff(static_cast<int>(fb_id), static_cast<int>(fa_id));
				if (sym != 0.0) { // Already computed
					L.insert(static_cast<int>(fa_id), static_cast<int>(fb_id)) = sym;
					facet_sum += sym;
				}
				else {
					// Determine whether the incident edge is concave or convex
					auto pa = fit->next()->vertex()->point();
					auto pb = fit->opposite()->next()->vertex()->point();
					auto temp = pb - pa;
					Eigen::Matrix<FT, 3, 1> p;
					p << temp.x(), temp.y(), temp.z();
					auto eta = p.dot(na) <= 0.0 ? 0.2 : 1.0;
					auto nb = Euclid::compute_facet_normal<Polyhedron_3>(*fb);
					auto diff = 0.5 * eta * (na - nb).squaredNorm();

					L.insert(static_cast<int>(fa_id), static_cast<int>(fb_id)) = diff;
					facet_sum += diff;
				}
			}
		} while (++fit != fa->facet_begin());

		sum += facet_sum;
	}

	const auto inv_sigma = 1.0; // Parameter
	auto inv_avg = edge_len.size() / (sum * 0.5);
	auto iter = 0;
	for (auto i = 0; i < L.outerSize(); ++i) {
		auto prob_sum = 0.0;
		for (SpMat::InnerIterator it(L, i); it; ++it) {
			auto prob = std::exp(-(it.value() * inv_avg) * inv_sigma) * edge_len[iter++];
			it.valueRef() = -prob;
			prob_sum += prob;
		}
		prob_sum = 1.0 / prob_sum;
		for (SpMat::InnerIterator it(L, i); it; ++it) {
			it.valueRef() *= prob_sum;
		}
	}

	for (auto i = 0; i < mesh.size_of_facets(); ++i) {
		L.insert(i, i) = 1.0;
	}

	A = L.bottomRightCorner(n, n);
	B = -L.bottomLeftCorner(n, m);
}

template<typename Polyhedron_3>
inline void random_walk_segmentation(const Polyhedron_3& mesh, std::vector<int>& seed_indices, std::vector<int>& facet_class)
{
	using FT = typename Polyhedron_3::Traits::Kernel::FT;
	using SpMat = Eigen::SparseMatrix<FT, Eigen::RowMajor>;

	// Construct the linear equation
	std::sort(seed_indices.begin(), seed_indices.end());
	auto m = static_cast<int>(seed_indices.size()); // Number of seeded
	auto n = static_cast<int>(mesh.size_of_facets()) - m; // Number of unseeded
	SpMat A(n, n);
	SpMat B(n, m);
	_construct_equation(mesh, seed_indices, A, B);

	// Solve the equation to segment
	facet_class.resize(n + m, 0);
	std::copy(seed_indices.begin(), seed_indices.end(), facet_class.begin());
	std::vector<FT> max_probabilities(n, static_cast<FT>(-1.0));
	Eigen::SparseLU<SpMat> solver;
	solver.analyzePattern(A);
	solver.factorize(A);
	for (auto i = 0; i < m; ++i) {
		Eigen::Matrix<FT, Eigen::Dynamic, 1> b = B.col(i);
		Eigen::Matrix<FT, Eigen::Dynamic, 1> x = solver.solve(b);
		for (auto j = 0; j < n; ++j) {
			if (x(j, 0) > max_probabilities[j]) {
				max_probabilities[j] = x(j, 0);
				facet_class[j + m] = seed_indices[i];
			}
		}
	}

	// Rearrange the result so that it's ordered by facet's id
	for (auto i = m - 1; i >= 0; --i) {
		auto id = seed_indices[i];
		auto temp = facet_class[i];
		facet_class[i] = facet_class[id];
		facet_class[id] = temp;
	}
}

} // namespace Euclid
