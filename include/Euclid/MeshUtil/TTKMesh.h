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
#include <ttk/base/DataTypes.h>

namespace Euclid
{
/**@{*/

#ifdef TTK_CELL_ARRAY_NEW

/**Create a ttk triangulation from positions and indices.
 *
 * Have to keep the internal buffers since they are not stored in mesh
 * Using new (ttk>=0.9) layout.
 */
template<typename Triangulation, typename IT>
void make_mesh(Triangulation& mesh,
               std::vector<ttk::LongSimplexId>& connectivity,
               std::vector<ttk::LongSimplexId>& offset,
               const std::vector<float>& positions,
               const std::vector<IT>& indices);

/**Create a ttk triangulation from positions and indices.
 *
 * Have to keep the internal buffers since they are not stored in mesh
 * Using new (ttk>=0.9) layout.
 */
template<typename Triangulation, typename IT>
void make_mesh(Triangulation& mesh,
               std::vector<ttk::LongSimplexId>& connectivity,
               std::vector<ttk::LongSimplexId>& offset,
               const std::vector<double>& positions,
               const std::vector<IT>& indices);

#else

/**Create a ttk triangulation from positions and indices.
 *
 * Have to keep the internal buffers since they are not stored in mesh
 * Using legacy (ttk<=0.8) layout.
 */
template<typename Triangulation, typename IT>
void make_mesh(Triangulation& mesh,
               std::vector<ttk::LongSimplexId>& cells,
               const std::vector<float>& positions,
               const std::vector<IT>& indices);

/**Create a ttk triangulation from positions and indices.
 *
 * Have to keep the internal buffers since they are not stored in mesh
 * Using legacy (ttk<=0.8) layout.
 */
template<typename Triangulation, typename IT>
void make_mesh(Triangulation& mesh,
               std::vector<ttk::LongSimplexId>& cells,
               const std::vector<double>& positions,
               const std::vector<IT>& indices);

#endif

/**Extract positions and indices from the ttk mesh.
 *
 */
template<typename Triangulation, typename FT, typename IT>
void extract_mesh(const Triangulation& mesh,
                  std::vector<FT>& positions,
                  std::vector<IT>& indices);

/**@}*/
} // namespace Euclid

#include "src/TTKMesh.cpp"
