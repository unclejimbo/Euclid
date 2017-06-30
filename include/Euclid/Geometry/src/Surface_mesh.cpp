namespace Euclid
{

template<typename Point_3>
bool inline build_surface_mesh(
	const std::vector<Point_3>& vertices,
	const std::vector<unsigned>& indices,
	CGAL::Surface_mesh<Point_3>& mesh)
{
	auto nv = static_cast<CGAL::Surface_mesh<Point_3>::size_type>(vertices.size());
	auto nf = static_cast<CGAL::Surface_mesh<Point_3>::size_type>(indices.size() / 3);
	auto ne = static_cast<CGAL::Surface_mesh<Point_3>::size_type>(nf + nv - 2); // Euler-characteristic
	mesh.reserve(nv, ne, nf);

	std::vector<Surface_mesh::Vertex_index> vindices;
	vindices.reserve(nv);
	for (const auto& v : vertices) {
		vindices.push_back(mesh.add_vertex(v));
	}
	for (auto i = 0; i < indices.size(); i += 3) {
		auto v0 = vindices[indices[i + 0]];
		auto v1 = vindices[indices[i + 1]];
		auto v2 = vindices[indices[i + 2]];
		mesh.add_face(v0, v1, v2);
	}

	if (mesh.is_valid()) {
		return true;
	}
	else {
		std::cerr << "Input mesh is not a valid Surface_mesh" << std::endl;
		return false;
	}
}

template<typename Point_3, typename FT>
bool inline build_surface_mesh(
	const std::vector<Eigen::Matrix<FT, 3, 1>>& vertices,
	const std::vector<unsigned>& indices,
	CGAL::Surface_mesh<Point_3>& mesh)
{
	auto nv = static_cast<CGAL::Surface_mesh<Point_3>::size_type>(vertices.size());
	auto nf = static_cast<CGAL::Surface_mesh<Point_3>::size_type>(indices.size() / 3);
	auto ne = static_cast<CGAL::Surface_mesh<Point_3>::size_type>(nf + nv - 2); // Euler-characteristic
	mesh.reserve(nv, ne, nf);

	std::vector<Surface_mesh::Vertex_index> vindices;
	vindices.reserve(nv);
	for (const auto& v : vertices) {
		auto p = Point_3(v(0, 0), v(1, 0), v(2, 0));
		vindices.push_back(mesh.add_vertex(p));
	}
	for (auto i = 0; i < indices.size(); i += 3) {
		auto v0 = vindices[indices[i + 0]];
		auto v1 = vindices[indices[i + 1]];
		auto v2 = vindices[indices[i + 2]];
		mesh.add_face(v0, v1, v2);
	}

	if (mesh.is_valid()) {
		return true;
	}
	else {
		std::cerr << "Input mesh is not a valid Surface_mesh" << std::endl;
		return false;
	}
}

} // namespace Euclid