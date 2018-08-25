/** Serialization.
 *
 *  This package provides convenient functions to serialize common data
 *  structures used in Euclid into binary file format. The serialization is
 *  powered by cereal. If you wish to use cereal directly, i.e. serializing into
 *  an xml file or serializing values contained in a stl container, the type
 *  supported in this module can be directly used by an Archive since
 *  serialization functions of these types are already defined within this
 *  module (not shown in the api documentation though).
 *  @defgroup PkgSerialize Serialize
 *  @ingroup PkgUtil
 */
#pragma once

#include <string>
#include <vector>
#include <Eigen/Core>

namespace Euclid
{
/**@{*/

/** Serialize an Eigen dense matrix/array to a binary file.
 *
 */
template<typename Derived>
void serialize(const std::string& filename,
               const Eigen::PlainObjectBase<Derived>& mat);

/** Deserialize an Eigen dense matrix/array from a binary file.
 *
 */
template<typename Derived>
void deserialize(const std::string& filename,
                 Eigen::PlainObjectBase<Derived>& mat);

/**@}*/
} // namespace Euclid

#include "src/Serialize.cpp"
