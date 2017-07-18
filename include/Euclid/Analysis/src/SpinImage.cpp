// Reference:
// [1] Andrew E. Johnson, Martial Hebert
//     Using Spin Images for Efficient Object Recognition in Cluttered 3D Scenes
//     PAMI'99
#define _USE_MATH_DEFINES
#include <Euclid/Geometry/MeshProperties.h>
#include <Euclid/Geometry/KernelGeometry.h>
#include <Euclid/Geometry/Transformation.h>
#include <tuple>
#include <cmath>

namespace Euclid
{

template<typename Mesh, typename VertexNormalMap, typename T>
inline void spin_image(
	const Mesh& mesh,
	const VertexNormalMap& vnmap,
	std::vector<std::vector<T>>& spin_imgs,
	unsigned bin_size,
	unsigned image_width,
	float support_angle)
{
	using VPMap = boost::property_map<Mesh, boost::vertex_point_t>::type;
	using Point_3 = boost::property_traits<VPMap>::value_type;
	using FT = CGAL::Kernel_traits<Point_3>::Kernel::FT;
	using VIter = boost::graph_traits<Mesh>::vertex_iterator;
	using EIter = boost::graph_traits<Mesh>::edge_iterator;
	using Transform = CGAL::Aff_transformation_3<Kernel>;
	auto vpmap = get(boost::vertex_point, mesh);
	const auto to_radian = M_PI / 180.0f;

	if (support_angle < 0.0f || support_angle > 180.0f) {
		std::cerr << "Invalid support angle" << std::endl;
		return;
	}

	auto nv = num_vertices(mesh);
	spin_imgs.resize(nv);
	for (auto& i : spin_imgs) {
		i.resize(image_width * image_width);
	}

	// Calculate the resolution of mesh
	T median = 0.0;
	EIter e_iter, e_end;
	std::tie(e_iter, e_end) = edges(mesh);
	while (e_iter != e_end) {
		median += Euclid::edge_length(*e_iter++, mesh);
	}
	median /= static_cast<T>(num_edges(mesh));
	auto bin_width = median * static_cast<T>(bin_size); 
	auto inv_bin_width = 1.0 / bin_width;

	// Compute spin images for all vertices
	VIter vi, vi_end;
	std::tie(vi, vi_end) = vertices(mesh);
	unsigned i = 0;
	while (vi != vi_end) {
		auto max_value = static_cast<FT>(1);

		// Transform the coordinate system so that vi is at origin
		// and its normal points in the y axis
		Transform transform;
		auto pi = vpmap[*vi];
		auto ni = vnmap[*vi];
		CGAL::Plane_3<Kernel> plane(pi, ni);
		auto tangent = Euclid::normalized(plane.base1());
		Euclid::transform_between_2_coord_systems(
			pi, pi + tangent, pi + ni,
			transform);

		VIter vj, vj_end;
		std::tie(vj, vj_end) = vertices(mesh);
		while (vj != vj_end) {
			auto nj = vnmap[*vj];
			if (ni * nj < std::cos(support_angle * to_radian)) {
				++vj;
				continue;
			}

			auto pj = transform(vpmap[*vj]);

			auto xz = std::sqrt(pj.x() * pj.x() + pj.z() * pj.z()) * inv_bin_width;
			auto col = static_cast<unsigned>(std::floor(xz));
			if (col >= image_width) {
				++vj;
				continue;
			}
			xz = xz - col;

			auto y = pj.y() * inv_bin_width + static_cast<FT>(0.5) + image_width / 2;
			auto row = static_cast<unsigned>(std::floor(y));
			if (row >= image_width || row < 0) {
				++vj;
				continue;
			}
			y = y - row;

			auto value = static_cast<FT>(0.5) * xz + static_cast<FT>(0.5) * y;
			auto index = row * image_width + col;
			spin_imgs[i][index] += value;
			if (spin_imgs[i][index] > max_value) {
				max_value = spin_imgs[i][index];
			}
			++vj;
		}

		// Normalize
		auto inv_max = 1 / max_value;
		for (auto& v : spin_imgs[i]) {
			v *= inv_max;
		}

		++i;
		++vi;
	}
}

} // namespace Euclid

#undef _USE_MATH_DEFINES
