/**********************************************************
* Package Overview:                                       *
* This package contains helper functions for Polyhedron_3 *
**********************************************************/
#pragma once
#include <CGAL/Polyhedron_3.h>

namespace Euclid
{

// Build Polyhedron_3 from vertices and indices
template<typename Polyhedron_3>
class TriMeshBuilder : public CGAL::Modifier_base<typename Polyhedron_3::HalfedgeDS>
{
	using FT = typename Polyhedron_3::Traits::Kernel::FT;
	using HDS = typename Polyhedron_3::HalfedgeDS;
	using Point_3 = typename Polyhedron_3::Traits::Kernel::Point_3;

public:
	TriMeshBuilder(const std::vector<FT>& vertices, const std::vector<unsigned>& indices);
	TriMeshBuilder(const std::vector<Point_3>& vertices, const std::vector<unsigned>& indices);
	void operator()(HDS& hds);

private:
	std::vector<FT> _vertices;
	std::vector<unsigned> _indices;
};

} // namespace Euclid

#include "src/Polyhedron_3.cpp"
