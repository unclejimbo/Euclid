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
    FT sin = 0.0;
    auto l1 = length(v1);
    auto l2 = length(v2);
    if (!Euclid::eq_abs_err(l1,
                            static_cast<FT>(0.0),
                            std::numeric_limits<FT>::epsilon() * 10) &&
        !Euclid::eq_abs_err(l2,
                            static_cast<FT>(0.0),
                            std::numeric_limits<FT>::epsilon() * 10)) {
        auto outer_prod = CGAL::cross_product(v1, v2);
        sin = std::min(length(outer_prod) / (l1 * l2), static_cast<FT>(1.0));
    }
    return sin;
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
    FT cos = 1.0;
    auto l1 = length(v1);
    auto l2 = length(v2);
    if (!Euclid::eq_abs_err(l1,
                            static_cast<FT>(0.0),
                            std::numeric_limits<FT>::epsilon() * 10) &&
        !Euclid::eq_abs_err(l2,
                            static_cast<FT>(0.0),
                            std::numeric_limits<FT>::epsilon() * 10)) {
        cos = v1 * v2 / (l1 * l2);
        cos = std::clamp(cos, static_cast<FT>(-1.0), static_cast<FT>(1.0));
    }
    return cos;
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
    FT tan = 0.0;
    auto l1 = v1.squared_length();
    auto l2 = v2.squared_length();
    if (!Euclid::eq_abs_err(l1,
                            static_cast<FT>(0.0),
                            std::numeric_limits<FT>::epsilon() * 10) &&
        !Euclid::eq_abs_err(l2,
                            static_cast<FT>(0.0),
                            std::numeric_limits<FT>::epsilon() * 10)) {
        auto inner_prod = v1 * v2;
        auto squared_cos = inner_prod * inner_prod / (l1 * l2);
        squared_cos = std::min(squared_cos, static_cast<FT>(1.0));
        tan =
            Euclid::eq_abs_err(squared_cos,
                               static_cast<FT>(0.0),
                               std::numeric_limits<FT>::epsilon() * 10)
                ? NAN
                : std::sqrt((static_cast<FT>(1.0) - squared_cos) / squared_cos);
        tan = inner_prod < 0.0 ? -tan : tan;
    }
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
    FT cot = NAN;
    auto l1 = v1.squared_length();
    auto l2 = v2.squared_length();
    if (!Euclid::eq_abs_err(l1,
                            static_cast<FT>(0.0),
                            std::numeric_limits<FT>::epsilon() * 10) &&
        !Euclid::eq_abs_err(l2,
                            static_cast<FT>(0.0),
                            std::numeric_limits<FT>::epsilon() * 10)) {
        auto inner_prod = v1 * v2;
        auto squared_cos = inner_prod * inner_prod / (l1 * l2);
        squared_cos = std::min(squared_cos, static_cast<FT>(1.0));
        cot =
            Euclid::eq_almost(squared_cos, static_cast<FT>(1.0))
                ? NAN
                : std::sqrt(squared_cos / (static_cast<FT>(1.0) - squared_cos));
        cot = inner_prod < 0.0 ? -cot : cot;
    }
    return cot;
}

template<typename Kernel>
typename Kernel::FT cotangent(const CGAL::Point_3<Kernel>& p1,
                              const CGAL::Point_3<Kernel>& p2,
                              const CGAL::Point_3<Kernel>& p3)
{
    return cotangent(p1 - p2, p3 - p2);
}

template<typename FT, typename CGALVec>
inline Eigen::Matrix<FT, 3, 1> cgal_to_eigen(const CGALVec& vec)
{
    return Eigen::Matrix<FT, 3, 1>(vec.x(), vec.y(), vec.z());
}

template<typename CGALVec, typename FT>
inline CGALVec eigen_to_cgal(const Eigen::Matrix<FT, 3, 1>& vec)
{
    return CGALVec(vec(0), vec(1), vec(2));
}

} // namespace Euclid
