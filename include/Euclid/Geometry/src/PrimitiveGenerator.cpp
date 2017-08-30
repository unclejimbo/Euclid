#include <Euclid/Geometry/Surface_mesh.h>
#include <Euclid/Geometry/Polyhedron_3.h>
#include <Euclid/Geometry/KernelGeometry.h>
#include <CGAL/Subdivision_method_3.h>
#include <algorithm>

namespace Euclid
{

// Partial specialization for Surface_mesh
template<typename Point>
class PrimitiveGenerator<CGAL::Surface_mesh<Point>>
{
	using Mesh = CGAL::Surface_mesh<Point>;
	using Point_3 = typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>
		::type>::value_type;
	using Kernel = typename CGAL::Kernel_traits<Point_3>::Kernel;
	using FT = typename Kernel::FT;

public:
	template<typename T>
	static void icosahedron(
		Mesh& mesh,
		T radius = 1.0,
		const Point_3& center = { 0.0, 0.0, 0.0 })
	{
		const FT x = 0.5257311;
		const FT y = 0.8506508;
		std::vector<FT> vertices = {
			0.0, -x, y,
			y, 0.0, x,
			y, 0.0, -x,
			-y, 0.0, -x,
			-y, 0.0, x,
			-x, y, 0.0,
			x, y, 0.0,
			x, -y, 0.0,
			-x, -y, 0.0,
			0.0, -x, -y,
			0.0, x, -y,
			0.0, x, y
		};
		std::vector<unsigned> indices = {
			1, 2, 6,
			1, 7, 2,
			3, 4, 5,
			4, 3, 8,
			6, 5, 11,
			5, 6, 10,
			9, 10, 2,
			10, 9, 3,
			7, 8, 9,
			8, 7, 0,
			11, 0, 1,
			0, 11, 4,
			6, 2, 10,
			1, 6, 11,
			3, 5, 10,
			5, 4, 11,
			2, 7, 9,
			7, 1, 0,
			3, 9, 8,
			4, 8, 0
		};
		for (size_t i = 0; i < vertices.size(); i += 3) {
			vertices[i] = vertices[i] * radius + center.x();
			vertices[i + 1] = vertices[i + 1] * radius + center.y();
			vertices[i + 2] = vertices[i + 2] * radius + center.z();
		}
		build_surface_mesh(mesh, vertices, indices);
	}

	template<typename T>
	static void subdivision_sphere(
		Mesh& mesh,
		T radius = 1.0,
		const Point_3& center = { 0.0, 0.0, 0.0 },
		int iterations = 4)
	{
		icosahedron(mesh, radius, center);
		CGAL::Subdivision_method_3::Loop_subdivision(mesh, iterations);
	}
};

// Partial specialization for Polyhedron_3
template<typename PolyhedronTraits_3, 
	typename PolyhedronItems_3,
	template<typename T, typename I, typename> class HalfedgeDS,
	typename Alloc>
class PrimitiveGenerator<CGAL::Polyhedron_3<PolyhedronTraits_3, PolyhedronItems_3, HalfedgeDS, Alloc>>
{
	using Mesh = CGAL::Polyhedron_3<PolyhedronTraits_3, PolyhedronItems_3, HalfedgeDS, Alloc>;
	using Point_3 = typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>
		::type>::value_type;
	using Kernel = typename CGAL::Kernel_traits<Point_3>::Kernel;
	using FT = typename Kernel::FT;

public:
	template<typename T>
	static void icosahedron(
		Mesh& mesh,
		T radius = 1.0,
		const Point_3& center = { 0.0, 0.0, 0.0 })
	{
		const FT x = 0.5257311;
		const FT y = 0.8506508;
		std::vector<FT> vertices = {
			0.0, -x, y,
			y, 0.0, x,
			y, 0.0, -x,
			-y, 0.0, -x,
			-y, 0.0, x,
			-x, y, 0.0,
			x, y, 0.0,
			x, -y, 0.0,
			-x, -y, 0.0,
			0.0, -x, -y,
			0.0, x, -y,
			0.0, x, y
		};
		std::vector<unsigned> indices = {
			1, 2, 6,
			1, 7, 2,
			3, 4, 5,
			4, 3, 8,
			6, 5, 11,
			5, 6, 10,
			9, 10, 2,
			10, 9, 3,
			7, 8, 9,
			8, 7, 0,
			11, 0, 1,
			0, 11, 4,
			6, 2, 10,
			1, 6, 11,
			3, 5, 10,
			5, 4, 11,
			2, 7, 9,
			7, 1, 0,
			3, 9, 8,
			4, 8, 0
		};
		for (size_t i = 0; i < vertices.size(); i += 3) {
			vertices[i] = vertices[i] * radius + center.x();
			vertices[i + 1] = vertices[i + 1] * radius + center.y();
			vertices[i + 2] = vertices[i + 2] * radius + center.z();
		}
		TriMeshBuilder<Mesh> builder(vertices, indices);
		mesh.delegate(builder);
	}

	template<typename T>
	static void subdivision_sphere(
		Mesh& mesh,
		T radius = 1.0,
		const Point_3& center = { 0.0, 0.0, 0.0 },
		int iterations = 4)
	{
		icosahedron(mesh, 1.0);
		CGAL::Subdivision_method_3::Loop_subdivision(mesh, iterations);

		// Subdivision could cause severe shrinkage, so it needs to be scaled afterwards
		auto vpmap = get(boost::vertex_point, mesh);
		FT r = 0.0;
		for (const auto& v : vertices(mesh)) {
			auto p = vpmap[v];
			r += length(p - CGAL::ORIGIN);
		}
		r /= num_vertices(mesh);
		for (const auto& v : vertices(mesh)) {
			auto x = vpmap[v].x() * radius / r + center.x();
			auto y = vpmap[v].y() * radius / r + center.y();
			auto z = vpmap[v].z() * radius / r + center.z();
			vpmap[v] = Point_3(x, y, z);
		}
	}
};

} // namespace Euclid