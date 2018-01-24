#include <Euclid/Math/Matrix.h>
#include <Euclid/Math/Vector.h>
#include <cmath>
#include <tuple>

namespace Euclid
{

namespace _impl
{

template<typename FT>
void _to_eigen(const std::vector<FT>& positions,
               std::vector<Eigen::Matrix<FT, 3, 1>>& points)
{
    points.reserve(positions.size() / 3);
    for (size_t i = 0; i < positions.size(); i += 3) {
        points.emplace_back(positions[i], positions[i + 1], positions[i + 2]);
    }
}

template<typename Point_3, typename FT>
void _to_eigen(const std::vector<Point_3>& pointset,
               std::vector<Eigen::Matrix<FT, 3, 1>>& points)
{
    points.reserve(pointset.size());
    for (const auto& p : pointset) {
        points.emplace_back(p.x(), p.y(), p.z());
    }
}

template<typename Mesh, typename FT>
void _to_eigen(const Mesh& mesh, std::vector<Eigen::Matrix<FT, 3, 1>>& points)
{
    points.reserve(num_vertices(mesh));
    auto vpmap = get(CGAL::vertex_point, mesh);
    auto[v_beg, v_end] = vertices(mesh);
    while (v_beg != v_end) {
        auto p = vpmap[*v_beg++];
        points.emplace_back(p.x(), p.y(), p.z());
    }
}

template<typename ForwardIterator, typename PPMap, typename FT>
void _to_eigen(ForwardIterator first,
               ForwardIterator beyond,
               PPMap point_pmap,
               std::vector<Eigen::Matrix<FT, 3, 1>>& points)
{
    while (first != beyond) {
        auto p = point_pmap[*first++];
        points.emplace_back(p.x(), p.y(), p.z());
    }
}

} // namespace _impl

template<typename Kernel>
inline OBB<Kernel>::OBB(const std::vector<FT>& positions)
{
    std::vector<EigenVec> points;
    _impl::_to_eigen(positions, points);
    _build_obb(points);
}

template<typename Kernel>
inline OBB<Kernel>::OBB(const std::vector<Point_3>& pointset)
{
    std::vector<EigenVec> points;
    _impl::_to_eigen(pointset, points);
    _build_obb(points);
}

template<typename Kernel>
template<typename Mesh>
inline OBB<Kernel>::OBB(const Mesh& mesh)
{
    std::vector<EigenVec> points;
    _impl::_to_eigen(mesh, points);
    _build_obb(points);
}

template<typename Kernel>
template<typename ForwardIterator, typename PPMap>
inline OBB<Kernel>::OBB(ForwardIterator first,
                        ForwardIterator beyond,
                        PPMap point_pmap)
{
    std::vector<EigenVec> points;
    _impl::_to_eigen(first, beyond, point_pmap, points);
    _build_obb(points);
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3 OBB<Kernel>::center() const
{
    return _lbb + _directions[0] * 0.5 + _directions[1] * 0.5 +
           _directions[2] * 0.5;
}

template<typename Kernel>
template<size_t N>
inline typename OBB<Kernel>::Vector_3 OBB<Kernel>::axis() const
{
    static_assert(N < 3);
    return Euclid::normalized(_directions[N]);
}

template<typename Kernel>
template<size_t N>
inline typename OBB<Kernel>::FT OBB<Kernel>::length() const
{
    static_assert(N < 3);
    return Euclid::length(_directions[N]);
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3 OBB<Kernel>::lbb() const
{
    return _lbb;
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3 OBB<Kernel>::lbf() const
{
    return _lbb + _directions[2];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3 OBB<Kernel>::ltb() const
{
    return _lbb + _directions[1];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3 OBB<Kernel>::ltf() const
{
    return _lbb + _directions[1] + _directions[2];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3 OBB<Kernel>::rbb() const
{
    return _lbb + _directions[0];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3 OBB<Kernel>::rbf() const
{
    return _lbb + _directions[0] + _directions[2];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3 OBB<Kernel>::rtb() const
{
    return _lbb + _directions[0] + _directions[1];
}

template<typename Kernel>
inline typename OBB<Kernel>::Point_3 OBB<Kernel>::rtf() const
{
    return _lbb + _directions[0] + _directions[1] + _directions[2];
}

template<typename Kernel>
inline void OBB<Kernel>::_build_obb(
    const std::vector<typename OBB<Kernel>::EigenVec>& points)
{
    // Conduct pca analysis
    Euclid::PCA<FT, 3> pca(points);

    // Unit lengh, sorted
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

    // Record corner point and direction vector
    _lbb = CGAL::ORIGIN + x_min * _directions[0] + y_min * _directions[1] +
           z_min * _directions[2];
    _directions[0] *= (x_max - x_min);
    _directions[1] *= (y_max - y_min);
    _directions[2] *= (z_max - z_min);
}

} // namespace Euclid
