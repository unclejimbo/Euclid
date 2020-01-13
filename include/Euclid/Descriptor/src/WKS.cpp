#include <algorithm>
#include <cmath>
#include <stdexcept>

#include <Eigen/QR>
#include <Euclid/Geometry/Spectral.h>

namespace Euclid
{

template<typename Mesh>
void WKS<Mesh>::build(const Mesh& mesh, unsigned k)
{
    _mesh = &mesh;
    auto n = spectrum(mesh, k, _loglambda, _phi2);
    // abs fix numerical error
    _lambda_max = std::abs(_loglambda(n - 1));
    _lambda_min = std::abs(_loglambda(1));
    _loglambda = _loglambda.array().abs().log().matrix().eval();
    _phi2 = _phi2.array().square().matrix().eval();
}

template<typename Mesh>
void WKS<Mesh>::build(const Mesh& mesh,
                      const Vec* eigenvalues,
                      const Mat* eigenfunctions)
{
    _mesh = &mesh;
    // abs fix numerical error
    _lambda_max = std::abs(eigenvalues->coeff(eigenvalues->size() - 1));
    _lambda_min = std::abs(eigenvalues->coeff(1));
    _loglambda = (*eigenvalues).array().abs().log().matrix().eval();
    _phi2 = (*eigenfunctions).array().square().matrix().eval();
}

template<typename Mesh>
template<typename Derived>
void WKS<Mesh>::compute(Eigen::ArrayBase<Derived>& wks,
                        unsigned escales,
                        float emin,
                        float emax,
                        float sigma)
{
    if (emin >= emax || sigma <= 0) {
        // the parameters described in paper form a linear system
        Eigen::Matrix3f A;
        Eigen::Vector3f B;
        A << 1.0f, 0.0f, -2.0f, 0.0f, 1.0f, 2.0f, 7.0f, -7.0f, escales + 0.0f;
        B << std::log(_lambda_min), std::log(_lambda_max), 0.0f;
        Eigen::Vector3f x = A.colPivHouseholderQr().solve(B);
        emin = x(0);
        emax = x(1);
        sigma = x(2);
        EASSERT(emin < 0);
        EASSERT(emax > emin);
        EASSERT(sigma > 0);
    }
    auto estep = (emax - emin) / escales;
    auto edenom = 0.5f / (sigma * sigma);
    auto vimap = get(boost::vertex_index, *_mesh);
    auto nv = num_vertices(*_mesh);
    wks.derived().resize(escales, nv);

    for (auto v : vertices(*_mesh)) {
        auto idx = get(vimap, v);
        for (size_t i = 0; i < escales; ++i) {
            auto e = emin + estep * i;
            auto wks_e = static_cast<FT>(0);
            auto ce = static_cast<FT>(0);
            for (int j = 1; j < _loglambda.size(); ++j) {
                auto exp = std::exp(-std::pow(e - _loglambda(j), 2) * edenom);
                ce += exp;
                wks_e += exp * _phi2(idx, j);
            }
            wks(i, idx) = wks_e / ce;
        }
    }
}

} // namespace Euclid
