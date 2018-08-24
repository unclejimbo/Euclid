/** Geodesic distance.
 *
 *  This package contains algorithms for computing geodesic distance.
 *  @defgroup PkgGeodesics Geodesics
 *  @ingroup PkgGeometry
 */
#pragma once

#include <vector>
#include <CGAL/boost/graph/properties.h>
#include <CGAL/Kernel_traits.h>
#include <Eigen/SparseCholesky>
#include <Euclid/Util/Memory.h>

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
    /** Build up the necesssary computational components.
     *
     *  Compute the Laplacian matrix and pre-factor the linear system.
     *
     *  @param mesh Target triangle mesh.
     *  @param scale The time scale of the heat diffusion, relative to the
     *  average edge length of the mesh.
     *  @param resolution The average edge length of the mesh, provide a value
     *  if you have already computed it, otherwise it'll be computed internally.
     *  @param cot_mat The cotangent matrix of the mesh, provide a value if you
     *  have already computed it, otherwise it'll be computed internally.
     *  @param mass_mat The mass matrix of the mesh, provide a value if you have
     *  already computed it, otherwise it'll be computed internally.
     */
    void build(const Mesh& mesh,
               float scale = 1.0f,
               FT resolution = 0,
               const SpMat* cot_mat = nullptr,
               const SpMat* mass_mat = nullptr);

    /** Reset the time scale.
     *
     *  @param scale The time scale of the heat diffusion, relative to the
     *  average edge length of the mesh.
     */
    void scale(float scale);

    /** Compute geodesics distance from a vertex.
     *
     *  @param v The vertex descriptor.
     *  @param geodesics The output geodesics distances from all the mesh
     *  vertices to the target vertex v.
     */
    template<typename T>
    void compute(
        const typename boost::graph_traits<const Mesh>::vertex_descriptor& v,
        std::vector<T>& geodesics);

public:
    /** The target mesh.
     *
     */
    const Mesh* mesh = nullptr;

    /** The resolution of mesh.
     *
     *  Average edge length.
     */
    FT resolution = 0.0;

    /** Cotangent matrix.
     *
     *  If you directly set this member without calling the build, you need to
     *  update heat_solver and poisson_solver manually.
     */
    ProPtr<const SpMat> cot_mat = nullptr;

    /** Mass matrix.
     *
     *  If you directly set this member without calling the build, you need to
     *  update heat_solver and poisson_solver manually.
     */
    ProPtr<const SpMat> mass_mat = nullptr;

    /** The heat equation solver.
     *
     */
    Eigen::SimplicialLDLT<SpMat> heat_solver;

    /** The poisson equation solver.
     *
     */
    Eigen::SimplicialLDLT<SpMat> poisson_solver;
};

/** @}*/
} // namespace Euclid

#include "src/HeatMethod.cpp"
