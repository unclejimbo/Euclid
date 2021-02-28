#pragma once
#include <Euclid/Config.h>
#ifdef EUCLID_USE_SPECTRA

#include <Eigen/Core>
#include <Euclid/MeshUtil/MeshDefs.h>

namespace Euclid
{
/**@{ @ingroup PkgDescriptor*/

/** Heat kernel signature.
 *
 *  HKS is a intrinsic, multiscale, local shape descriptor.
 *
 *  **Reference**
 *
 *  Sun J., Ovsjanikov M., Guibas L..
 *  A concise and provably informative multi-scale signature based on heat
 *  diffusion.
 *  Proceedings of the Symposium on Geometry Processing, 2009.
 */
template<typename Mesh>
class HKS
{
public:
    using FT = FT_t<Mesh>;
    using Vec = Eigen::Matrix<FT, Eigen::Dynamic, 1>;
    using Mat = Eigen::Matrix<FT, Eigen::Dynamic, Eigen::Dynamic>;

public:
    /** Build up the necessary computational components.
     *
     *  Compute eigen decomposition of the mesh Laplacian. Note that the result
     *  number of eigenstructures might be smaller than what is requested in the
     *  because of numerical issues and some may not converge.
     *
     *  @param mesh The target mesh.
     *  @param k Number of eigenvalues/eigenvectors to use.
     */
    void build(const Mesh& mesh, unsigned k = 300);

    /** Build up the necessary computational components.
     *
     *  From precomputed eigen decomposition.
     *
     *  @param mesh The target mesh.
     *  @param eigenvalues Precomputed eigenvalues.
     *  @param eigenfunctions Precomputed eigenfunctions.
     */
    void build(const Mesh& mesh,
               const Vec* eigenvalues,
               const Mat* eigenfunctions);

    /** Compute hks for all vertices.
     *
     *  @param hks Output heat kernel signatures
     *  @param tscales Number of time scales to use.
     *  @param tmin The minimum time value, default to -1 which will use the
     *  parameter setting described in the paper.
     *  @param tmax The maximum time value, default to -1 which will use the
     *  parameter setting described in the paper.
     */
    template<typename Derived>
    void compute(Eigen::ArrayBase<Derived>& hks,
                 unsigned tscales = 100,
                 float tmin = -1.0f,
                 float tmax = -1.0f);

private:
    const Mesh* _mesh;
    Mat _phi2;     // @f$\phi * \phi@f$.
    Vec _emlambda; // @f$e^{-\lambda}@f$.
    FT _lambda_max;
    FT _lambda_min;
};

/** @}*/
} // namespace Euclid

#include "src/HKS.cpp"

#endif
