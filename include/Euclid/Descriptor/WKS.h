#pragma once
#include <Euclid/Config.h>
#ifdef EUCLID_USE_SPECTRA

#include <Eigen/Core>
#include <Euclid/MeshUtil/MeshDefs.h>

namespace Euclid
{
/**@{ @ingroup PkgDescriptor*/

/** Wave kernel signature.
 *
 *  WKS is a intrinsic, multiscale, local shape descriptor.
 *
 *  **Reference**
 *
 *  Aubry, M., Schlickewei U., Cremers D..
 *  The wave kernel signature: A quantum mechanical approach to shape analysis.
 *  The IEEE International Conference on Computer Vision (ICCV), 2011.
 */
template<typename Mesh>
class WKS
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

    /** Compute wks for all vertices.
     *
     *  @param wks Output wave kernel signatures
     *  @param escales Number of energy scales to use.
     *  @param emin The minimum energy scale. Setting emin >= emax will use the
     *  parameters described in the paper.
     *  @param emax The maximum energy scale. Setting emin >= emax will use the
     *  parameters described in the paper.
     *  @param sigma The variance of the log normal distribution. Setting sigma
     *  <= 0 will use the parameters described in the paper.
     */
    template<typename Derived>
    void compute(Eigen::ArrayBase<Derived>& wks,
                 unsigned escales = 100,
                 float emin = 0.0f,
                 float emax = -1.0f,
                 float sigma = -1.0f);

private:
    const Mesh* _mesh;
    Mat _phi2;
    Vec _loglambda;
    FT _lambda_max;
    FT _lambda_min;
};

/** @}*/
} // namespace Euclid

#include "src/WKS.cpp"

#endif
