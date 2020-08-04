namespace Euclid
{

template<typename Mesh>
int num_bondaries(const Mesh& mesh)
{
    auto himap = get(boost::halfedge_index, mesh);
    int cnt = 0;
    std::vector<bool> visited(num_halfedges(mesh));
    for (auto h : halfedges(mesh)) {
        auto hidx = get(himap, h);
        if (!visited[hidx]) {
            visited[hidx] = true;
            if (CGAL::is_border(h, mesh)) {
                auto hend = h;
                auto hi = next(h, mesh);
                while (hi != hend) {
                    visited[get(himap, hi)] = true;
                    hi = next(hi, mesh);
                }
                ++cnt;
            }
        }
    }
    return cnt;
}

template<typename Mesh>
int euler_characteristic(const Mesh& mesh)
{
    auto nv = static_cast<int>(num_vertices(mesh));
    auto ne = static_cast<int>(num_edges(mesh));
    auto nf = static_cast<int>(num_faces(mesh));
    return nv - ne + nf;
}

template<typename Mesh>
int genus(const Mesh& mesh)
{
    int euler = euler_characteristic(mesh);
    auto nb = num_bondaries(mesh);
    return (2 - euler - nb) / 2;
}

} // namespace Euclid
