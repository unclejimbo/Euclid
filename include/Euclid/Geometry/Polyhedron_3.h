#pragma once

#include <CGAL/Polyhedron_incremental_builder_3.h>

/**********************************************************
* Package Overview:                                       *
* This package contains functions to build and operate on *
* Polyhedron_3 mesh in CGAL                               *
**********************************************************/
namespace Euclid
{

template<typename Polyhedron_3>
class TriMeshBuilder : public CGAL::Modifier_base<typename Polyhedron_3::HalfedgeDS>
{
	using FT = typename Polyhedron_3::Traits::Kernel::FT;
	using HDS = typename Polyhedron_3::HalfedgeDS;
	using Vec3 = Eigen::Matrix<typename Polyhedron_3::Traits::Kernel::FT, 3, 1>;

public:
	TriMeshBuilder(const std::vector<FT>& vertices, const std::vector<unsigned>& indices)
		: _vertices(vertices), _indices(indices)
	{
	}

	TriMeshBuilder(const std::vector<Vec3>& vertices, const std::vector<unsigned>& indices)
		: _indices(indices)
	{
		_vertices.reserve(vertices.size() / 3);
		for (const auto& v : vertices) {
			_vertices.push_back(v(0));
			_vertices.push_back(v(1));
			_vertices.push_back(v(2));
		}
	}

	void operator()(HDS& hds)
	{
		CGAL::Polyhedron_incremental_builder_3<HDS> builder(hds, true);
		builder.begin_surface(_vertices.size(), _indices.size() / 3);

		for (size_t i = 0; i < _vertices.size(); i += 3) {
			builder.add_vertex(HDS::Vertex::Point(
				_vertices[i], _vertices[i + 1], _vertices[i + 2]));
		}

		for (size_t i = 0; i < _indices.size(); ++i) {
			builder.begin_facet();
			builder.add_vertex_to_facet(_indices[i++]);
			builder.add_vertex_to_facet(_indices[i++]);
			builder.add_vertex_to_facet(_indices[i]);
			builder.end_facet();
		}

		builder.end_surface();
	}

private:
	std::vector<FT> _vertices;
	std::vector<unsigned> _indices;
};


template<typename Polyhedron_3>
decltype(auto)
compute_facet_normal(const typename Polyhedron_3::Face_handle::value_type& f)
{
	auto n = CGAL::normal(
		f.facet_begin()->vertex()->point(),
		f.facet_begin()->next()->vertex()->point(),
		f.facet_begin()->opposite()->vertex()->point());
	return Eigen::Matrix<typename Polyhedron_3::Traits::Kernel::FT, 3, 1>(
		n.x(), n.y(), n.z()).normalized();
}


template<typename Polyhedron_3>
decltype(auto)
compute_facet_area(const typename Polyhedron_3::Face_handle::value_type& f)
{
	return Polyhedron_3::Traits::Kernel::Compute_area_3()(
		f.halfedge()->vertex()->point(),
		f.halfedge()->next()->vertex()->point(),
		f.halfedge()->opposite()->vertex()->point());
}

} // namespace Euclid