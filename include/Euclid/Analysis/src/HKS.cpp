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
HKS<Mesh>::HKS(const Mesh& mesh, unsigned k)
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
    this->k = n;
    this->eigenvalues = eigensolver.eigenvalues();
    EASSERT(this->eigenvalues.rows() == n);
    this->eigenfunctions = eigensolver.eigenvectors();
    EASSERT(this->eigenfunctions.cols() == n);
    EASSERT(this->eigenfunctions.rows() == num_vertices(mesh));
}

template<typename Mesh>
template<typename T>
void HKS<Mesh>::compute(const Vertex& v,
                        std::vector<T>& hks,
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
        tmin = c / this->eigenvalues(this->k - 1);
        tmax = c / this->eigenvalues(1);
    }
    auto log_tmin = std::log(tmin);
    auto log_tmax = std::log(tmax);
    auto log_tstep = (log_tmax - log_tmin) / tscales;
    auto vimap = get(boost::vertex_index, *this->mesh);
    hks.clear();
    hks.resize(tscales);

    for (size_t i = 0; i < tscales; ++i) {
        auto t = std::exp(log_tmin + log_tstep * i);
        auto denom = static_cast<FT>(0);
        auto hks_t = static_cast<FT>(0);
        for (size_t j = 1; j < this->k; ++j) {
            auto eig = this->eigenvalues(j);
            auto e = std::exp(-eig * t);
            auto phi = this->eigenfunctions(get(vimap, v), j);
            denom += e;
            hks_t += e * phi * phi;
        }
        hks_t /= denom;
        hks[i] = hks_t;
    }
}

} // namespace Euclid
