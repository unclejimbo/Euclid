/** Obj I/O.
 *
 *  Obj is a geometry file format developed by wavefront, capable of storing
 *  vertex positions, normals, and texcoordinates.
 *
 *  There are several kinds of keywords specified in an obj file format.
 *  However, we'll restrict ourselves to the face element and ignore the others
 *  including groupings and materials.
 *
 *  @defgroup PkgObjIO Obj I/O
 *  @ingroup PkgIO
 */
#pragma once

#include <string>
#include <vector>

namespace Euclid
{
/** @{*/

/** Read Obj file.
 *
 *  #### Note
 *  Note that the obj file format is primarily designed for rendering and the
 *  vertex properties may not refer to true geometric quantities. For example,
 *  the vertex of a cube may have one position but three normals corresponding
 *  to three faces. Thus the output properties may have different sizes.
 */
template<typename FT>
void read_obj(const std::string& filename,
              std::vector<FT>& positions,
              std::vector<FT>* texcoords = nullptr,
              std::vector<FT>* normals = nullptr);

/** Read Obj file.
 *
 *  #### Note
 *  Note that the obj file format is primarily designed for rendering and the
 *  vertex properties may not refer to true geometric quantities. For example,
 *  the vertex of a cube may have one position but three normals corresponding
 *  to three faces. Thus the output properties may have different sizes, so as
 *  the indices.
 */
template<int N, typename FT, typename IT>
void read_obj(const std::string& filename,
              std::vector<FT>& positions,
              std::vector<IT>& pindices,
              std::vector<FT>* texcoords = nullptr,
              std::vector<IT>* tindices = nullptr,
              std::vector<FT>* normals = nullptr,
              std::vector<IT>* nindices = nullptr);

/** Write Obj file.
 *
 */
template<typename FT>
void write_obj(const std::string& filename,
               const std::vector<FT>& positions,
               const std::vector<FT>* texcoords = nullptr,
               const std::vector<FT>* normals = nullptr);

/** Write Obj file.
 *
 */
template<int N, typename FT, typename IT>
void write_obj(const std::string& filename,
               const std::vector<FT>& positions,
               const std::vector<IT>& pindices,
               const std::vector<FT>* texcoords = nullptr,
               const std::vector<IT>* tindices = nullptr,
               const std::vector<FT>* normals = nullptr,
               const std::vector<IT>* nindices = nullptr);

/** @}*/
} // namespace Euclid

#include "src/ObjIO.cpp"
