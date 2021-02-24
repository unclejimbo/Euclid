#include <map>
#include <Euclid/Topology/HomotopyGenerator.h>

namespace Euclid
{

template<typename Mesh, typename SEM, typename SVM>
void mark_seam_mesh_with_cut_graph(const Mesh& mesh,
                                   CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh)
{
    auto v = *(vertices(mesh).first);
    mark_seam_mesh_with_cut_graph(mesh, seam_mesh, v);
}

template<typename Mesh, typename SEM, typename SVM>
void mark_seam_mesh_with_cut_graph(
    const Mesh& mesh,
    CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh,
    typename boost::graph_traits<Mesh>::vertex_descriptor seed)
{
    auto generators = greedy_homotopy_generators(mesh, seed);
    for (const auto& g : generators) {
        for (size_t i = 0; i < g.size(); ++i) {
            seam_mesh.add_seam(g[i], g[(i + 1) % g.size()]);
        }
    }
}

template<typename Mesh,
         typename SEM,
         typename SVM,
         typename UVMap,
         typename FT,
         typename IT>
void extract_seam_mesh(const CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh,
                       const UVMap uvmap,
                       std::vector<FT>& positions,
                       std::vector<FT>& texcoords,
                       std::vector<IT>& indices)
{
    positions.reserve(num_vertices(seam_mesh) * 3);
    texcoords.reserve(num_vertices(seam_mesh) * 2);
    indices.reserve(num_faces(seam_mesh) * 3);

    auto vpmap = get(boost::vertex_point, seam_mesh);
    using vd = typename CGAL::Seam_mesh<Mesh, SEM, SVM>::vertex_descriptor;
    std::map<vd, int> vimap;
    int cnt = 0;

    for (auto v : vertices(seam_mesh)) {
        auto p = get(vpmap, v);
        auto uv = get(uvmap, v);
        positions.push_back(p.x());
        positions.push_back(p.y());
        positions.push_back(p.z());
        texcoords.push_back(uv.x());
        texcoords.push_back(uv.y());
        vimap[v] = cnt++;
    }

    for (auto f : faces(seam_mesh)) {
        for (auto v : vertices_around_face(halfedge(f, seam_mesh), seam_mesh)) {
            indices.push_back(vimap[v]);
        }
    }
}

} // namespace Euclid
