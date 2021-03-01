#include <queue>
#include <unordered_set>
#include <CGAL/boost/graph/iterator.h>

namespace Euclid
{

template<typename Mesh>
std::vector<vertex_t<Mesh>> nring_vertices(vertex_t<Mesh> target,
                                           const Mesh& mesh,
                                           unsigned n)
{
    using Vertex = vertex_t<Mesh>;
    std::vector<Vertex> neighbors;
    if (n == 1) {
        for (auto v : CGAL::vertices_around_target(target, mesh)) {
            neighbors.push_back(v);
        }
    }
    else if (n > 1) {
        std::unordered_set<Vertex> visited{ target };
        std::queue<std::pair<Vertex, unsigned>> targets;
        targets.emplace(target, n);
        do {
            auto t = targets.front().first;
            auto m = targets.front().second;
            targets.pop();
            for (auto v : CGAL::vertices_around_target(t, mesh)) {
                if (visited.find(v) == visited.end()) {
                    visited.insert(v);
                    neighbors.push_back(v);
                    if (m > 1) {
                        targets.emplace(v, m - 1);
                    }
                }
            }
        } while (!targets.empty());
    }
    return neighbors;
}

template<typename Mesh>
std::pair<halfedge_t<Mesh>, halfedge_t<Mesh>> find_common_edge(const Mesh& mesh,
                                                               face_t<Mesh> f1,
                                                               face_t<Mesh> f2)
{
    for (auto h1 : CGAL::halfedges_around_face(halfedge(f1, mesh), mesh)) {
        if (face(opposite(h1, mesh), mesh) == f2) {
            auto h2 = halfedge(f2, mesh);
            return std::make_pair(h1, h2);
        }
    }
    halfedge_t<Mesh> h;
    return std::make_pair(h, h);
}

} // namespace Euclid
