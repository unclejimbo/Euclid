/** Numeric utilities.
 *
 *  This package provides functions for more robust floating point comparisons
 *  than using operator ==, !=, <, > directly.
 *
 *  #### Warnings
 *  Choose epsilon according to your value range, otherwise you'll get undesired
 *  outcomes.
 *
 *  @defgroup PkgNumeric Numeric
 *  @ingroup PkgMath
 */
#pragma once

#include <limits>
#include <type_traits>

namespace Euclid
{
/** @{*/

/** Two floating points are almost equal.
 *
 *  This is the most versatile equality check to use. It returns true when
 *  either a relative or absolute error bound is satified, with the assumption
 *  that the tolerance for both kinds of errors are the same.
 *
 *  @param a
 *  @param b
 *  @param epsilon Default value is sufficient for common cases. However, when
 *  comparing very closed values less than one, try to increase epsilon because
 *  this senario is equivalent to using eq_abs_err().
 */
template<typename T,
         typename = std::enable_if_t<std::is_floating_point<T>::value>>
bool eq_almost(T a, T b, T epsilon = std::numeric_limits<T>::epsilon());

/** Two floating points are almost equal.
 *
 *  This is the most versatile equality check to use. It returns true when
 *  either a relative or absolute error bound is satified.
 *
 *  @param a
 *  @param b
 *  @param epsilon Default value is sufficient for most cases. However, when
 *  comparing very closed values less than one, try to increase epsilon because
 *  this senario is equivalent to using eq_abs_err().
 */
template<typename T,
         typename = std::enable_if_t<std::is_floating_point<T>::value>>
bool eq_almost(T a, T b, T eps_rel, T eps_abs);

/** Two floating points are equal under a relative error bound.
 *
 *  Faster than eq_almost() if not comparing zeros.
 *
 *  @param a
 *  @param b
 *  @param epsilon Default value is sufficient for most cases. However, when
 *  comparing very small values, epsilon might be very high.
 */
template<typename T,
         typename = std::enable_if_t<std::is_floating_point<T>::value>>
bool eq_rel_err(T a, T b, T epsilon = std::numeric_limits<T>::epsilon());

/** Two floating points are equal under an absolute error bound.
 *
 *  Only meaningful when you know the bounds of a, b and choose epsilon wisely.
 *
 *  @param a
 *  @param b
 *  @param epsilon Default value is only useful for comparing to zero. You need
 *  to specify this value in order to be meaningful for your case.
 */
template<typename T,
         typename = std::enable_if_t<std::is_floating_point<T>::value>>
bool eq_abs_err(T a, T b, T epsilon = std::numeric_limits<T>::epsilon());

/** Two floating points are equal under an ulp error bound.
 *
 *  Units in the Last Place distance measures how much two floating points
 *  differs in their integer representations. It's also more accurate when
 *  comparing to an exact value, and robust in common cases.
 *
 *  @param a
 *  @param b
 *  @param max_ulps The maximum ulp difference threshold, where 4 is enough
 *  for most cases.
 */
template<typename T,
         typename = std::enable_if_t<std::is_floating_point<T>::value>>
bool eq_ulp(T a, T b, std::size_t max_ulps = 4);

/** Safer version of a < b.
 *
 *  Returns true even if a is greater than b by a small margin, due to numeric
 *  error bounded by epsilon.
 *
 *  @param a
 *  @param b
 *  @param epsilon
 */
template<typename T,
         typename = std::enable_if_t<std::is_floating_point<T>::value>>
bool less_safe(T a, T b, T epsilon = std::numeric_limits<T>::epsilon());

/** Safer version of a > b.
 *
 *  Returns true even if a is less than b by a small margin, due to numeric
 *  error bounded by epsilon.
 *
 *  @param a
 *  @param b
 *  @param epsilon
 *
 */
template<typename T,
         typename = std::enable_if_t<std::is_floating_point<T>::value>>
bool greater_safe(T a, T b, T epsilon = std::numeric_limits<T>::epsilon());

/** @}*/
} // namespace Euclid

#include "src/Numeric.cpp"
