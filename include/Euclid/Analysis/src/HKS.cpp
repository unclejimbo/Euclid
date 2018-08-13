#include <algorithm>
#include <cmath>
#include <stdexcept>
#include <string>

#include <Eigen/SparseCore>
#include <MatOp/SparseSymShiftSolve.h>
#include <SymEigsShiftSolver.h>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Math/Matrix.h>
#include <Euclid/Util/Assert.h>

namespace Euclid
{

template<typename Mesh>
HKS<Mesh>::~HKS()
{
    if (!_is_shared) {
        delete this->eigenvalues;
        delete this->eigenfunctions;
    }
}

template<typename Mesh>
void HKS<Mesh>::build(const Mesh& mesh, unsigned k)
{
    using SpMat = Eigen::SparseMatrix<FT>;

    if (k > num_vertices(mesh)) {
        std::string err("You've requested ");
        err.append(std::to_string(k));
        err.append(" eigen values but there are only ");
        err.append(std::to_string(num_vertices(mesh)));
        err.append(" vertices in your mesh.");
        EWARNING(err);
        k = num_vertices(mesh);
    }

    // Construct a symmetric Laplacian matrix
    SpMat cot_mat = Euclid::laplacian_matrix(mesh);
    SpMat mass_mat = Euclid::mass_matrix(mesh);
    Euclid::for_each(mass_mat, [](FT& value) { value = 1 / std::sqrt(value); });
    SpMat laplacian = -mass_mat * cot_mat * mass_mat;

    // Eigen decomposition of the Laplacian matrix
    auto convergence = std::min(2 * k + 1, num_vertices(mesh));
    Spectra::SparseSymShiftSolve<FT> op(laplacian);
    Spectra::SymEigsShiftSolver<FT,
                                Spectra::LARGEST_MAGN,
                                Spectra::SparseSymShiftSolve<FT>>
        eigensolver(&op, k, convergence, 0.0f);
    eigensolver.init();
    auto n = eigensolver.compute(
        1000, static_cast<FT>(1e-10), Spectra::SMALLEST_MAGN);
    if (eigensolver.info() != Spectra::SUCCESSFUL) {
        throw std::runtime_error(
            "Unable to compute eigen values of the Laplacian matrix.");
    }
    if (n < k) {
        auto str = std::to_string(k);
        str.append(" eigen values are requested, but only ");
        str.append(std::to_string(n));
        str.append(" values converged in computation.");
        EWARNING(str);
    }

    this->mesh = &mesh;
    this->eigenvalues = new Vec(eigensolver.eigenvalues());
    EASSERT(this->eigenvalues->rows() == n);
    this->eigenfunctions = new Mat(eigensolver.eigenvectors());
    EASSERT(this->eigenfunctions->cols() == n);
    EASSERT(this->eigenfunctions->rows() == num_vertices(mesh));
    _is_shared = false;
}

template<typename Mesh>
void HKS<Mesh>::build(const Mesh& mesh,
                      const Vec* eigenvalues,
                      const Mat* eigenfunctions)
{
    this->mesh = &mesh;
    this->eigenvalues = eigenvalues;
    this->eigenfunctions = eigenfunctions;
    _is_shared = true;
}

template<typename Mesh>
template<typename T>
void HKS<Mesh>::compute(const Vertex& v,
                        Eigen::Array<T, Eigen::Dynamic, 1>& hks,
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
    hks.resize(tscales);

    for (size_t i = 0; i < tscales; ++i) {
        auto t = std::exp(log_tmin + log_tstep * i);
        auto denom = static_cast<FT>(0);
        auto hks_t = static_cast<FT>(0);
        for (size_t j = 1; j < this->eigenvalues->size(); ++j) {
            auto eig = this->eigenvalues->coeff(j);
            auto e = std::exp(-eig * t);
            auto phi = this->eigenfunctions->coeff(get(vimap, v), j);
            denom += e;
            hks_t += e * phi * phi;
        }
        hks_t /= denom;
        hks(i) = hks_t;
    }
}

template<typename Mesh>
template<typename T>
void HKS<Mesh>::compute(Eigen::Array<T, Eigen::Dynamic, Eigen::Dynamic>& hks,
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
    hks.resize(tscales, nv);

    for (auto v : vertices(*this->mesh)) {
        auto idx = get(vimap, v);
        for (size_t i = 0; i < tscales; ++i) {
            auto t = std::exp(log_tmin + log_tstep * i);
            auto denom = static_cast<FT>(0);
            auto hks_t = static_cast<FT>(0);
            for (size_t j = 1; j < this->eigenvalues->size(); ++j) {
                auto eig = this->eigenvalues->coeff(j);
                auto e = std::exp(-eig * t);
                auto phi = this->eigenfunctions->coeff(idx, j);
                denom += e;
                hks_t += e * phi * phi;
            }
            hks_t /= denom;
            hks(i, idx) = hks_t;
        }
    }
}

} // namespace Euclid
