#include <map>

namespace Euclid
{

template<typename Mesh>
int num_boundaries(const Mesh& mesh)
{
    // himap is not always available so compute it on the fly
    using hd = boost::graph_traits<Mesh>::halfedge_descriptor;
    std::map<hd, int> himap;
    int cnt = 0;
    for (auto h : halfedges(mesh)) {
        himap[h] = cnt++;
    }

    cnt = 0;
    std::vector<bool> visited(num_halfedges(mesh));
    for (auto h : halfedges(mesh)) {
        auto hidx = himap[h];
        if (!visited[hidx]) {
            visited[hidx] = true;
            if (CGAL::is_border(h, mesh)) {
                auto hend = h;
                auto hi = next(h, mesh);
                while (hi != hend) {
                    visited[himap[hi]] = true;
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
    auto nb = num_boundaries(mesh);
    return (2 - euler - nb) / 2;
}

} // namespace Euclid
