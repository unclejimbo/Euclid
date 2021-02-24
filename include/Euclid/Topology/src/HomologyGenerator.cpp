#include <queue>
#include <random>
#include <unordered_set>
#include <vector>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Topology/HomotopyGenerator.h>
#include <Euclid/Topology/MeshTopology.h>

// FIX CGAL BUG
#include <CGAL/hash_openmesh.h>
namespace std
{
template<>
struct hash<CGAL::internal::OMesh_edge<OpenMesh::HalfedgeHandle>>
    : public CGAL::cpp98::unary_function<OpenMesh::HalfedgeHandle, std::size_t>
{

    std::size_t operator()(
        const CGAL::internal::OMesh_edge<OpenMesh::HalfedgeHandle>& h) const
    {
        return h.idx();
    }
};
} // namespace std

namespace Euclid
{

namespace _impl
{

template<typename Mesh>
using EdgeSet = std::unordered_set<edge_t<Mesh>>;

template<typename Mesh>
double loop_length(const Mesh& mesh, const VertexChain<Mesh>& loop)
{
    double len = 0.0;
    for (size_t i = 0; i < loop.size(); ++i) {
        auto v0 = loop[i];
        auto v1 = loop[(i + 1) % loop.size()];
        auto h = halfedge(v0, v1, mesh);
        len += edge_length(h.first, mesh);
    }
    return len;
}

template<typename Mesh>
bool is_mesh_connected(const Mesh& mesh,
                       const EdgeSet<Mesh>& s1,
                       const EdgeSet<Mesh>& s2)
{
    auto fimap = get(boost::face_index, mesh);
    std::queue<face_t<Mesh>> queue;
    std::vector<bool> touched(num_faces(mesh), false);
    size_t ntouched = 0;

    auto froot = *(faces(mesh).first);
    queue.push(froot);
    touched[get(fimap, froot)] = true;
    ntouched = 1;

    while (!queue.empty()) {
        auto f = queue.front();
        queue.pop();
        for (auto h : CGAL::halfedges_around_face(halfedge(f, mesh), mesh)) {
            auto e = edge(h, mesh);
            if (s1.find(e) == s1.end() && s2.find(e) == s2.end()) {
                auto hoppo = opposite(h, mesh);
                auto foppo = face(hoppo, mesh);
                auto fidx = get(fimap, foppo);
                if (!touched[fidx]) {
                    queue.push(foppo);
                    touched[fidx] = true;
                    ++ntouched;
                }
            }
        }
    }

    return ntouched == num_faces(mesh);
}

} // namespace _impl

template<typename Mesh>
VertexChains<Mesh> greedy_homology_generators(const Mesh& mesh, double accept)
{
    std::random_device rd;
    std::mt19937 rng(rd());
    std::uniform_real_distribution<> dist;

    using Loop = std::pair<double, VertexChain<Mesh>>;
    std::vector<Loop> loops;
    for (auto v : vertices(mesh)) {
        if (dist(rng) <= accept) {
            auto homotopy_basis = greedy_homotopy_generators(mesh, v);
            for (const auto& b : homotopy_basis) {
                // discard loops randomly to speed up process
                loops.emplace_back(_impl::loop_length(mesh, b), b);
            }
        }
    }
    std::sort(loops.begin(), loops.end(), [](const Loop& l1, const Loop& l2) {
        return l1.first < l2.first;
    });

    VertexChains<Mesh> generators;
    const auto two_g = genus(mesh) * 2;
    _impl::EdgeSet<Mesh> added;
    for (const auto& l : loops) {
        _impl::EdgeSet<Mesh> running;
        for (size_t i = 0; i < l.second.size(); ++i) {
            auto v0 = l.second[i];
            auto v1 = l.second[(i + 1) % l.second.size()];
            auto h = halfedge(v0, v1, mesh);
            auto e = edge(h.first, mesh);
            running.insert(e);
        }
        if (_impl::is_mesh_connected(mesh, added, running)) {
            added.insert(running.begin(), running.end());
            generators.push_back(l.second);
        }
        if (generators.size() == two_g) {
            break;
        }
    }
    return generators;
}

} // namespace Euclid
