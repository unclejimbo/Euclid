#pragma once


/**********************************************************
* Package Overview:                                       *
* This package contains functions to build and operate on *
* Polyhedron_3 mesh in CGAL                               *
**********************************************************/
namespace Euclid
{

// Build Polyhedron_3 from vertices and indices
template<typename Polyhedron_3>
class TriMeshBuilder : public CGAL::Modifier_base<typename Polyhedron_3::HalfedgeDS>
{
	using FT = typename Polyhedron_3::Traits::Kernel::FT;
	using HDS = typename Polyhedron_3::HalfedgeDS;
	using Vec3 = Eigen::Matrix<typename Polyhedron_3::Traits::Kernel::FT, 3, 1>;

public:
	TriMeshBuilder(const std::vector<FT>& vertices, const std::vector<unsigned>& indices);
	TriMeshBuilder(const std::vector<Vec3>& vertices, const std::vector<unsigned>& indices);
	void operator()(HDS& hds);

private:
	std::vector<FT> _vertices;
	std::vector<unsigned> _indices;
};


template<typename Polyhedron_3>
Eigen::Matrix<typename Polyhedron_3::Traits::Kernel::FT, 3, 1>
compute_facet_normal(const typename Polyhedron_3::Face_handle::value_type& f);


template<typename Polyhedron_3>
decltype(auto)
compute_facet_area(const typename Polyhedron_3::Face_handle::value_type& f);

} // namespace Euclid

#include "src/Polyhedron_3.cpp"
