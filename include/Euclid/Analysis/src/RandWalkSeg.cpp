// Reference:
// [1] Yu-Kun Lai, Shi-Min Hu, Ralph R. Martin, Paul L. Rosin.
//     Rapid and Effective Segmentation of 3D Models Using Random Walks.
//     CAGD'09.
#include <Euclid/Geometry/MeshProperties.h>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
#include <CGAL/boost/graph/iterator.h>
#include <CGAL/boost/graph/helpers.h>
#include <boost/iterator/counting_iterator.hpp>
#include <vector>
#include <unordered_map>
#include <tuple>
#include <algorithm>
#include <ostream>

namespace Euclid
{

namespace _impl
{

// Construct linear system for mesh
template<typename Mesh>
inline void _construct_equation(
	const Mesh& mesh,
	const std::vector<int>& ids,
	Eigen::SparseMatrix<float>& A,
	Eigen::SparseMatrix<float>& B)
{
	using SpMat = Eigen::SparseMatrix<float>;

	auto fimap = get(CGAL::face_index, mesh);
	auto vpmap = get(CGAL::vertex_point, mesh);
	auto m = B.cols();
	auto n = B.rows();
	auto sum = 0.0;
	auto n_faces = num_faces(mesh);
	std::vector<int> rows;
	std::vector<int> cols;
	std::vector<float> edge_len;
	std::vector<float> ds;
	rows.reserve(3 * n_faces);
	cols.reserve(3 * n_faces);
	edge_len.reserve(3 * n_faces);
	ds.reserve(3 * n_faces);

	// Compute the ingredients of the laplacian matrix
	boost::graph_traits<const Mesh>::face_iterator fa, f_end;
	std::tie(fa, f_end) = faces(mesh);
	for (; fa != f_end; ++fa) {
		auto fa_id = ids[fimap[*fa]];
		auto facet_sum = 0.0;
		auto na = Euclid::facet_normal(mesh, *fa);

		CGAL::Halfedge_around_face_iterator<Mesh> fit, fit_end;
		std::tie(fit, fit_end) = halfedges_around_face(halfedge(*fa, mesh), mesh);
		do {
			if (!is_border(opposite(*fit, mesh), mesh)) { // Non-boundary
				auto p1 = vpmap[target(*fit, mesh)];
				auto p2 = vpmap[source(*fit, mesh)];
				auto edge = p2 - p1;
				auto len = std::sqrt(edge.x() * edge.x() + edge.y() * edge.y() + edge.z() * edge.z());
				edge_len.push_back(len);

				// Determine whether the incident edge is concave or convex
				auto pa = vpmap[target(next(*fit, mesh), mesh)];
				auto pb = vpmap[target(next(opposite(*fit, mesh), mesh), mesh)];
				auto temp = pb - pa;
				Eigen::Vector3d p;
				p << temp.x(), temp.y(), temp.z();
				auto eta = p.dot(na) <= 0.0 ? 0.2 : 1.0;

				auto fb = face(opposite(*fit, mesh), mesh);
				auto fb_id = ids[fimap[fb]];
				auto nb = Euclid::facet_normal(mesh, fb);
				auto diff = 0.5 * eta * (na - nb).squaredNorm();

				cols.push_back(fa_id);
				rows.push_back(fb_id);
				ds.push_back(diff);
				facet_sum += diff;
			}
			else { // Placeholder
				ds.push_back(-1.0);
			}
		} while (++fit != fit_end);

		sum += facet_sum;
	}

	// Normalize
	std::vector<Eigen::Triplet<double>> values;
	values.reserve(cols.size());
	const auto inv_sigma = 1.0; // Parameter
	auto inv_avg = edge_len.size() / (sum * 0.5);
	auto d_iter = ds.begin();
	auto e_iter = edge_len.begin();
	auto c_iter = cols.begin();
	auto r_iter = rows.begin();
	while (d_iter != ds.end()) {
		auto prob_sum = 0.0;
		for (auto j = 0; j < 3; ++j) {
			if (*d_iter != -1.0) {
				auto prob = std::exp(-(*d_iter * inv_avg) * inv_sigma) * *e_iter++;
				*d_iter++ = -prob;
				prob_sum += prob;
			}
			else {
				++d_iter;
			}
		}
		prob_sum = 1.0 / prob_sum;
		d_iter -= 3;
		for (auto j = 0; j < 3; ++j) {
			if (*d_iter != -1.0) {
				values.emplace_back(*c_iter++, *r_iter++, *d_iter++ * prob_sum);
			}
			else {
				++d_iter;
			}
		}
	}

	for (auto i = 0; i < mesh.size_of_facets(); ++i) {
		values.emplace_back(i, i, 1.0);
	}

	// Fill in the matrix
	SpMat L(m + n, m + n);
	L.setFromTriplets(values.begin(), values.end());
	A = L.bottomRightCorner(n, n);
	B = -L.bottomLeftCorner(n, m);
	A.makeCompressed();
	B.makeCompressed();
}

// Construct linear system for point cloud
template<typename FT>
inline void _construct_equation(
	const std::vector<Eigen::Matrix<FT, 3, 1>>& points,
	const std::vector<Eigen::Matrix<FT, 3, 1>>& normals,
	const std::vector<std::vector<int>>& neighbors,
	const std::vector<int>& ids,
	Eigen::SparseMatrix<FT>& A,
	Eigen::SparseMatrix<FT>& B)
{
	using SpMat = Eigen::SparseMatrix<FT>;

	const auto inv_sigma1 = 1.0;
	const auto inv_sigma2 = 1.0;
	auto m = B.cols();
	auto n = B.rows();
	auto n_points = m + n;
	auto n_neighbors = neighbors[0].size();
	std::vector<int> rows;
	std::vector<int> cols;
	std::vector<FT> d1s;
	std::vector<FT> d2s;
	rows.reserve(n_points * n_neighbors);
	cols.reserve(n_points * n_neighbors);
	d1s.reserve(n_points * n_neighbors);
	d2s.reserve(n_points * n_neighbors);
	auto d2_sum = 0.0;

	// Compute the ingredients of the laplacian matrix
	for (auto i = 0; i < n_points; ++i) {
		auto pi = points[i];
		auto ni = normals[i];
		auto d1_sum = 0.0;

		for (auto j = 0; j < n_neighbors; ++j) {
			auto neighbor = neighbors[i][j];
			cols.push_back(ids[i]);
			rows.push_back(ids[neighbor]);

			auto pj = points[neighbor];
			auto nj = normals[neighbor];
			auto eta = ((pj - pi) - (pj - pi).dot(ni) * ni).dot(nj) >= 0.0 ? 0.2 : 1.0; // convex : concave

			auto d1 = (pi - pj).squaredNorm();
			d1s.push_back(d1);
			d1_sum += d1;
			auto d2 = 0.5 * eta * (ni - nj).squaredNorm();
			d2s.push_back(d2);
			d2_sum += d2;
		}

		auto d1_inv_avg = n_neighbors / d1_sum;
		for (auto j = 0; j < n_neighbors; ++j) {
			auto idx = i * n_neighbors + j;
			d1s[idx] = std::exp(-d1s[idx] * d1_inv_avg * inv_sigma1);
		}
	}

	auto d2_inv_avg = (n_points * n_neighbors) / d2_sum;
	for (auto& d2 : d2s) {
		d2 = std::exp(-d2 * d2_inv_avg * inv_sigma2);
	}

	// Normalize
	std::vector<Eigen::Triplet<FT>> values;
	values.reserve(n_points * (n_neighbors + 1));
	for (auto i = 0; i < n_points; ++i) {
		std::vector<FT> ds(n_neighbors);
		FT sum = 0.0;
		for (auto j = 0; j < n_neighbors; ++j) {
			auto idx = i * n_neighbors + j;
			ds[j] = d1s[idx] * d2s[idx];
			sum += ds[j];
		}
		FT inv_sum = 1.0 / sum;
		for (auto j = 0; j < n_neighbors; ++j) {
			auto idx = i * n_neighbors + j;
			values.emplace_back(cols[idx], rows[idx], -ds[j] * inv_sum);
		}
		values.emplace_back(i, i, 1.0);
	}

	// Fill in the matrix
	SpMat L(m + n, m + n);
	L.setFromTriplets(values.begin(), values.end());
	A = L.bottomRightCorner(n, n);
	B = -L.bottomLeftCorner(n, m);
	A.makeCompressed();
	B.makeCompressed();
}

} // namespace _impl

template<typename Mesh>
inline void random_walk_segmentation(const Mesh& mesh, std::vector<int>& seed_indices, std::vector<int>& facet_class)
{
	using SpMat = Eigen::SparseMatrix<float>;

	// Construct the linear equation
	auto m = static_cast<int>(seed_indices.size()); // Number of seeded
	auto n = static_cast<int>(num_faces(mesh)) - m; // Number of unseeded
	std::sort(seed_indices.begin(), seed_indices.end());
	std::vector<int> ids(m + n, -1); // ids[FacetID] -> MatrixID
	int inc = 0;
	while (inc < seed_indices.size()) {
		ids[seed_indices[inc]] = inc;
		++inc;
	}
	for (auto& id : ids) {
		if (id == -1) {
			id = inc++;
		}
	}
	SpMat A(n, n);
	SpMat B(n, m);
	_impl::_construct_equation(mesh, ids, A, B);

	std::vector<int> inv_ids(m + n);
	for (auto i = 0; i < m + n; ++i) {
		inv_ids[ids[i]] = i;
	}

	// Solve the equation to segment
	facet_class.resize(n + m, 0);
	for (auto s : seed_indices) {
		facet_class[s] = s;
	}
	std::vector<float> max_probabilities(n, static_cast<float>(-1.0));
	Eigen::SparseLU<SpMat> solver;
	solver.compute(A);
	if (solver.info() != Eigen::Success) {
		std::cerr << solver.lastErrorMessage() << std::endl;
		return;
	}
	for (auto i = 0; i < m; ++i) {
		Eigen::Matrix<float, Eigen::Dynamic, 1> b = B.col(i);
		Eigen::Matrix<float, Eigen::Dynamic, 1> x = solver.solve(b);
		for (auto j = 0; j < n; ++j) {
			if (x(j, 0) > max_probabilities[j]) {
				max_probabilities[j] = x(j, 0);
				facet_class[inv_ids[j + m]] = seed_indices[i];
			}
		}
	}
}

template<typename FT>
inline void random_walk_segmentation(
	const std::vector<Eigen::Matrix<FT, 3, 1>>& vertices,
	const std::vector<Eigen::Matrix<FT, 3, 1>>& normals,
	std::vector<int>& seed_indices, std::vector<int>& point_class)
{
	using Vector3 = Eigen::Matrix<FT, 3, 1>;
	using Matrix3x3 = Eigen::Matrix<FT, 3, 3>;
	using SpMat = Eigen::SparseMatrix<FT>;
	using Kernel = CGAL::Simple_cartesian<FT>;
	using Point = Kernel::Point_3;
	using BaseTraits = CGAL::Search_traits_3<Kernel>;
	using IndexPointMap = boost::const_associative_property_map<std::unordered_map<int, Point>>;
	using KdTreeTraits = CGAL::Search_traits_adapter<int, IndexPointMap, BaseTraits>;
	using KNN = CGAL::Orthogonal_k_neighbor_search<KdTreeTraits>;
	using KdTree = typename KNN::Tree;
	using Splitter = KdTree::Splitter;
	using Distance = KNN::Distance;
	const int k = 6; // Use 6 neighbors according to Euler formula

	// Construct the property map of point index and point data
	std::unordered_map<int, Point> points;
	points.reserve(vertices.size());
	for (auto i = 0; i < vertices.size(); ++i) {
		auto p = vertices[i];
		points.emplace(i, Point(p(0, 0), p(1, 0), p(2, 0)));
	}
	IndexPointMap ppmap(points);

	// Construct the k-d tree
	KdTree tree(
		boost::counting_iterator<int>(0),
		boost::counting_iterator<int>(static_cast<int>(points.size())),
		Splitter(),
		KdTreeTraits(ppmap)
	);
	Distance dist(ppmap);

	// Query neighbors for all points and store the neighbors' indices
	std::vector<std::vector<int>> neighbors(vertices.size());
	for (auto i = 0; i < vertices.size(); ++i) {
		auto p = vertices[i];
		Point query(p(0, 0), p(1, 0), p(2, 0));
		KNN knn(tree, query, k + 1, 0, true, dist);
		neighbors[i].reserve(k);

		auto it = knn.begin();
		++it; // The first will always return the identical point
		for (it; it != knn.end(); ++it) {
			neighbors[i].push_back(it->first);
		}
	}

	// Construct the linear equation
	auto m = static_cast<int>(seed_indices.size()); // Number of seeded
	auto n = static_cast<int>(vertices.size()) - m; // Number of unseeded
	std::sort(seed_indices.begin(), seed_indices.end());
	SpMat A(n, n);
	SpMat B(n, m);
	std::vector<int> ids(m + n, -1); // ids[FacetID] -> MatrixID
	int inc = 0;
	while (inc < seed_indices.size()) {
		ids[seed_indices[inc]] = inc;
		++inc;
	}
	for (auto& id : ids) {
		if (id == -1) {
			id = inc++;
		}
	}
	_impl::_construct_equation(vertices, normals, neighbors, ids, A, B);

	std::vector<int> inv_ids(m + n);
	for (auto i = 0; i < m + n; ++i) {
		inv_ids[ids[i]] = i;
	}

	// Solve the equation to segment
	point_class.resize(n + m, 0);
	for (auto s : seed_indices) {
		point_class[s] = s;
	}
	std::vector<FT> max_probabilities(n, static_cast<FT>(-1.0));
	Eigen::SparseLU<SpMat> solver;
	solver.compute(A);
	if (solver.info() != Eigen::Success) {
		std::cerr << solver.lastErrorMessage() << std::endl;
		return;
	}
	for (auto i = 0; i < m; ++i) {
		Eigen::Matrix<FT, Eigen::Dynamic, 1> b = B.col(i);
		Eigen::Matrix<FT, Eigen::Dynamic, 1> x = solver.solve(b);
		for (auto j = 0; j < n; ++j) {
			if (x(j, 0) > max_probabilities[j]) {
				max_probabilities[j] = x(j, 0);
				point_class[inv_ids[j + m]] = seed_indices[i];
			}
		}
	}
}

} // namespace Euclid