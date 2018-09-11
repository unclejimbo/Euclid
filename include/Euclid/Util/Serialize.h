/** Serialization.
 *
 *  This package provides convenient functions to serialize/deserialize common
 *  data structures used in Euclid into binary file format. The serialization is
 *  powered by cereal. The following types are supported, in addition to all
 *  the other types already available in cereal itself,
 *
 *  - Eigen::Matrix
 *  - Eigen::Array
 *  - Eigen::SparseMatrix
 *
 *  If you wish to use cereal directly, i.e. serializing into an xml file, the
 *  types supported in this module can be directly used by an Archive since
 *  proper save and load functions of these types have already been defined
 *  within this module (not shown in the api though).
 *
 *  @defgroup PkgSerialize Serialize
 *  @ingroup PkgUtil
 */
#pragma once

#include <string>
#include <vector>
#include <Eigen/Core>
#include <Eigen/SparseCore>

namespace Euclid
{
/**@{*/

/** Serialize data into binary file.
 *
 */
template<typename... T>
void serialize(const std::string& filename, const T&... data);

/** Deserialize data from binary file.
 *
 */
template<typename... T>
void deserialize(const std::string& filename, T&... data);

/**@}*/
} // namespace Euclid

#include "src/Serialize.cpp"
