#include <cmath>
#include <algorithm>

namespace Euclid
{

namespace _impl
{

#include "AlmostEquals.h"

} // namespace _impl

template<typename T, typename>
inline bool eq_almost(T a, T b, T epsilon)
{
    return std::abs(a - b) <=
           std::max({ static_cast<T>(1.0), std::abs(a), std::abs(b) }) *
               epsilon;
}

template<typename T, typename>
inline bool eq_almost(T a, T b, T eps_rel, T eps_abs)
{
    return std::abs(a - b) <=
           std::max(
               { eps_abs, std::max({ std::abs(a), std::abs(b) }) * eps_rel });
}

template<typename T, typename>
inline bool eq_rel_err(T a, T b, T epsilon)
{
    return std::abs(a - b) <= std::max(a, b) * epsilon;
}

template<typename T, typename>
inline bool eq_abs_err(T a, T b, T epsilon)
{
    return std::abs(a - b) < epsilon;
}

template<typename T, typename>
inline bool eq_ulp(T a, T b, std::size_t epsilon)
{
    _impl::FloatingPoint<T> x(a);
    _impl::FloatingPoint<T> y(b);
    x.kMaxUlps = epsilon;
    y.kMaxUlps = epsilon;
    return x.AlmostEquals(y);
}

template<typename T, typename>
inline bool less_safe(T a, T b, T epsilon)
{
    return a - b <=
           std::max({ static_cast<T>(1.0), std::abs(a), std::abs(b) }) *
               epsilon;
}

template<typename T, typename>
inline bool greater_safe(T a, T b, T epsilon)
{
    return b - a <=
           std::max({ static_cast<T>(1.0), std::abs(a), std::abs(b) }) *
               epsilon;
}

} // Euclid
