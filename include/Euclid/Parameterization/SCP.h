/**Spectral conformal parameterization.
 *
 * **Reference**
 *
 * [1] Mullen, P., etc.
 * Spectral conformal parameterization.
 *
 * @defgroup PkgSCP Spectral conformal parameterization.
 * @ingroup PkgParameterization
 */
#pragma once

#include <CGAL/Surface_mesh_parameterization/Error_code.h>

namespace Euclid
{
/**@{*/

/**Compute planar conformal parameterization with dcp algorithm.
 *
 * @param mesh Input mesh.
 * @param uvm Vertex uv-coordinate property map.
 */
template<typename Mesh, typename VertexUVMap>
void spectral_conformal_parameterization(Mesh& mesh, VertexUVMap uvm);

template<typename Mesh>
class SCP_parameterizer_3
{
public:
    using TriangleMesh = Mesh;
    using halfedge_descriptor =
        typename boost::graph_traits<TriangleMesh>::halfedge_descriptor;
    using Scalar = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                    value_type>::Kernel::FT;

public:
    template<typename VertexUVMap,
             typename VertexIndexMap,
             typename VertexParameterizedMap>
    CGAL::Surface_mesh_parameterization::Error_code parameterize(
        TriangleMesh& mesh,
        halfedge_descriptor bhd,
        VertexUVMap uvmap,
        VertexIndexMap vimap,
        VertexParameterizedMap vpmap);
};

/**@}*/
} // namespace Euclid

#include "src/SCP.cpp"
