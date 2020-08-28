#include <stdexcept>

#include <boost/math/constants/constants.hpp>
#include <Eigen/Eigenvalues>
#include <Euclid/Math/Statistics.h>
#include <Euclid/Math/Vector.h>

namespace Euclid
{

template<typename Kernel>
void OBB<Kernel>::build(const std::vector<FT>& positions)
{
    if (positions.empty()) {
        throw std::invalid_argument("Input is empty.");
    }
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Size of input is not divisble by 3.");
    }

    Eigen::Map<const Eigen::
                   Matrix<FT, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>>
        v(positions.data(), positions.size() / 3, 3);
    _build(v);
}

template<typename Kernel>
template<typename ForwardIterator, typename VPMap>
void OBB<Kernel>::build(ForwardIterator first,
                        ForwardIterator beyond,
                        VPMap vpmap)
{
    std::vector<FT> positions;
    while (first != beyond) {
        positions.push_back(get(vpmap, *first).x());
        positions.push_back(get(vpmap, *first).y());
        positions.push_back(get(vpmap, *first).z());
        ++first;
    }
    build(positions);
}

template<typename Kernel>
template<typename Derived>
void OBB<Kernel>::build(const Eigen::MatrixBase<Derived>& v)
{
    if (v.rows() == 0) {
        throw std::invalid_argument("Input is empty.");
    }

    _build(v);
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
template<typename Derived>
void OBB<Kernel>::_build(const Eigen::MatrixBase<Derived>& points)
{
    using namespace boost::math::constants;

    // Conduct pca by eigen decompose the covariance matrix of points
    Eigen::Matrix<FT, 3, 3> covariance;
    Euclid::covariance_matrix(points, covariance);
    Eigen::SelfAdjointEigenSolver<Eigen::Matrix<FT, 3, 3>> eigs;
    eigs.compute(covariance);
    if (eigs.info() != Eigen::Success) {
        throw std::runtime_error("PCA no convergence.");
    }

    // Unit lengh, sorted in descending order w.r.t. eigen values
    Euclid::eigen_to_cgal(eigs.eigenvectors().col(2), _directions[0]);
    Euclid::eigen_to_cgal(eigs.eigenvectors().col(1), _directions[1]);
    Euclid::eigen_to_cgal(eigs.eigenvectors().col(0), _directions[2]);

    // Transform points to pca coordinate system
    Eigen::Matrix<FT, Eigen::Dynamic, 3> tpoints = points * eigs.eigenvectors();
    Eigen::Matrix<FT, Eigen::Dynamic, 1> tmax = tpoints.colwise().maxCoeff();
    Eigen::Matrix<FT, Eigen::Dynamic, 1> tmin = tpoints.colwise().minCoeff();

    // Record center and length of box
    _center = CGAL::ORIGIN + half<FT>() * (tmax(2) + tmin(2)) * _directions[0] +
              half<FT>() * (tmax(1) + tmin(1)) * _directions[1] +
              half<FT>() * (tmax(0) + tmin(0)) * _directions[2];
    _directions[0] *= half<FT>() * (tmax(2) - tmin(2));
    _directions[1] *= half<FT>() * (tmax(1) - tmin(1));
    _directions[2] *= half<FT>() * (tmax(0) - tmin(0));
}

} // namespace Euclid
