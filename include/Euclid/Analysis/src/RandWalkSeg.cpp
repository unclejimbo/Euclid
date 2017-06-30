// Reference:
// [1] Yu-Kun Lai, Shi-Min Hu, Ralph R. Martin, Paul L. Rosin.
//     Rapid and Effective Segmentation of 3D Models Using Random Walks.
//     CAGD'09.
#include <Euclid/Geometry/MeshProperties.h>
#include <Eigen/SparseCore>
#include <Eigen/SparseLU>
#include <CGAL/Orthogonal_k_neighbor_search.h>
#include <CGAL/Search_traits_3.h>
#include <CGAL/Search_traits_adapter.h>
#include <boost/iterator/counting_iterator.hpp>
#include <iterator>
#include <vector>
#include <tuple>
#include <algorithm>
#include <ostream>

namespace Euclid
{

namespace _impl
{

// Construct linear system for mesh
template<typename Mesh, typename FT>
inline void _construct_equation(
	const Mesh& mesh,
	const std::vector<int>& ids,
	Eigen::SparseMatrix<FT>& A,
	Eigen::SparseMatrix<FT>& B)
{
	using SpMat = Eigen::SparseMatrix<FT>;

	auto fimap = get(boost::face_index, mesh);
	auto vpmap = get(CGAL::vertex_point, mesh);
	auto m = B.cols();
	auto n = B.rows();
	auto sum = 0.0;
	auto n_faces = num_faces(mesh);
	std::vector<int> rows;
	std::vector<int> cols;
	std::vector<FT> edge_len;
	std::vector<FT> ds;
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
		auto na = Euclid::facet_normal(*fa, mesh);

		auto fit_end = halfedge(*fa, mesh);
		auto fit = fit_end;
		do {
			auto oppo = opposite(fit, mesh);
			if (!is_border(oppo, mesh)) { // Non-boundary
				/*auto p1 = vpmap[target(fit, mesh)];
				auto p2 = vpmap[source(fit, mesh)];
				auto edge = p2 - p1;
				auto len = std::sqrt(edge.x() * edge.x() + edge.y() * edge.y() + edge.z() * edge.z());*/
				edge_len.push_back(Euclid::edge_length(fit, mesh));

				// Determine whether the incident edge is concave or convex
				auto pa = vpmap[target(next(fit, mesh), mesh)];
				auto pb = vpmap[target(next(oppo, mesh), mesh)];
				auto temp = pb - pa;
				Eigen::Matrix<FT, 3, 1> p;
				p << temp.x(), temp.y(), temp.z();
				auto eta = p.dot(na) <= 0.0 ? 0.2 : 1.0;

				auto fb = face(oppo, mesh);
				auto fb_id = ids[fimap[fb]];
				auto nb = Euclid::facet_normal(fb, mesh);
				auto diff = 0.5 * eta * (na - nb).squaredNorm();

				cols.push_back(fa_id);
				rows.push_back(fb_id);
				ds.push_back(diff);
				facet_sum += diff;
			}
			else { // Placeholder
				ds.push_back(-1.0);
			}
			fit = next(fit, mesh);
		} while (fit != fit_end);

		sum += facet_sum;
	}

