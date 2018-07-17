/** Geodesic distance.
 *
 *  This package contains algorithms for computing geodesic distance.
 *  @defgroup PkgGeodesics Geodesics
 *  @ingroup PkgGeometry
 */
#pragma once

#include <vector>
#include <CGAL/Kernel_traits.h>
#include <Eigen/SparseCholesky>

namespace Euclid
{
/** @{*/

/** Single source approximate geodesic distance using the heat method.
 *
 *  **Reference**
 *
 *  Crane K, Weischedel C, Wardetzky M.
 *  Geodesics in heat: A new approach to computing distance based on heat
 *  flow.
 *  ACM Transactions on Graphics (TOG), 2013.
 */
template<typename Mesh>
class GeodesicsInHeat
{
public:
    using Kernel = typename CGAL::Kernel_traits<typename boost::property_traits<
        typename boost::property_map<Mesh, boost::vertex_point_t>::type>::
                                                    value_type>::Kernel;
    using FT = typename Kernel::FT;
    using Vector_3 = typename Kernel::Vector_3;
    using SpMat = Eigen::SparseMatrix<FT>;

public:
    /** Default constructor.*/
    GeodesicsInHeat() = default;

    /** Build the compute structure.
     *
     *  @param mesh Target triangle mesh.
     *  @param scale The time scale of the heat diffusion, relative to the mean
     *  edge length of the mesh.
     */
    explicit GeodesicsInHeat(const Mesh& mesh, float scale = 1.0f);

    /** Build the compute structure.
     *
     *  @param mesh Target triangle mesh.
     *  @param scale The time scale of the heat diffusion, relative to the mean
     *  edge length of the mesh.
     */
    bool build(const Mesh& mesh, float scale = 1.0f);

    /** Reset the time scale.
     *
     *  @param scale The time scale of the heat diffusion, relative to the mean
     *  edge length of the mesh.
     */
    bool scale(float scale);

    /** Compute geodesics distance from a vertex.
     *
     *  @param vertex_descriptor The vertex descriptor.
     *  @param gedesics The output geodesics distances from all the mesh
     *  vertices to the target vertex.
     */
    template<typename T>
    bool compute(
        const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
        std::vector<T>& geodesics);

private:
    const Mesh* _mesh;
    FT _avg_spacing;
    SpMat _cot_mat;
    SpMat _mass_mat;
    Eigen::SimplicialLDLT<SpMat> _heat_solver;
    Eigen::SimplicialLDLT<SpMat> _poisson_solver;
};

/** @}*/
} // namespace Euclid

#include "src/HeatMethod.cpp"
