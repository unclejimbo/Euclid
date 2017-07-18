#include <cmath>

namespace Euclid
{

template<typename Kernel>
inline typename Kernel::FT length(const CGAL::Vector_3<Kernel>& vec)
{
	return std::sqrt(vec.squared_length());
}

template<typename Kernel>
inline void normalize(CGAL::Vector_3<Kernel>& vec)
{
	vec /= Euclid::length(vec);
}

template<typename Kernel>
inline CGAL::Vector_3<Kernel> normalized(const CGAL::Vector_3<Kernel>& vec)
{
	return vec / Euclid::length(vec);
}

template<typename FT, typename CGALVec>
inline Eigen::Matrix<FT, 3, 1> cgal_to_eigen(const CGALVec& vec)
{
	return Eigen::Matrix<FT, 3, 1>(vec.x(), vec.y(), vec.z());
}

template<typename CGALVec, typename FT>
inline CGALVec eigen_to_cgal(const Eigen::Matrix<FT, 3, 1>& vec)
{
	return CGALVec(vec(0), vec(1), vec(2));
}

} // namespace Euclid
