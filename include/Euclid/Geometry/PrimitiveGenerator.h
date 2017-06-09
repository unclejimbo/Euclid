#pragma once
/******************************************************************
* Pacakge Overview:                                               *
* This package provides convenient functions to create primitives *
******************************************************************/
namespace Euclid
{

template<typename Polyhedron_3>
Polyhedron_3 icosahedron(
	typename Polyhedron_3::Traits::Kernel::FT radius = 1.0,
	const Eigen::Matrix<typename Polyhedron_3::Traits::Kernel::FT, 3, 1>& center = { 0.0, 0.0, 0.0 });

template<typename Polyhedron_3>
Polyhedron_3 subdivision_sphere(
	typename Polyhedron_3::Traits::Kernel::FT radius = 1.0,
	const Eigen::Matrix<typename Polyhedron_3::Traits::Kernel::FT, 3, 1>& center = { 0.0, 0.0, 0.0 },
	int iterations = 4);

} // namespace euclid

#include "src/PrimitiveGenerator.cpp"
