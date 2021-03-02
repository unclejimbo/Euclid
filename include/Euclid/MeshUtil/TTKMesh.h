/**Convert to and from ttk triangulation.
 *
 * This package provides functions to make ttk triangulation from raw buffers
 * and vice versa.
 *
 * Raw index array should be first packed into cells using make_cells(), then
 * fed into make_mesh(). Same for extraction.
 *
 * @defgroup PkgTTKMesh TTK Mesh
 * @ingroup PkgMeshUtil
 */
#pragma once

#include <vector>

namespace Euclid
{
/**@{*/

/**Create the cell array from index array.
 *
 */
template<typename IT>
void make_cells(std::vector<long long>& cells,
                const std::vector<IT>& indices,
                int n);

/**Extract the index array from cell array.
 *
 */
template<typename IT>
void extract_cells(std::vector<IT>& indices,
                   const std::vector<long long>& cells);

/**Create a ttk triangulation from positions and cells.
 *
 */
template<typename Triangulation>
void make_mesh(Triangulation& mesh,
               const std::vector<float>& positions,
               const std::vector<long long>& cells);

/**Create a ttk triangulation from positions and cells.
 *
 */
template<typename Triangulation>
void make_mesh(Triangulation& mesh,
               const std::vector<double>& positions,
               const std::vector<long long>& cells);

/**Extract positions and cells from the mesh.
 *
 */
template<typename Triangulation, typename FT>
void extract_mesh(const Triangulation& mesh,
                  std::vector<FT>& positions,
                  std::vector<long long>& cells);

/**@}*/
} // namespace Euclid

#include "src/TTKMesh.cpp"
