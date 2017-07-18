#include <cmath>
#include <iostream>

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

template<typename Kernel>
inline typename Kernel::FT
area(const CGAL::Point_3<Kernel>& p1,
	const CGAL::Point_3<Kernel>& p2,
	const CGAL::Point_3<Kernel>& p3)
{
	if (CGAL::collinear(p1, p2, p3)) {
		std::cerr << "Input points are collinear, area is zero" << std::endl;
		return 0.0;
	}
	else {
		return std::sqrt(CGAL::cross_product(p3 - p2, p1 - p2).squared_length()) * 0.5;
	}
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