	// Normalize
	std::vector<Eigen::Triplet<FT>> values;
	values.reserve(cols.size());
	const auto inv_sigma = 1.0; // Parameter
	auto inv_avg = edge_len.size() / (sum * 0.5);
	auto d_iter = ds.begin();
	auto e_iter = edge_len.begin();
	auto c_iter = cols.begin();
	auto r_iter = rows.begin();
	while (d_iter != ds.end()) {
		auto prob_sum = static_cast<FT>(0.0);
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

	for (boost::graph_traits<Mesh>::faces_size_type i = 0; i < num_faces(mesh); ++i) {
		values.emplace_back(i, i, static_cast<FT>(1.0));
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
template<typename ForwardIterator, typename Index, typename PPMap, typename NPMap, typename FT>
inline void _construct_equation(
	ForwardIterator first,
	ForwardIterator beyond,
	PPMap point_pmap,
	NPMap normal_pmap,
	const std::vector<std::vector<int>>& neighbors,
	const std::vector<Index>& indices,
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
	auto i = 0;
	for (auto iter = first; iter != beyond; ++iter, ++i) {
		auto pi = point_pmap[*iter];
		auto ni = normal_pmap[*iter];
		auto d1_sum = 0.0;

		for (auto j = 0; j < n_neighbors; ++j) {
			auto neighbor = neighbors[i][j];
			cols.push_back(ids[i]);
			rows.push_back(ids[neighbor]);

			auto pj = point_pmap[indices[neighbor]];
			auto nj = normal_pmap[indices[neighbor]];
			auto eta = ((pj - pi) - ((pj - pi) * ni) * ni) * nj >= 0.0 ? 0.2 : 1.0; // convex : concave

			auto d1 = (pi - pj).squared_length();
			d1s.push_back(d1);
			d1_sum += d1;
			auto d2 = 0.5 * eta * (ni - nj).squared_length();
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
		values.emplace_back(i, i, static_cast<FT>(1.0));
	}

	// Fill in the matrix
	SpMat L(m + n, m + n);
	L.setFromTriplets(values.begin(), values.end());
	A = L.bottomRightCorner(n, n);
	B = -L.bottomLeftCorner(n, m);
	A.makeCompressed();
	B.makeCompressed();
}

// Adapt PointPropertyMap to take in integer as key type
template<typename PPMap>
class IPMapAdaptor
{
	using PointKey = typename boost::property_traits<PPMap>::key_type;
	using Point_3 = typename boost::property_traits<PPMap>::value_type;

public:
	using value_type = Point_3;
	using reference = const value_type&;
	using key_type = int;
	using category = boost::lvalue_property_map_tag;

	IPMapAdaptor(PPMap ppmap, const std::vector<PointKey>& pks)
		: _ppmap(ppmap), _pks(pks) {}
	
	reference operator[](key_type key) const
	{
		return _ppmap[_pks[key]];
	}

	friend reference get(IPMapAdaptor ipmap, key_type key)
	{
		return ipmap[key];
	}

private:
	PPMap _ppmap;
	const std::vector<PointKey>& _pks;
};

} // namespace _impl

template<typename Mesh>
inline void random_walk_segmentation(const Mesh& mesh, std::vector<int>& seed_indices, std::vector<int>& face_class)
{
	using VertexPointMap = boost::property_map<Mesh, boost::vertex_point_t>::type;
	using Point_3 = boost::property_traits<VertexPointMap>::value_type;
	using FT = CGAL::Kernel_traits<Point_3>::Kernel::FT;
	using SpMat = Eigen::SparseMatrix<FT>;

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
	face_class.resize(n + m, 0);
	for (auto s : seed_indices) {
		face_class[s] = s;
	}
	std::vector<FT> max_probabilities(n, static_cast<FT>(-1.0));
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
				face_class[inv_ids[j + m]] = seed_indices[i];
			}
		}
	}
}

template<typename ForwardIterator, typename PPMap, typename NPMap>
inline void random_walk_segmentation(
	ForwardIterator first,
	ForwardIterator beyond,
	PPMap point_pmap,
	NPMap normal_pmap,
	std::vector<int>& seed_indices,
	std::vector<int>& point_class)
{
	using Index = std::iterator_traits<ForwardIterator>::value_type;
	using Point_3 = boost::property_traits<PPMap>::value_type;
	using Vector_3 = boost::property_traits<NPMap>::value_type;
	using Kernel = CGAL::Kernel_traits<Point_3>::Kernel;
	using IPMap = _impl::IPMapAdaptor<PPMap>;
	using BaseTraits = CGAL::Search_traits_3<Kernel>;
	using KdTreeTraits = CGAL::Search_traits_adapter<int, IPMap, BaseTraits>;
	using KNN = CGAL::Orthogonal_k_neighbor_search<KdTreeTraits>;
	using KdTree = typename KNN::Tree;
	using Splitter = KdTree::Splitter;
	using Distance = KNN::Distance;
	using FT = Kernel::FT;
	using SpMat = Eigen::SparseMatrix<FT>;
	const int k = 6; // Use 6 neighbors according to Euler formula

	// Construct the IPMap
	std::vector<Index> indices;
	int n = 0;
	for (auto iter = first; iter != beyond; ++iter) {
		indices.push_back(*iter);
		++n;
	}
	IPMap ipmap(point_pmap, indices);

	// Construct the k-d tree
	KdTree tree(
		boost::counting_iterator<int>(0),
		boost::counting_iterator<int>(n),
		Splitter(),
		KdTreeTraits(ipmap)
	);
	Distance dist(ipmap);

	// Query neighbors for all points and store the neighbors' indices
	std::vector<std::vector<int>> neighbors;
	for (auto iter = first; iter < beyond; ++iter) {
		std::vector<int> neighbors_i;
		neighbors_i.reserve(k);

		auto p = point_pmap[*iter];
		KNN knn(tree, p, k + 1, 0.00001, true, dist);

		auto it = knn.begin();
		++it; // The first will always return the identical point
		for (it; it != knn.end(); ++it) {
			neighbors_i.push_back(it->first);
		}

		neighbors.push_back(neighbors_i);
	}

	// Construct the linear equation
	auto m = static_cast<int>(seed_indices.size()); // Number of seeded
	n -= m; // Number of unseeded
	std::sort(seed_indices.begin(), seed_indices.end());
	SpMat A(n, n);
	SpMat B(n, m);
	std::vector<int> ids(m + n, -1); // ids[PointID] -> MatrixID
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
	_impl::_construct_equation(first, beyond, point_pmap, normal_pmap,
		neighbors, indices, ids, A, B);

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