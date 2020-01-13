#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <Euclid/Geometry/Spectral.h>

namespace Euclid
{

template<typename Mesh>
void HKS<Mesh>::build(const Mesh& mesh, unsigned k)
{
    _mesh = &mesh;
    auto n = spectrum(mesh, k, _emlambda, _phi2);
    _lambda_max = _emlambda(n - 1);
    _lambda_min = std::abs(_emlambda(1)); // abs fix numerical error
    _emlambda = (-_emlambda).array().exp().matrix().eval();
    _phi2 = _phi2.array().square().matrix().eval();
}

template<typename Mesh>
void HKS<Mesh>::build(const Mesh& mesh,
                      const Vec* eigenvalues,
                      const Mat* eigenfunctions)
{
    _mesh = &mesh;
    _lambda_max = eigenvalues->coeff(eigenvalues->size() - 1);
    _lambda_min = std::abs(eigenvalues->coeff(1)); // abs fix numerical error
    _emlambda = (-*eigenvalues).array().exp().matrix().eval();
    _phi2 = (*eigenfunctions).array().square().matrix().eval();
}

template<typename Mesh>
template<typename Derived>
void HKS<Mesh>::compute(Eigen::ArrayBase<Derived>& hks,
                        unsigned tscales,
                        float tmin,
                        float tmax)
{
    if (tmin > 0 && tmax > 0) {
        if (tmin >= tmax) {
            throw std::invalid_argument("tmin is larger than tmax.");
        }
    }
    else {
        auto c = static_cast<FT>(4.0 * std::log(10.0));
        tmin = c / _lambda_max;
        tmax = c / _lambda_min;
    }
    auto log_tmin = std::log(tmin);
    auto log_tmax = std::log(tmax);
    auto log_tstep = (log_tmax - log_tmin) / tscales;
    auto vimap = get(boost::vertex_index, *_mesh);
    auto nv = num_vertices(*_mesh);
    hks.derived().resize(tscales, nv);

    for (auto v : vertices(*_mesh)) {
        auto idx = get(vimap, v);
        for (size_t i = 0; i < tscales; ++i) {
            auto t = std::exp(log_tmin + log_tstep * i);
            auto hks_t = static_cast<FT>(0);
            for (int j = 0; j < _emlambda.size(); ++j) {
                hks_t += std::pow(_emlambda(j), t) * _phi2(idx, j);
            }
            hks(i, idx) = hks_t;
        }
    }
    hks.colwise() /= hks.rowwise().sum();
}

} // namespace Euclid
