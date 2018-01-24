/** Off I/O.
 *
 *  Off is simple geometry file format storing positions,
 *  and indices of edges and faces.
 *  @defgroup PkgOffIO Off I/O
 *  @ingroup PkgIO
 */
#pragma once
#include <string>
#include <vector>

namespace Euclid
{
/** @{*/

/** Read off file.
 *
 *  Read positions from an off file. Omit indices in the file.
 */
template<typename T>
void read_off(const std::string& file_name, std::vector<T>& positions);

/** Read off file.
 *
 *  Read positions and indices from an off file.
 *  Note that index count can vary per face, but it's assumed
 *  to be fixed for all faces by this reader.
 *  Also note that off file format can also store edge indices,
 *  which is often 0 and useless, thus omitted by this reader.
 */
template<int N, typename T1, typename T2>
void read_off(const std::string& file_name,
              std::vector<T1>& positions,
              std::vector<T2>& indices);

/** Write off file.
 *
 *  Write point positions to an off file.
 */
template<typename T>
void write_off(const std::string& file_name, const std::vector<T>& positions);

/** Write off file.
 *
 *  Write point positions and indices to an off file.
 */
template<int N, typename T1, typename T2>
void write_off(const std::string& file_name,
               const std::vector<T1>& positions,
               const std::vector<T2>& indices = nullptr);

/** @}*/
} // namespace Euclid

#include "src/OffIO.cpp"
