/** Off I/O.
 *
 *  Off is a simple geometry file format storing positions, indices, and
 *  optionally colors.
 *  @defgroup PkgOffIO Off I/O
 *  @ingroup PkgIO
 */
#pragma once

#include <string>
#include <type_traits>
#include <vector>

namespace Euclid
{
/** @{*/

/** Read off file.
 *
 *  Off file can store positions, indices, vertex colors and face colors. Note
 *  that index count can vary per face, but it's asumed to be fixed for all
 *  faces by this reader. Also note that off file format can store edge indices,
 *  which is often 0, thus omitted by this reader.
 *
 *  @tparam N Number of vertices per face.
 *  @tparam FT Type of floating point value.
 *  @tparam IT Type of index value.
 *  @tparam CT Type of color value.
 *
 *  @param filename Input file name.
 *  @param positions Vertex positions.
 *  @param vcolors Vertex colors, including r, g, b, a. Use nullptr if you don't
 *  want to read them.
 *  @param findices Face indices. Use nullptr if you don't want to read them.
 *  @param fcolors Face colors, including r, g, b, a. Use nullptr if you don't
 *  want to read them.
 *
 *  **Note**
 *
 *  Proper overloading functions are implemented so that it's not necessary to
 *  provide template parameters even if nullptr is used.
 */
template<int N,
         typename FT,
         typename IT,
         typename CT,
         typename Interface = std::enable_if_t<false>>
void read_off(const std::string& filename,
              std::vector<FT>& positions,
              std::vector<CT>* vcolors,
              std::vector<IT>* findices,
              std::vector<CT>* fcolors);

/** Write off file.
 *
 *  Off file can store positions, indices, vertex colors and face colors. Note
 *  that index count can vary per face, but it's asumed to be fixed for all
 *  faces by this reader. Also note that off file format can store edge indices,
 *  which is often 0, thus omitted by this reader.
 *
 *  @tparam N Number of vertices per face.
 *  @tparam FT Type of floating point value.
 *  @tparam IT Type of index value.
 *  @tparam CT Type of color value.
 *
 *  @param filename Input file name.
 *  @param positions Vertex positions.
 *  @param vcolors Vertex colors, including r, g, b, a. Use nullptr if you don't
 *  want to write them.
 *  @param findices Face indices. Use nullptr if you don't want to write them.
 *  @param fcolors Face colors, including r, g, b, a. Use nullptr if you don't
 *  want to write them.
 *
 *  **Note**
 *
 *  Proper overloading functions are implemented so that it's not necessary to
 *  provide template parameters even if nullptr is used.
 */
template<int N,
         typename FT,
         typename IT,
         typename CT,
         typename Interface = std::enable_if_t<false>>
void write_off(const std::string& filename,
               const std::vector<FT>& positions,
               const std::vector<CT>* vcolors,
               const std::vector<IT>* findices,
               const std::vector<CT>* fcolors);

/** @}*/
} // namespace Euclid

#include "src/OffIO.cpp"
