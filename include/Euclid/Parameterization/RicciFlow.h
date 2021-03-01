/**Conformal parameterization using discrete Ricci flow.
 *
 * **Reference**
 *
 * [1] Jin, M., Kim, J., Luo, F., Gu, X.
 * Discrete Surface Ricci Flow.
 *
 * [2] Yang, YL., Guo, R., Luo, F., Hu, SM., Gu, X.
 * Generalized Discrete Ricci Flow.
 *
 * @defgroup PkgRicciFlow Ricci flow.
 * @ingroup PkgParameterization
 */
#pragma once

#include <CGAL/boost/graph/Seam_mesh.h>
#include <CGAL/Surface_mesh_parameterization/Error_code.h>

namespace Euclid
{
/**@{*/

/**Compute the circle packing metric.
 *
 * Inverse circle packing metric is used for better robustness.
 *
 * @param mesh Input mesh.
 * @param vrm Vertex radius property map.
 * @param ewm Edge weight property map.
 */
template<typename Mesh, typename VertexRadiusMap, typename EdgeWeightMap>
void circle_packing_metric(Mesh& mesh, VertexRadiusMap vrm, EdgeWeightMap ewm);

/**Embed the circle packing metric into plane.
 *
 * The uv-coordinates are stored in the seam mesh.
 *
 * @param mesh Input mesh.
 * @param vrm Vertex radius property map.
 * @param ewm Edge weight property map.
 * @param seam_mesh The seam mesh.
 * @param uvm Vertex uv-coordinate property map.
 */
template<typename Mesh,
         typename VertexRadiusMap,
         typename EdgeWeightMap,
         typename SEM,
         typename SVM,
         typename VertexUVMap>
void embed_circle_packing_metric(const Mesh& mesh,
                                 VertexRadiusMap vrm,
                                 EdgeWeightMap ewm,
                                 CGAL::Seam_mesh<Mesh, SEM, SVM>& seam_mesh,
                                 VertexUVMap uvm);

/**Ricci flow solver types.
 *
 */
enum class RicciFlowSolverType
{
    /**Gradient descent solver.*/
    GradientDescent,
    /**Newton solver.*/
    Newton
};

/**Ricci flow solver settings.
 *
 */
struct RicciFlowSolverSettings
{
    /**Solver type.*/
    RicciFlowSolverType type = RicciFlowSolverType::GradientDescent;
    /**Step size.*/
    double step = 5e-2;
    /**Termination threshold.*/
    double eps = 1e-5;
    /**Maximum number of iterations.*/
    int max_iters = 5000;
    /**Print solver status.*/
    bool verbose = false;
};

enum class RicciFlowSolverStatus
{
    Optimal,
    Suboptimal,
    InvalidInput,
};

/**The discrete Ricci flow.
 *
 * Optimize the given circle packing metric using Ricci flow.
 *
 * @param mesh Input mesh.
 * @param vrm Vertex radius property map.
 * @param ewm Edge weight property map.
 * @param vcm Vertex target curvature map.
 * @param settings Solver settings.
 */
template<typename Mesh,
         typename VertexRadiusMap,
         typename EdgeWeightMap,
         typename VertexCurvatureMap>
RicciFlowSolverStatus ricci_flow(
    Mesh& mesh,
    VertexRadiusMap vrm,
    EdgeWeightMap ewm,
    VertexCurvatureMap vcm,
    const RicciFlowSolverSettings& settings = RicciFlowSolverSettings());

/**Compute plane parameterization with Ricci flow.
 *
 * This class is compatible with
 * CGAL::Surface_mesh_parameterization::parameterize().
 */
template<typename Mesh, typename SEM, typename SVM>
class Ricci_flow_parameterizer3
{
public:
    using TriangleMesh = CGAL::Seam_mesh<Mesh, SEM, SVM>;
    using halfedge_descriptor =
        typename boost::graph_traits<TriangleMesh>::halfedge_descriptor;
    using Scalar = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                    value_type>::Kernel::FT;

public:
    /**Constructor.
     *
     * @param mesh Input mesh.
     */
    explicit Ricci_flow_parameterizer3(const Mesh& mesh);

    /**Set solver settings.
     *
     * @param settings Settings.
     */
    void set_solver_settings(const RicciFlowSolverSettings& settings);

    /**Add cone singularities.
     *
     * Set vertex v as a cone of @f$2k\pi@f$.
     *
     * @param v The target vertex.
     * @param k The target gaussian curvature of v.
     */
    void add_cone(typename boost::graph_traits<Mesh>::vertex_descriptor v,
                  Scalar k);

    /**Parameterize the surface.
     *
     * Only CGAL::Seam_mesh is supported. Note some parameters are not used
     * internally but only serves to be consistent with the CGAL interface.
     *
     * @param mesh The seam mesh.
     * @param bhd A boundary halfedge, not used.
     * @param uvmap A property map to store the uv coordinates.
     * @param vimap A property map to store the vertex index, not used.
     * @param vpmap A property map to store if a vertex has been parameterized.
     */
    template<typename VertexUVMap,
             typename VertexIndexMap,
             typename VertexParameterizedMap>
    CGAL::Surface_mesh_parameterization::Error_code parameterize(
        TriangleMesh& mesh,
        halfedge_descriptor bhd,
        VertexUVMap uvmap,
        VertexIndexMap vimap,
        VertexParameterizedMap vpmap);

private:
    using vd = typename boost::graph_traits<Mesh>::vertex_descriptor;
    using ed = typename boost::graph_traits<Mesh>::edge_descriptor;
    using VRM = std::map<vd, Scalar>;
    using VRPM = boost::associative_property_map<VRM>;
    using EWM = std::map<ed, Scalar>;
    using EWPM = boost::associative_property_map<EWM>;
    using VCM = std::map<vd, Scalar>;
    using VCPM = boost::associative_property_map<VCM>;

private:
    const Mesh& _underlying_mesh;
    RicciFlowSolverSettings _settings;
    VRM _vrm;
    VRPM _vrpm;
    EWM _ewm;
    EWPM _ewpm;
    VCM _vcm;
    VCPM _vcpm;
};

/**@}*/
} // namespace Euclid

#include "src/RicciFlow.cpp"
