#include <CGAL/Subdivision_method_3.h>
#include <Eigen/Dense>
#include <algorithm>

namespace Euclid
{

template<typename Polyhedron_3>
inlline Polyhedron_3 icosahedron(
	typename Polyhedron_3::Traits::Kernel::FT radius = 1.0,
	const Eigen::Matrix<typename Polyhedron_3::Traits::Kernel::FT, 3, 1>& center = { 0.0, 0.0, 0.0 })
{
	using FT = typename Polyhedron_3::Traits::Kernel::FT;

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
		vertices[i] = vertices[i] * radius + center(0);
		vertices[i + 1] = vertices[i + 1] * radius + center(1);
		vertices[i + 2] = vertices[i + 2] * radius + center(2);
	}

	Polyhedron_3 icosahedron;
	TriMeshBuilder<Polyhedron_3> builder(vertices, indices);
	icosahedron.delegate(builder);
	return icosahedron;
}


template<typename Polyhedron_3>
inline Polyhedron_3 subdivision_sphere(
	typename Polyhedron_3::Traits::Kernel::FT radius = 1.0,
	const Eigen::Matrix<typename Polyhedron_3::Traits::Kernel::FT, 3, 1>& center = { 0.0, 0.0, 0.0 },
	int iterations = 4)
{
	auto sphere = icosahedron<Polyhedron_3>(radius, center);
	CGAL::Subdivision_method_3::Loop_subdivision(sphere, iterations);
	return sphere;
}

} // namespace euclid