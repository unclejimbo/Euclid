/**Discrete exterior calculus.
 *
 * This package provides common DEC operators.
 *
 * **Reference**
 *
 * [1] Elcott, S., Schroder, P.
 * Building Your Own DEC at Home.
 *
 * [2] Gu, X., Yau, S.T.
 * Global Conformal Surface Parameterization.
 *
 * @defgroup PkgDEC Discrete Exterior Calculus
 * @ingroup PkgGeometry
 */
#pragma once

#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <CGAL/boost/graph/properties.h>

namespace Euclid
{
/**@{*/

/**Give a consistent orientation for halfedges.
 *
 * @param mesh Target triangle mesh.
 * @param h Target halfedge.
 */
template<typename Mesh>
int halfedge_orientation(
    const Mesh& mesh,
    typename boost::graph_traits<const Mesh>::halfedge_descriptor h);

/**Exterior derivative operator of 0-form.
 *
 * Exterior derivative for 0-form f is @f$D0 * f@f$.
 *
 * @param mesh Target triangle mesh.
 * @param op An E x V matrix acting as the exterior derivative operator.
 */
template<typename Mesh, typename T>
void d0(const Mesh& mesh, Eigen::SparseMatrix<T>& op);

/**Exterior derivative operator of 1-form.
 *
 * Exterior derivative for 1-form w is @f$D1 * w@f$.
 *
 * @param mesh Target triangle mesh.
 * @param op An F x E matrix acting as the exterior derivative operator.
 */
template<typename Mesh, typename T>
void d1(const Mesh& mesh, Eigen::SparseMatrix<T>& op);

/**Hodge star operator of 1-form.
 *
 * Equivalent to the cotangent matrix.
 *
 * @param mesh Target triangle mesh.
 * @param op An E x E matrix acting as the hodge star operator.
 */
template<typename Mesh, typename T>
void star1(const Mesh& mesh, Eigen::SparseMatrix<T>& op);

/**The primal x primal wedge product.
 *
 * The wedge product is not linear so it cannot be represented as a matrix.
 * Instead, we take the 1-forms as input and return the result.
 *
 * @param mesh Target triangle mesh.
 * @param w1 1-form.
 * @param w2 1-form.
 * @param result An F x 1 matrix storing the result of wedge product.
 */
template<typename Mesh, typename DerivedA, typename DerivedB, typename DerivedC>
void wedge1(const Mesh& mesh,
            const Eigen::MatrixBase<DerivedA>& w1,
            const Eigen::MatrixBase<DerivedB>& w2,
            Eigen::MatrixBase<DerivedC>& result);

/**The primal x dual wedge product.
 *
 * The wedge product is not linear so it cannot be represented as a matrix.
 * Instead, we take the 1-forms as input and return the result.
 *
 * @param mesh Target triangle mesh.
 * @param w1 1-form.
 * @param w2 1-form.
 * @param result An F x 1 matrix storing the result of wedge product.
 */
template<typename Mesh, typename DerivedA, typename DerivedB, typename DerivedC>
void star_wedge1(const Mesh& mesh,
                 const Eigen::MatrixBase<DerivedA>& w1,
                 const Eigen::MatrixBase<DerivedB>& w2,
                 Eigen::MatrixBase<DerivedC>& result);

/**@}*/
} // namespace Euclid

#include "src/DEC.cpp"
