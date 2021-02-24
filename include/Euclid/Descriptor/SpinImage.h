#pragma once

#include <vector>
#include <Eigen/Core>
#include <Euclid/MeshUtil/MeshDefs.h>
#include <Euclid/Util/Memory.h>

namespace Euclid
{
/**@{ @ingroup PkgDescriptor*/

/** The spin image descriptor.
 *
 *  Spin image is an image-based local shape descriptor. For every point
 *  on a mesh, an image is generated by projecting points onto the image
 *  plane within a local support.
 *
 *  **Reference**
 *
 *  Johnson A E, Hebert M.
 *  Using spin images for efficient object recognition in cluttered 3D
 * 	scenes[J].
 *  IEEE Transactions on pattern analysis and machine intelligence,
 *  1999, 21(5): 433-449.
 */
template<typename Mesh>
class SpinImage
{
public:
    using Vector_3 = Vector_3_t<Mesh>;
    using FT = FT_t<Mesh>;

public:
    /** Build up the necessary computational components.
     *
     *  @param mesh The target mesh.
     *  @param vnormals The precomputed vertex normals, default to nullptr and
     *  it'll be computed internally.
     *  @param resolution The mesh resolution, i.e. avg. edge length. Default to
     *  0.0 and it'll be computed internally.
     */
    void build(const Mesh& mesh,
               const std::vector<Vector_3>* vnormals = nullptr,
               FT resolution = 0.0);

    /** Compute the spin image descriptor for all vertices.
     *
     *  @param spin_img The output spin image for v.
     *  @param bin_scale Multiple of the mesh resolution, default to 1.0 which
     *  is equavalent to use the mesh resolution as bin size.
     *  @param image_width Number of rows and columns for the image, default to
     *  16. bin_size * image_width equals to the actual image size in mesh
     *  coordinates.
     *  @param support_angle Maximum support angle in degrees, default to 90.0
     *  thus supporting vertices pointing in the same direction as v.
     */
    template<typename Derived>
    void compute(Eigen::ArrayBase<Derived>& spin_img,
                 float bin_scale = 1.0f,
                 int image_width = 16,
                 float support_angle = 90.0f);

public:
    /** The mesh being processed.
     *
     */
    const Mesh* mesh = nullptr;

    /** The vertex normals.
     *
     */
    ProPtr<const std::vector<Vector_3>> vnormals = nullptr;

    /** The mesh resolution.
     *
     */
    FT resolution = 0.0;
};

/** @}*/
} // namespace Euclid

#include "src/SpinImage.cpp"
