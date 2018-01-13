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
 *  Read point set or triangle mesh from a off file.
 *  Note that index count can vary per face, but it's assumed
 *  to be 3, namely a triangle, by this reader.
 *  Also note that off file format can also store edge indices,
 *  which is often 0 and useless, thus omitted by this reader.
 *
 *  @param[in] file_name Off file name.
 *  @param[in] positions The buffer to store point positions.
 *  @param[in] indices The buffer to store face indices.
 */
template<typename T1, typename T2>
void read_off(const std::string& file_name,
    std::vector<T1>& positions,
    std::vector<T2>* indices = nullptr);


/** Write off file.
 *
 */
template<typename T1, typename T2>
void write_off(const std::string& file_name,
    const std::vector<T1>& positions,
    const std::vector<T2>* indices = nullptr);

/** @}*/
} // namespace Euclid

#include "src/OffIO.cpp"
