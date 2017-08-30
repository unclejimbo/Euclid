/******************************************************************
* Pacakge Overview:                                               *
* This package provides convenient functions to create primitives *
******************************************************************/
#pragma once

namespace Euclid
{

template<typename Mesh>
class PrimitiveGenerator
{
	using Point_3 = typename boost::property_traits<
		typename boost::property_map<Mesh, boost::vertex_point_t>
		::type>::value_type;

public:
	template<typename T>
	static void icosahedron(
		Mesh& mesh,
		T radius = 1.0,
		const Point_3& center = { 0.0, 0.0, 0.0 });

	template<typename T>
	static void subdivision_sphere(
		Mesh& mesh,
		T radius = 1.0,
		const Point_3& center = { 0.0, 0.0, 0.0 },
		int iterations = 4);
};

} // namespace Euclid

#include "src/PrimitiveGenerator.cpp"
