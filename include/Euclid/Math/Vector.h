/** Vector utilities.
 *
 *  This package contains convenient functions to work with
 *  vectors of CGAL and Eigen.
 *  @defgroup PkgVector Vector
 *  @ingroup PkgMath
 */
#pragma once

#include <CGAL/Point_3.h>
#include <CGAL/Vector_3.h>
#include <Eigen/Core>

namespace Euclid
{
/** @{*/

/** Length of a vector.
 *
 */
template<typename Kernel>
typename Kernel::FT length(const CGAL::Vector_3<Kernel>& vec);

/** Normalize a vector.
 *
 */
template<typename Kernel>
void normalize(CGAL::Vector_3<Kernel>& vec);

/** Return a normalized version of the input vector.
 *
 */
template<typename Kernel>
CGAL::Vector_3<Kernel> normalized(const CGAL::Vector_3<Kernel>& vec);

/** Area of the triangle.
 *
 */
template<typename Kernel>
typename Kernel::FT area(const CGAL::Point_3<Kernel>& p1,
                         const CGAL::Point_3<Kernel>& p2,
                         const CGAL::Point_3<Kernel>& p3);

/** Sine of the angle between two vectors.
 *
 */
template<typename Kernel>
typename Kernel::FT sine(const CGAL::Vector_3<Kernel>& v1,
                         const CGAL::Vector_3<Kernel>& v2);

/** Sine of the angle between (p1-p2) and (p3-p2).
 *
 */
template<typename Kernel>
typename Kernel::FT sine(const CGAL::Point_3<Kernel>& p1,
                         const CGAL::Point_3<Kernel>& p2,
                         const CGAL::Point_3<Kernel>& p3);

/** Cosine of the angle between two vectors.
 *
 */
template<typename Kernel>
typename Kernel::FT cosine(const CGAL::Vector_3<Kernel>& v1,
                           const CGAL::Vector_3<Kernel>& v2);

/** Cosine of the angle between (p1-p2) and (p3-p2).
 *
 */
template<typename Kernel>
typename Kernel::FT cosine(const CGAL::Point_3<Kernel>& p1,
                           const CGAL::Point_3<Kernel>& p2,
                           const CGAL::Point_3<Kernel>& p3);

/** Tangent of the angle between two vectors.
 *
 */
template<typename Kernel>
typename Kernel::FT tangent(const CGAL::Vector_3<Kernel>& v1,
                            const CGAL::Vector_3<Kernel>& v2);

/** Tangent of the angle between (p1-p2) and (p3-p2).
 *
 */
template<typename Kernel>
typename Kernel::FT tangent(const CGAL::Point_3<Kernel>& p1,
                            const CGAL::Point_3<Kernel>& p2,
                            const CGAL::Point_3<Kernel>& p3);

/** Cotangent of the angle between two vectors.
 *
 */
template<typename Kernel>
typename Kernel::FT cotangent(const CGAL::Vector_3<Kernel>& v1,
                              const CGAL::Vector_3<Kernel>& v2);

/** Cotangent of the angle between (p1-p2) and (p3-p2).
 *
 */
template<typename Kernel>
typename Kernel::FT cotangent(const CGAL::Point_3<Kernel>& p1,
                              const CGAL::Point_3<Kernel>& p2,
                              const CGAL::Point_3<Kernel>& p3);

/** Convert a CGAL vector-like to Eigen.
 *
 * @tparam CGALVec CGAL::Vector_3, CGAL::Point_3 and the like.
 */
template<typename FT, typename CGALVec>
Eigen::Matrix<FT, 3, 1> cgal_to_eigen(const CGALVec& vec);

/** Convert an Eigen vector to CGAL.
 *
 * @tparam CGALVec CGAL::Vector_3, CGAL_Point_3 and the like.
 */
template<typename CGALVec, typename FT>
CGALVec eigen_to_cgal(const Eigen::Matrix<FT, 3, 1>& vec);

/** @}*/
} // namespace Euclid

#include "src/Vector.cpp"
