#include <CGAL/Polyhedron_incremental_builder_3.h>

namespace Euclid
{

template<typename Polyhedron_3>
inline TriMeshBuilder<Polyhedron_3>::TriMeshBuilder(const std::vector<FT>& vertices, const std::vector<unsigned>& indices)
	: _vertices(vertices), _indices(indices)
{
}

template<typename Polyhedron_3>
inline TriMeshBuilder<Polyhedron_3>::TriMeshBuilder(const std::vector<Vec3>& vertices, const std::vector<unsigned>& indices)
	: _indices(indices)
{
	_vertices.reserve(vertices.size() / 3);
	for (const auto& v : vertices) {
		_vertices.push_back(v(0));
		_vertices.push_back(v(1));
		_vertices.push_back(v(2));
	}
}

template<typename Polyhedron_3>
inline TriMeshBuilder<Polyhedron_3>::TriMeshBuilder(const std::vector<Point_3>& vertices, const std::vector<unsigned>& indices)
	: _indices(indices)
{
	_vertices.reserve(vertices.size() / 3);
	for (const auto& v : vertices) {
		_vertices.push_back(v.x());
		_vertices.push_back(v.y());
		_vertices.push_back(v.z());
	}
}

template<typename Polyhedron_3>
inline void TriMeshBuilder<Polyhedron_3>::operator()(HDS& hds)
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

} // namespace Euclid
