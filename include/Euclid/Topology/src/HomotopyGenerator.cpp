#include <algorithm>
#include <queue>
#include <vector>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>
#include <CGAL/boost/graph/iterator.h>
#include <CGAL/boost/graph/properties.h>

namespace Euclid
{

namespace _impl
{

template<typename Mesh>
std::vector<vertex_t<Mesh>> mark_shortest_path(
    const Mesh& mesh,
    vertex_t<Mesh> src,
    std::vector<bool>& on_shortest_path_tree)
{
    auto vimap = get(boost::vertex_index, mesh);
    auto eimap = get(boost::edge_index, mesh);
    std::vector<vertex_t<Mesh>> predecessors(num_vertices(mesh));
    boost::dijkstra_shortest_paths(
        mesh,
        src,
        boost::predecessor_map(
            boost::make_iterator_property_map(predecessors.begin(), vimap)));

    for (auto v : vertices(mesh)) {
        if (v != src) {
            auto [h, exist] = halfedge(v, predecessors[get(vimap, v)], mesh);
            assert(exist == true);
            auto e = edge(h, mesh);
            on_shortest_path_tree[get(eimap, e)] = true;
        }
    }

    return predecessors;
}

template<typename Mesh>
void spanning_tree_on_dual_graph_not_cross_shortest_path_tree(
    const Mesh& mesh,
    const std::vector<bool>& on_shortest_path_tree,
    std::vector<bool>& cross_spanning_tree)
{
    using FD = face_t<Mesh>;
    auto fimap = get(CGAL::face_index, mesh);
    auto eimap = get(CGAL::edge_index, mesh);

    auto frange = faces(mesh);
    auto froot = *frange.first;
    std::queue<FD> q;
    q.push(froot);
    std::vector<bool> touched(num_faces(mesh), false);
    touched[get(fimap, froot)] = true;

    while (!q.empty()) {
        auto f = q.front();
        q.pop();
        for (auto h : CGAL::halfedges_around_face(halfedge(f, mesh), mesh)) {
            auto e = edge(h, mesh);
            auto eidx = get(eimap, e);
            if (!on_shortest_path_tree[eidx]) {
                auto ho = opposite(h, mesh);
                auto fo = face(ho, mesh);
                auto fidx = get(fimap, fo);
                if (!touched[fidx]) {
                    touched[fidx] = true;
                    q.push(fo);
                    cross_spanning_tree[eidx] = true;
                }
            }
        }
    }
}

template<typename Mesh>
VertexChain<Mesh> find_generator(
    const Mesh& mesh,
    edge_t<Mesh> e,
    vertex_t<Mesh> src,
    const std::vector<vertex_t<Mesh>>& predecessors)
{
    auto vimap = get(boost::vertex_index, mesh);
    auto h = halfedge(e, mesh);
    auto v1 = source(h, mesh);
    auto v2 = target(h, mesh);

    VertexChain<Mesh> generator;
    while (v1 != src) {
        generator.push_back(v1);
        v1 = predecessors[get(vimap, v1)];
    }
    generator.push_back(src);
    std::reverse(generator.begin(), generator.end());
    while (v2 != src) {
        generator.push_back(v2);
        v2 = predecessors[get(vimap, v2)];
    }
    return generator;
}

template<typename Mesh>
VertexChains<Mesh> find_generators(
    const Mesh& mesh,
    vertex_t<Mesh> src,
    const std::vector<bool>& on_shortest_path_tree,
    const std ::vector<bool>& cross_spanning_tree,
    const std ::vector<vertex_t<Mesh>>& predecessors)
{
    auto eimap = get(boost::edge_index, mesh);

    // find edges not on shortest path tree and crossing spanning tree
    std::vector<edge_t<Mesh>> left_edges;
    for (auto e : edges(mesh)) {
        auto eidx = get(eimap, e);
        if (!on_shortest_path_tree[eidx] && !cross_spanning_tree[eidx]) {
            left_edges.push_back(e);
        }
    }

    // find generators from left edges, should be 2G generators
    VertexChains<Mesh> generators;
    for (auto e : left_edges) {
        generators.push_back(find_generator(mesh, e, src, predecessors));
    }

    return generators;
}

} // namespace _impl

template<typename Mesh>
VertexChains<Mesh> greedy_homotopy_generators(const Mesh& mesh,
                                              vertex_t<Mesh> v)
{
    std::vector<bool> on_shortest_path_tree(num_edges(mesh), false);
    auto predecessors =
        _impl::mark_shortest_path(mesh, v, on_shortest_path_tree);

    std::vector<bool> cross_spanning_tree(num_edges(mesh), false);
    _impl::spanning_tree_on_dual_graph_not_cross_shortest_path_tree(
        mesh, on_shortest_path_tree, cross_spanning_tree);

    auto generators = _impl::find_generators(
        mesh, v, on_shortest_path_tree, cross_spanning_tree, predecessors);

    return generators;
}

} // namespace Euclid
