#include <cmath>
#include <exception>
#include <tuple>

#include <Euclid/Math/Matrix.h>
#include <Euclid/Math/Vector.h>

namespace Euclid
{

namespace _impl
{

template<typename FT>
void _to_eigen(const std::vector<FT>& positions,
               std::vector<Eigen::Matrix<FT, 3, 1>>& epoints)
{
    epoints.reserve(positions.size() / 3);
    for (size_t i = 0; i < positions.size(); i += 3) {
        epoints.emplace_back(positions[i], positions[i + 1], positions[i + 2]);
    }
}

template<typename Point_3, typename FT>
void _to_eigen(const std::vector<Point_3>& points,
               std::vector<Eigen::Matrix<FT, 3, 1>>& epoints)
{
    epoints.reserve(points.size());
    for (const auto& p : points) {
        epoints.emplace_back(p.x(), p.y(), p.z());
    }
}

template<typename ForwardIterator, typename PPMap, typename FT>
void _to_eigen(ForwardIterator first,
               ForwardIterator beyond,
               PPMap ppmap,
               std::vector<Eigen::Matrix<FT, 3, 1>>& epoints)
{
    while (first != beyond) {
        auto p = ppmap[*first++];
        epoints.emplace_back(p.x(), p.y(), p.z());
    }
}

} // namespace _impl

template<typename Kernel>
OBB<Kernel>::OBB(const std::vector<FT>& positions)
{
    if (positions.empty()) { throw std::invalid_argument("Input is empty"); }
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Size of input is not divisble by 3");
    }

    std::vector<EigenVec> epoints;
    _impl::_to_eigen(positions, epoints);
    _build_obb(epoints);
}

template<typename Kernel>
OBB<Kernel>::OBB(const std::vector<Point_3>& points)
{
    if (points.empty()) { throw std::invalid_argument("Input is empty"); }

    std::vector<EigenVec> epoints;
    _impl::_to_eigen(points, epoints);
    _build_obb(epoints);
}

template<typename Kernel>
template<typename ForwardIterator, typename PPMap>
OBB<Kernel>::OBB(ForwardIterator first, ForwardIterator beyond, PPMap ppmap)
{
    if (first == beyond) { throw std::invalid_argument("Input is empty"); }

    std::vector<EigenVec> epoints;
    _impl::_to_eigen(first, beyond, ppmap, epoints);
    _build_obb(epoints);
}

template<typename Kernel>
typename OBB<Kernel>::Point_3 OBB<Kernel>::center() const
{
    return _center;
}

template<typename Kernel>
typename OBB<Kernel>::Vector_3 OBB<Kernel>::axis(int n) const
{
    if (n < 0 || n >= 3) {
        throw std::invalid_argument("Invalid argument for OBB::length(int).");
    }
    return Euclid::normalized(_directions[n]);
}

template<typename Kernel>
typename OBB<Kernel>::FT OBB<Kernel>::length(int n) const
{
    if (n < 0 || n >= 3) {
        throw std::invalid_argument("Invalid argument for OBB::length(int).");
    }
    return Euclid::length(_directions[n]) * 2;
}

template<typename Kernel>
void OBB<Kernel>::_build_obb(
    const std::vector<typename OBB<Kernel>::EigenVec>& points)
{
    // Conduct pca analysis
    Euclid::PCA<FT, 3> pca(points);

    // Unit lengh, sorted in descending order w.r.t. eigen values
    _directions[0] =
        Euclid::eigen_to_cgal<Vector_3>(pca.template eigen_vector<0>());
    _directions[1] =
        Euclid::eigen_to_cgal<Vector_3>(pca.template eigen_vector<1>());
    _directions[2] =
        Euclid::eigen_to_cgal<Vector_3>(pca.template eigen_vector<2>());

    // Length in the pca coordinate system
    const auto& vec = points[0];
    auto x_max = vec.dot(pca.template eigen_vector<0>());
    auto x_min = x_max;
    auto y_max = vec.dot(pca.template eigen_vector<1>());
    auto y_min = y_max;
    auto z_max = vec.dot(pca.template eigen_vector<2>());
    auto z_min = z_max;
    for (size_t i = 1; i < points.size(); ++i) {
        const auto& vec = points[i];
        x_max = std::max(x_max, vec.dot(pca.template eigen_vector<0>()));
        x_min = std::min(x_min, vec.dot(pca.template eigen_vector<0>()));
        y_max = std::max(y_max, vec.dot(pca.template eigen_vector<1>()));
        y_min = std::min(y_min, vec.dot(pca.template eigen_vector<1>()));
        z_max = std::max(z_max, vec.dot(pca.template eigen_vector<2>()));
        z_min = std::min(z_min, vec.dot(pca.template eigen_vector<2>()));
    }

    // Record center point and direction vector
    constexpr const auto one_half = static_cast<FT>(0.5);
    _center = CGAL::ORIGIN + one_half * (x_min + x_max) * _directions[0] +
              one_half * (y_max + y_min) * _directions[1] +
              one_half * (z_min + z_max) * _directions[2];
    _directions[0] *= (x_max - x_min) * one_half; // half of box edge length
    _directions[1] *= (y_max - y_min) * one_half;
    _directions[2] *= (z_max - z_min) * one_half;

    // Store all corner points and their
}

} // namespace Euclid
