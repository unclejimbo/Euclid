#include <algorithm>
#include <exception>
#include <utility>

namespace Euclid
{

template<typename FT, int RowSize>
inline Eigen::Matrix<FT, RowSize, RowSize> covariance_matrix(
    const std::vector<Eigen::Matrix<FT, RowSize, 1>>& points)
{
    if (points.size() > 1) {
        Eigen::Matrix<FT, RowSize, Eigen::Dynamic> mat(RowSize, points.size());
        for (auto i = 0; i < points.size(); ++i) {
            mat.col(i) = points[i];
        }
        auto centered = mat.colwise() - mat.rowwise().mean();
        return (centered * centered.adjoint()) / (mat.cols() - 1.0);
    }
    else if (points.size() == 1) {
        return Eigen::Matrix<FT, RowSize, RowSize>::Zero();
    }
    else {
        throw std::invalid_argument("Input vector is empty!");
    }
}

template<typename FT, int RowSize>
inline PCA<FT, RowSize>::PCA(
    const std::vector<Eigen::Matrix<FT, RowSize, 1>>& points)
{
    auto covariance = Euclid::covariance_matrix<FT, RowSize>(points);

    Eigen::SelfAdjointEigenSolver<Eigen::Matrix<FT, RowSize, RowSize>>
        eigensolver(covariance);
    if (eigensolver.info() != Eigen::Success) {
        throw std::runtime_error("Eigen decomposition does not converge!");
    }
    else {
        // Sort the eigen vectors wrt eigen values in descending order
        auto eigen_values = eigensolver.eigenvalues();
        std::vector<std::pair<FT, int>> value_index_map;
        value_index_map.reserve(eigen_values.rows());
        for (auto i = 0; i < eigen_values.rows(); ++i) {
            value_index_map.emplace_back(eigen_values(i, 0), i);
        }
        std::sort(
            value_index_map.begin(),
            value_index_map.end(),
            [](const std::pair<FT, int>& v1, const std::pair<FT, int>& v2) {
                return v1.first > v2.first;
            });

        _eigen_values.resize(value_index_map.size());
        std::transform(value_index_map.begin(),
                       value_index_map.end(),
                       _eigen_values.begin(),
                       [](const std::pair<FT, int>& v) { return v.first; });

        auto eigen_vectors = eigensolver.eigenvectors();
        _eigen_vectors.resize(value_index_map.size());
        std::transform(value_index_map.begin(),
                       value_index_map.end(),
                       _eigen_vectors.begin(),
                       [&eigen_vectors](const std::pair<FT, int>& v) {
                           return Eigen::Matrix<FT, RowSize, 1>(
                               eigen_vectors.col(v.second));
                       });
    }
}

template<typename FT, int RowSize>
template<int i>
inline FT PCA<FT, RowSize>::eigen_value() const
{
    static_assert(i < RowSize);
    return _eigen_values[i];
}

template<typename FT, int RowSize>
template<int i>
inline Eigen::Matrix<FT, RowSize, 1> PCA<FT, RowSize>::eigen_vector() const
{
    static_assert(i < RowSize);
    return _eigen_vectors[i];
}

} // namespace Euclid
