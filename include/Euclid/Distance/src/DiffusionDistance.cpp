#include <cmath>

namespace Euclid
{

template<typename DerivedA, typename DerivedB, typename T>
T diffusion_distance(const Eigen::MatrixBase<DerivedA>& lambdas,
                     const Eigen::MatrixBase<DerivedB>& phis,
                     int x,
                     int y,
                     double t)
{
    T dist = 0.0;
    for (Eigen::Index i = 0; i < lambdas.rows(); ++i) {
        auto a = (phis(x, i) - (phis(y, i)));
        auto b = lambdas(i);
        dist += a * a * std::exp(-2.0 * t * b);
    }
    return std::sqrt(dist);
}

} // namespace Euclid
