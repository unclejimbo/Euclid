/**Convert to and from Eigen mesh.
 *
 * This package provides functions to make Eigen mesh from raw buffers and vice
 * versa.
 *
 * Matrix-based representation are used in libigl.
 *
 * @defgroup PkgEigenMesh Eigen Mesh
 * @ingroup PkgMeshUtil
 */
#pragma once

#include <vector>
#include <Eigen/Core>

namespace Euclid
{
/** @{*/

/** Create a matrix representation of mesh from positions and indices.
 *
 */
template<int N, typename DerivedV, typename DerivedF, typename FT, typename IT>
void make_mesh(Eigen::PlainObjectBase<DerivedV>& V,
               Eigen::PlainObjectBase<DerivedF>& F,
               const std::vector<FT>& positions,
               const std::vector<IT>& indices);

/** Extract positions and indices from mesh matrices.
 *
 */
template<int N, typename DerivedV, typename DerivedF, typename FT, typename IT>
void extract_mesh(const Eigen::MatrixBase<DerivedV>& V,
                  const Eigen::MatrixBase<DerivedF>& F,
                  std::vector<FT>& positions,
                  std::vector<IT>& indices);

/** Extract positions from position matrix.
 *
 *  This function is helpful when the mesh positions are changed but topology
 *  remains the same.
 */
template<typename DerivedV, typename FT>
void extract_mesh(const Eigen::MatrixBase<DerivedV>& V,
                  std::vector<FT>& positions);

/** @}*/
} // namespace Euclid

#include "src/EigenMesh.cpp"
