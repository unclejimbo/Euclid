/**Delaunay mesh.
 *
 * **Reference**
 *
 * [1] Dyer, R., etc.
 * Delaunay mesh construction.
 *
 * @defgroup PkgDelaunayTriangulation Delaunay triangulation.
 * @ingroup PkgSurfaceDelaunay
 */
#pragma once

#include <Euclid/MeshUtil/MeshDefs.h>

namespace Euclid
{
/**@{*/

/**Test if a mesh is Delaunay.
 *
 * Return true if all edges are locally Delaunay.
 */
template<typename Mesh>
bool is_delaunay(const Mesh& mesh, double eps = 1e-14);

/**Test if an edge is locally Delaunay.
 *
 * Return true if a1 + a2 <= PI, where a1, a2 are the angles of opposite
 * corners.
 */
template<typename Mesh>
bool is_delaunay(const Mesh& mesh, edge_t<Mesh> e, double eps = 1e-14);

/**Schemes for remeshing a mesh to be Delaunay.
 *
 */
enum class RemeshDelaunayScheme
{
    /**Just flip non-locally Delaunay edges untils they disappear.
     *
     * This will change the underlying geometry when flipping non-planar edges.
     * Some edges cannot be flipped (cf. Fig 6 [1]), thus they output is not
     * guaranteed to be Delaunay.
     */
    SimpleFlip,

    /**Combine edge flip with edge split.
     *
     * This will keep the underlying geometry intact, since non-planar edges are
     * not flipped. Instead, a lot more vertices will be inserted into the mesh.
     */
    GeometryPreserving,

    /**A combination of the simple flip and geometry presvering scheme.
     *
     * Do edge split only on edges with a dihedral angle larger than a
     * threshold. Otherwise just flip that edge, which will incur a small
     * geometric change.
     */
    FeaturePreserving,
};

/**The local mesh elements when splitting.
 *
 *     v             v
 *    / \           /|\
 *   /   \         / | \
 *  p-----q ===>  p--s--q
 *   \   /         \ | /
 *    \ /           \|/
 *     u             u
 */
template<typename Mesh>
struct SplitSite
{
    edge_t<Mesh> epq;
    vertex_t<Mesh> vs;
    halfedge_t<Mesh> hps;
    halfedge_t<Mesh> hqs;
    halfedge_t<Mesh> hus;
    halfedge_t<Mesh> hvs;
};

/**A dumb remesh delaunay visitor.
 *
 * Define your own visitor following this concept.
 */
template<typename Mesh>
struct RemeshDelaunayVisitor
{
    /**Called before algorithm starts.
     *
     */
    void on_started(Mesh&) {}

    /**Called after algorithm finishes.
     *
     */
    void on_finished(Mesh&) {}

    /**Called before flipping an edge.
     *
     */
    void on_flipping(Mesh&, const edge_t<Mesh>&) {}

    /**Called after flipping an edge.
     *
     */
    void on_flipped(Mesh&, const edge_t<Mesh>&) {}

    /**Called if an edge is not flippable.
     *
     */
    void on_nonflippable(Mesh&, const edge_t<Mesh>&) {}

    /**Called before splitting an edge.
     *
     * Invalid for SimpleFlip scheme.
     */
    void on_splitting(Mesh&, const edge_t<Mesh>&) {}

    /**Called after splitting an edge.
     *
     * Invalid for SimpleFlip scheme.
     */
    void on_split(Mesh&, const SplitSite<Mesh>&) {}
};

/**Remesh an arbitary mesh into Delaunay mesh.
 *
 * @param mesh Input mesh.
 * @param scheme Remesh scheme.
 * @param dihedral_angle The threshold used for FeaturePreserving scheme.
 */
template<typename Mesh>
void remesh_delaunay(
    Mesh& mesh,
    RemeshDelaunayScheme scheme = RemeshDelaunayScheme::SimpleFlip,
    double dihedral_angle = 10.0);

/**Remesh an arbitary mesh into Delaunay mesh.
 *
 * @param mesh Input mesh.
 * @param visitor Remesh visitor.
 * @param scheme Remesh scheme.
 * @param dihedral_angle The threshold used for FeaturePreserving scheme.
 */
template<typename Mesh, typename Visitor>
void remesh_delaunay(
    Mesh& mesh,
    Visitor& visitor,
    RemeshDelaunayScheme scheme = RemeshDelaunayScheme::SimpleFlip,
    double dihedral_angle = 10.0);

/**@}*/
} // namespace Euclid

#include "src/DelaunayMesh.cpp"
