#include <cmath>
#include <stdexcept>

namespace Euclid
{

template<typename Derived, typename T>
T l1(const Eigen::ArrayBase<Derived>& d1, const Eigen::ArrayBase<Derived>& d2)
{
    return (d1 - d2).abs().sum();
}

template<typename Derived, typename T>
T l2(const Eigen::ArrayBase<Derived>& d1, const Eigen::ArrayBase<Derived>& d2)
{
    return std::sqrt((d1 - d2).square().sum());
}

template<typename Derived, typename T>
T chi2(const Eigen::ArrayBase<Derived>& d1, const Eigen::ArrayBase<Derived>& d2)
{
    return 2 * ((d1 - d2).square() / (d1 + d2)).sum();
}

template<typename Derived, typename T>
T chi2_asym(const Eigen::ArrayBase<Derived>& d1,
            const Eigen::ArrayBase<Derived>& d2)
{
    return ((d1 - d2).square() / d1).sum();
}

} // namespace Euclid
