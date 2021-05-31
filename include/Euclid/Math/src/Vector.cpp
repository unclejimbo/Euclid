#include <algorithm>
#include <cmath>

#include <CGAL/Kernel/global_functions.h>
#include <Euclid/Math/Numeric.h>
#include <Euclid/Util/Assert.h>

namespace Euclid
{

template<typename Kernel>
inline typename Kernel::FT length(const CGAL::Vector_3<Kernel>& vec)
{
    return std::sqrt(vec.squared_length());
}

template<typename Kernel>
inline void normalize(CGAL::Vector_3<Kernel>& vec)
{
    using FT = typename Kernel::FT;
    auto l = Euclid::length(vec);
    if (!Euclid::eq_abs_err(
            l, static_cast<FT>(0.0), std::numeric_limits<FT>::epsilon() * 10)) {
        vec /= l;
    }
    else {
        EWARNING("Can't normalize zero vector.");
    }
}

template<typename Kernel>
inline CGAL::Vector_3<Kernel> normalized(const CGAL::Vector_3<Kernel>& vec)
{
    using FT = typename Kernel::FT;
    auto l = Euclid::length(vec);
    auto rtn = vec;
    if (!Euclid::eq_abs_err(
            l, static_cast<FT>(0.0), std::numeric_limits<FT>::epsilon() * 10)) {
        rtn /= l;
    }
    else {
        EWARNING("Can't normalize zero vector.");
    }
    return rtn;
}

template<typename Kernel>
inline typename Kernel::FT area(const CGAL::Point_3<Kernel>& p1,
                                const CGAL::Point_3<Kernel>& p2,
                                const CGAL::Point_3<Kernel>& p3)
{
    if (CGAL::collinear(p1, p2, p3)) {
        EWARNING("Input points are collinear, resulting in zero area");
        return 0.0;
    }
    else {
        return std::sqrt(
                   CGAL::cross_product(p3 - p2, p1 - p2).squared_length()) *
               static_cast<typename Kernel::FT>(0.5);
    }
}

template<typename Kernel>
typename Kernel::FT sine(const CGAL::Vector_3<Kernel>& v1,
                         const CGAL::Vector_3<Kernel>& v2)
{
    using FT = typename Kernel::FT;
    auto l1 = length(v1);
    auto l2 = length(v2);
    auto outer_prod = CGAL::cross_product(v1, v2);
    auto sin = length(outer_prod) / (l1 * l2 + static_cast<FT>(1e-10));
    return std::clamp(sin, static_cast<FT>(-1.0), static_cast<FT>(1.0));
}

template<typename Kernel>
typename Kernel::FT sine(const CGAL::Point_3<Kernel>& p1,
                         const CGAL::Point_3<Kernel>& p2,
                         const CGAL::Point_3<Kernel>& p3)
{
    return sine(p1 - p2, p3 - p2);
}

template<typename Kernel>
typename Kernel::FT cosine(const CGAL::Vector_3<Kernel>& v1,
                           const CGAL::Vector_3<Kernel>& v2)
{
    using FT = typename Kernel::FT;
    auto l1 = length(v1);
    auto l2 = length(v2);
    auto cos = v1 * v2 / (l1 * l2 + static_cast<FT>(1e-10));
    return std::clamp(cos, static_cast<FT>(-1.0), static_cast<FT>(1.0));
}

template<typename Kernel>
typename Kernel::FT cosine(const CGAL::Point_3<Kernel>& p1,
                           const CGAL::Point_3<Kernel>& p2,
                           const CGAL::Point_3<Kernel>& p3)
{
    return cosine(p1 - p2, p3 - p2);
}

template<typename Kernel>
typename Kernel::FT tangent(const CGAL::Vector_3<Kernel>& v1,
                            const CGAL::Vector_3<Kernel>& v2)
{
    using FT = typename Kernel::FT;
    auto tan = Euclid::length(CGAL::cross_product(v1, v2)) /
               ((v1 * v2) + static_cast<FT>(1e-10));
    return tan;
}

template<typename Kernel>
typename Kernel::FT tangent(const CGAL::Point_3<Kernel>& p1,
                            const CGAL::Point_3<Kernel>& p2,
                            const CGAL::Point_3<Kernel>& p3)
{
    return tangent(p1 - p2, p3 - p2);
}

template<typename Kernel>
typename Kernel::FT cotangent(const CGAL::Vector_3<Kernel>& v1,
                              const CGAL::Vector_3<Kernel>& v2)
{
    using FT = typename Kernel::FT;
    auto cot = (v1 * v2) / (Euclid::length(CGAL::cross_product(v1, v2)) +
                            static_cast<FT>(1e-10));
    return cot;
}

template<typename Kernel>
typename Kernel::FT cotangent(const CGAL::Point_3<Kernel>& p1,
                              const CGAL::Point_3<Kernel>& p2,
                              const CGAL::Point_3<Kernel>& p3)
{
    return cotangent(p1 - p2, p3 - p2);
}

template<typename CGALVec, typename Derived>
void cgal_to_eigen(const CGALVec& from, Eigen::PlainObjectBase<Derived>& to)
{
    to.derived().resize(3);
    to << from.x(), from.y(), from.z();
}

template<typename Derived, typename CGALVec>
void eigen_to_cgal(const Eigen::DenseBase<Derived>& from, CGALVec& to)
{
    to = CGALVec(from(0), from(1), from(2));
}

} // namespace Euclid
