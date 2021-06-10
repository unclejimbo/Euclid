#include <queue>
#include <unordered_set>
#include <boost/math/constants/constants.hpp>
#include <CGAL/boost/graph/Euler_operations.h>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Math/Numeric.h>

namespace Euclid
{

namespace _impl
{

template<typename Mesh>
void init_queue(const Mesh& mesh,
                std::queue<edge_t<Mesh>>& queue,
                std::vector<bool>& inqueue)
{
    auto eimap = get(boost::edge_index, mesh);
    inqueue.resize(num_edges(mesh), false);
    for (auto e : edges(mesh)) {
        if (!is_delaunay(mesh, e)) {
            queue.push(e);
            inqueue[get(eimap, e)] = true;
        }
    }
}

template<typename Mesh>
void init_physical_edges(const Mesh& mesh, std::vector<bool>& physical)
{
    auto eimap = get(boost::edge_index, mesh);
    physical.resize(num_edges(mesh), true);
    for (auto e : edges(mesh)) {
        if (eq_almost(dihedral_angle(e, mesh),
                      boost::math::constants::pi<FT_t<Mesh>>())) {
            physical[get(eimap, e)] = false;
        }
    }
}

template<typename Mesh, typename Visitor>
void flip(Mesh& mesh,
          Visitor& visitor,
          edge_t<Mesh> e,
          std::queue<edge_t<Mesh>>& queue,
          std::vector<bool>& inqueue)
{
    auto eimap = get(boost::edge_index, mesh);
    auto h = halfedge(e, mesh);
    auto ho = opposite(h, mesh);
    auto ha = next(h, mesh);
    auto hb = next(ha, mesh);
    auto hc = next(ho, mesh);
    auto hd = next(hc, mesh);
    auto ea = edge(ha, mesh);
    auto eb = edge(hb, mesh);
    auto ec = edge(hc, mesh);
    auto ed = edge(hd, mesh);
    visitor.on_flipping(mesh, e);
    CGAL::Euler::flip_edge(h, mesh);
    visitor.on_flipped(mesh, e);
    if (!inqueue[get(eimap, ea)]) {
        queue.push(ea);
        inqueue[get(eimap, ea)] = true;
    }
    if (!inqueue[get(eimap, eb)]) {
        queue.push(eb);
        inqueue[get(eimap, eb)] = true;
    }
    if (!inqueue[get(eimap, ec)]) {
        queue.push(ec);
        inqueue[get(eimap, ec)] = true;
    }
    if (!inqueue[get(eimap, ed)]) {
        queue.push(ed);
        inqueue[get(eimap, ed)] = true;
    }
}

template<typename Mesh>
bool is_flip_topologically_ok(const Mesh& mesh, edge_t<Mesh>& e)
{
    auto h = halfedge(e, mesh);
    return !halfedge(target(next(h, mesh), mesh),
                     target(next(opposite(h, mesh), mesh), mesh),
                     mesh)
                .second;
}

template<typename Mesh, typename Visitor>
void split(Mesh& mesh,
           Visitor& visitor,
           edge_t<Mesh> e,
           std::queue<edge_t<Mesh>>& queue,
           std::vector<bool>& inqueue,
           std::vector<bool>& physical_edges,
           std::unordered_set<vertex_t<Mesh>>& split_vertices)
{
    auto hpq = halfedge(e, mesh);
    auto hqp = opposite(hpq, mesh);
    auto hqv = next(hpq, mesh);
    auto hvp = prev(hpq, mesh);
    auto hpu = next(hqp, mesh);
    auto huq = prev(hqp, mesh);

    // do edge split
    visitor.on_splitting(mesh, e);
    auto hps = CGAL::Euler::split_edge(hpq, mesh);
    auto hsq = next(hps, mesh);
    auto hqs = opposite(hsq, mesh);
    auto hsv = CGAL::Euler::split_face(hps, hqv, mesh);
    auto hsu = CGAL::Euler::split_face(hqs, hpu, mesh);
    auto vs = target(hps, mesh);

    SplitSite<Mesh> site;
    site.epq = e;
    site.vs = vs;
    site.hps = hps;
    site.hqs = opposite(hsq, mesh);
    site.hvs = opposite(hsv, mesh);
    site.hus = opposite(hsu, mesh);
    visitor.on_split(mesh, site);

    // update realted buffers
    auto eimap = get(boost::edge_index, mesh);
    auto eqv = edge(hqv, mesh);
    if (!inqueue[get(eimap, eqv)]) {
        queue.push(eqv);
        inqueue[get(eimap, eqv)] = true;
    }
    auto evp = edge(hvp, mesh);
    if (!inqueue[get(eimap, evp)]) {
        queue.push(evp);
        inqueue[get(eimap, evp)] = true;
    }
    auto epu = edge(hpu, mesh);
    if (!inqueue[get(eimap, epu)]) {
        queue.push(epu);
        inqueue[get(eimap, epu)] = true;
    }
    auto euq = edge(huq, mesh);
    if (!inqueue[get(eimap, euq)]) {
        queue.push(euq);
        inqueue[get(eimap, euq)] = true;
    }
    auto esp = edge(hps, mesh);
    auto esq = edge(hsq, mesh);
    auto esu = edge(hsu, mesh);
    auto esv = edge(hsv, mesh);
    // auto isp = get(eimap, esp);
    // auto isq = get(eimap, esq);
    // auto isu = get(eimap, esu);
    // auto isv = get(eimap, esv);
    queue.push(esp);
    queue.push(esq);
    queue.push(esu);
    queue.push(esv);
    inqueue[get(eimap, esq)] = true;
    inqueue.push_back(true);         // esp
    inqueue.push_back(true);         // esv
    inqueue.push_back(true);         // esu
    physical_edges.push_back(true);  // esp
    physical_edges.push_back(false); // esv
    physical_edges.push_back(false); // esu
    split_vertices.insert(vs);

    // determine new vertex position
    auto vpmap = get(boost::vertex_point, mesh);
    auto vp = target(hvp, mesh);
    auto vq = target(huq, mesh);
    auto pp = get(vpmap, vp);
    auto pq = get(vpmap, vq);
    auto pm = CGAL::midpoint(pp, pq);
    auto lm = length(pp - pm);
    auto l = 0.0;
    if (lm > 1.5) {
        l = 2.0;
        while (l < lm) {
            l *= 2.0;
        }
        if (std::abs(l - lm) > std::abs(l * 0.5 - lm)) {
            l *= 0.5;
        }
    }
    else if (lm < 0.75) {
        l = 0.5;
        while (l > lm) {
            l *= 0.5;
        }
        if (std::abs(l - lm) > std::abs(l * 2.0 - lm)) {
            l *= 2.0;
        }
    }
    else {
        l = 1.0;
    }
    if (split_vertices.find(vp) == split_vertices.end()) {
        auto ps = pp + normalized(pm - pp) * l;
        put(vpmap, vs, ps);
    }
    else {
        auto ps = pq + normalized(pm - pq) * l;
        put(vpmap, vs, ps);
    }
}

template<typename Mesh, typename Visitor>
void remesh_delaunay_simple_flip(Mesh& mesh, Visitor& visitor)
{
    visitor.on_started(mesh);
    auto eimap = get(boost::edge_index, mesh);
    std::queue<edge_t<Mesh>> queue;
    std::vector<bool> inqueue;
    init_queue(mesh, queue, inqueue);
    while (!queue.empty()) {
        auto e = queue.front();
        queue.pop();
        inqueue[get(eimap, e)] = false;
        if (!is_delaunay(mesh, e) && is_flip_topologically_ok(mesh, e)) {
            flip(mesh, visitor, e, queue, inqueue);
        }
        else {
            visitor.on_nonflippable(mesh, e);
        }
    }
    visitor.on_finished(mesh);
}

template<typename Mesh, typename Visitor>
void remesh_delaunay_geometry_preserving(Mesh& mesh, Visitor& visitor)
{
    visitor.on_started(mesh);
    auto eimap = get(boost::edge_index, mesh);
    std::queue<edge_t<Mesh>> queue;
    std::vector<bool> inqueue;
    init_queue(mesh, queue, inqueue);
    std::vector<bool> physical_edges;
    init_physical_edges(mesh, physical_edges);
    std::unordered_set<vertex_t<Mesh>> split_vertices;
    while (!queue.empty()) {
        auto e = queue.front();
        queue.pop();
        inqueue[get(eimap, e)] = false;
        if (!is_delaunay(mesh, e)) {
            if (!physical_edges[get(eimap, e)]) {
                flip(mesh, visitor, e, queue, inqueue);
            }
            else {
                visitor.on_nonflippable(mesh, e);
                split(mesh,
                      visitor,
                      e,
                      queue,
                      inqueue,
                      physical_edges,
                      split_vertices);
            }
        }
    }
    visitor.on_finished(mesh);
}

template<typename Mesh, typename Visitor>
void remesh_delaunay_feature_preserving(Mesh& mesh,
                                        Visitor& visitor,
                                        double threshold)
{
    visitor.on_started(mesh);
    auto eimap = get(boost::edge_index, mesh);
    std::queue<edge_t<Mesh>> queue;
    std::vector<bool> inqueue;
    init_queue(mesh, queue, inqueue);
    std::vector<bool> physical_edges;
    init_physical_edges(mesh, physical_edges);
    std::unordered_set<vertex_t<Mesh>> split_vertices;
    while (!queue.empty()) {
        auto e = queue.front();
        queue.pop();
        inqueue[get(eimap, e)] = false;
        if (!is_delaunay(mesh, e)) {
            if (!physical_edges[get(eimap, e)] ||
                dihedral_angle(e, mesh) > threshold) {
                flip(mesh, visitor, e, queue, inqueue);
            }
            else {
                visitor.on_nonflippable(mesh, e);
                split(mesh,
                      visitor,
                      e,
                      queue,
                      inqueue,
                      physical_edges,
                      split_vertices);
            }
        }
    }
    visitor.on_finished(mesh);
}

} // namespace _impl

template<typename Mesh>
bool is_delaunay(const Mesh& mesh, double eps)
{
    for (auto e : edges(mesh)) {
        if (!is_delaunay(mesh, e, eps)) {
            return false;
        }
    }
    return true;
}

template<typename Mesh>
bool is_delaunay(const Mesh& mesh, edge_t<Mesh> e, double eps)
{
    auto cot = static_cast<double>(cotangent_weight(e, mesh));
    return cot > eps;
}

template<typename Mesh>
void remesh_delaunay(Mesh& mesh,
                     RemeshDelaunayScheme scheme,
                     double dihedral_angle)
{
    RemeshDelaunayVisitor<Mesh> visitor;
    remesh_delaunay(mesh, visitor, scheme, dihedral_angle);
}

template<typename Mesh, typename Visitor>
void remesh_delaunay(Mesh& mesh,
                     Visitor& visitor,
                     RemeshDelaunayScheme scheme,
                     double dihedral_angle)
{
    switch (scheme) {
    case RemeshDelaunayScheme::FeaturePreserving:
        _impl::remesh_delaunay_feature_preserving(
            mesh,
            visitor,
            boost::math::constants::pi<double>() -
                dihedral_angle * boost::math::constants::degree<double>());
        break;
    case RemeshDelaunayScheme::GeometryPreserving:
        _impl::remesh_delaunay_geometry_preserving(mesh, visitor);
        break;
    case RemeshDelaunayScheme::SimpleFlip:
    default:
        _impl::remesh_delaunay_simple_flip(mesh, visitor);
        break;
    }
}

} // namespace Euclid
