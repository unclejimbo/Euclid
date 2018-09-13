#include <cmath>
#include <numeric>

namespace Euclid
{

template<typename DerivedA, typename DerivedB, typename T, typename>
T l1(const Eigen::ArrayBase<DerivedA>& d1, const Eigen::ArrayBase<DerivedB>& d2)
{
    return (d1 - d2).abs().sum();
}

template<typename DerivedA, typename DerivedB, typename T, typename>
T l2(const Eigen::ArrayBase<DerivedA>& d1, const Eigen::ArrayBase<DerivedB>& d2)
{
    return std::sqrt((d1 - d2).square().sum());
}

template<typename DerivedA, typename DerivedB, typename T, typename>
T chi2(const Eigen::ArrayBase<DerivedA>& d1,
       const Eigen::ArrayBase<DerivedB>& d2)
{
    return 2 * ((d1 - d2).square() / (d1 + d2 + std::numeric_limits<T>::min()))
                   .sum();
}

template<typename DerivedA, typename DerivedB, typename T, typename>
T chi2_asym(const Eigen::ArrayBase<DerivedA>& d1,
            const Eigen::ArrayBase<DerivedB>& d2)
{
    return ((d1 - d2).square() / (d1 + std::numeric_limits<T>::min())).sum();
}

} // namespace Euclid
