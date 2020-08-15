#include <cmath>
#include <Euclid/Geometry/Spectral.h>

namespace Euclid
{

template<typename DerivedA, typename DerivedB, typename T>
T biharmonic_distance(const Eigen::MatrixBase<DerivedA>& lambdas,
                      const Eigen::MatrixBase<DerivedB>& phis,
                      int x,
                      int y)
{
    T dist = 0.0;
    for (Eigen::Index i = 0; i < lambdas.rows(); ++i) {
        auto a = (phis(x, i) - (phis(y, i)));
        auto b = lambdas(i);
        dist += a * a / (b * b);
    }
    return std::sqrt(dist);
}

} // namespace Euclid
