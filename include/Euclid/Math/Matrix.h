/** Matrix manipulation.
 *
 *  This package contains utilities to work with dense and sparse matrices in
 *  Eigen.
 *  @defgroup PkgMatrix Matrix
 *  @ingroup PkgMath
 */
#pragma once

#include <Eigen/SparseCore>

namespace Euclid
{
/** @{*/

/** Apply a function for each non-zero element in a sparse matrix.
 *
 */
template<typename Scalar,
         int Options,
         typename StorageIndex,
         typename UnaryFunction>
void for_each(Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat,
              UnaryFunction f);

/** Apply a function for each non-zero element in a sparse matrix and
 *  store the result to another.
 *
 */
template<typename Scalar,
         int Options,
         typename StorageIndex,
         typename UnaryFunction>
void transform(const Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat_in,
               Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat_out,
               UnaryFunction f);

/** @}*/
} // namespace Euclid

#include "src/Matrix.cpp"
