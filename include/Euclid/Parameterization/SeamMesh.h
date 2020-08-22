/**Seam mesh utilities.
 *
 * @defgroup PkgSeamMesh Seam mesh
 * @ingroup PkgParameterization
 */
#pragma once

#include <vector>
#include <boost/graph/graph_traits.hpp>
#include <CGAL/boost/graph/Seam_mesh.h>

namespace Euclid
{
/**@{*/

/**Mark the cut graph of a high-genus mesh as seams.
 *
 * Homotopy generators is used here as the cut graph.
 */
template<typename Mesh, typename SEM, typename SVM>
void mark_seam_mesh_with_cut_graph(const Mesh& mesh,
                                   CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh);

/**Mark the cut graph of a high-genus mesh as seams.
 *
 * Homotopy generators is used here as the cut graph.
 */
template<typename Mesh, typename SEM, typename SVM>
void mark_seam_mesh_with_cut_graph(
    const Mesh& mesh,
    CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh,
    typename boost::graph_traits<Mesh>::vertex_descriptor seed);

/**Extract the geometry from a seam mesh.
 *
 */
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
                       std::vector<IT>& indices);

/**@}*/
} // namespace Euclid

#include "src/SeamMesh.cpp"
