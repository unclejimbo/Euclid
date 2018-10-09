#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

#include <Eigen/SparseCore>
#include <Euclid/Geometry/Spectral.h>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Util/Assert.h>

namespace Euclid
{

template<typename Mesh>
void HKS<Mesh>::build(const Mesh& mesh, unsigned k)
{
    Vec lambdas;
    Mat phis;
    spectrum(mesh, k, lambdas, phis);
    this->mesh = &mesh;
    this->eigenvalues.reset(new Vec(lambdas), true);
    this->eigenfunctions.reset(new Mat(phis), true);
}

template<typename Mesh>
void HKS<Mesh>::build(const Mesh& mesh,
                      const Vec* eigenvalues,
                      const Mat* eigenfunctions)
{
    this->mesh = &mesh;
    this->eigenvalues.reset(eigenvalues);
    this->eigenfunctions.reset(eigenfunctions);
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
        tmin = c / this->eigenvalues->coeff(this->eigenvalues->size() - 1);
        tmax = c / this->eigenvalues->coeff(1);
    }
    auto log_tmin = std::log(tmin);
    auto log_tmax = std::log(tmax);
    auto log_tstep = (log_tmax - log_tmin) / tscales;
    auto vimap = get(boost::vertex_index, *this->mesh);
    auto nv = num_vertices(*this->mesh);
    hks.derived().resize(tscales, nv);

    for (auto v : vertices(*this->mesh)) {
        auto idx = get(vimap, v);
        for (size_t i = 0; i < tscales; ++i) {
            auto t = std::exp(log_tmin + log_tstep * i);
            auto hks_t = static_cast<FT>(0);
            for (int j = 0; j < this->eigenvalues->size(); ++j) {
                auto eig = this->eigenvalues->coeff(j);
                auto e = std::exp(-eig * t);
                auto phi = this->eigenfunctions->coeff(idx, j);
                hks_t += e * phi * phi;
            }
            hks(i, idx) = hks_t;
        }
    }
    hks.colwise() /= hks.rowwise().sum();
}

} // namespace Euclid
