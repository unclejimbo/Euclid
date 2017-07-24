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

template<typename Kernel>
typename Kernel::FT
sine(const CGAL::Vector_3<Kernel>& v1,
	const CGAL::Vector_3<Kernel>& v2)
{
	auto inner_prod = v1 * v2;
	auto squared_cos = inner_prod * inner_prod /
		(v1.squared_length() * v2.squared_length());
	if (squared_cos > static_cast<typename Kernel::FT>(1.0)) {
		squared_cos = static_cast<typename Kernel::FT>(1.0);
	}
	auto sin = std::sqrt(1.0 - squared_cos);
	return sin;
}

template<typename Kernel>
typename Kernel::FT
sine(const CGAL::Point_3<Kernel>& p1,
	const CGAL::Point_3<Kernel>& p2,
	const CGAL::Point_3<Kernel>& p3)
{
	return sine(p1 - p2, p3 - p2);
}

template<typename Kernel>
typename Kernel::FT
cosine(const CGAL::Vector_3<Kernel>& v1,
	const CGAL::Vector_3<Kernel>& v2)
{
	auto cos = v1 * v2 / (length(v1) * length(v2));
	// If floating point precision error happens
	if (cos > static_cast<typename Kernel::FT>(1.0)) {
		cos = static_cast<typename Kernel::FT>(1.0);
	}
	if (cos < static_cast<typename Kernel::FT>(-1.0)) {
		cos = static_cast<typename Kernel::FT>(-1.0);
	}
	return cos;
}

template<typename Kernel>
typename Kernel::FT
cosine(const CGAL::Point_3<Kernel>& p1,
	const CGAL::Point_3<Kernel>& p2,
	const CGAL::Point_3<Kernel>& p3)
{
	return cosine(p1 - p2, p3 - p2);
}

template<typename Kernel>
typename Kernel::FT
tangent(const CGAL::Vector_3<Kernel>& v1,
	const CGAL::Vector_3<Kernel>& v2)
{
	auto inner_prod = v1 * v2;
	auto squared_cos = inner_prod * inner_prod /
		(v1.squared_length() * v2.squared_length());
	if (squared_cos > static_cast<typename Kernel::FT>(1.0)) {
		squared_cos = static_cast<typename Kernel::FT>(1.0);
	}
	if (squared_cos == static_cast<typename Kernel::FT>(0.0)) {
		return NAN;
	}
	else {
		return std::sqrt((1.0 - squared_cos) / squared_cos)
	}
}

template<typename Kernel>
typename Kernel::FT
tangent(const CGAL::Point_3<Kernel>& p1,
	const CGAL::Point_3<Kernel>& p2,
	const CGAL::Point_3<Kernel>& p3)
{
	return tangent(p1 - p2, p3 - p2);
}

template<typename Kernel>
typename Kernel::FT
cotangent(const CGAL::Vector_3<Kernel>& v1,
	const CGAL::Vector_3<Kernel>& v2)
{
	auto inner_prod = v1 * v2;
	auto squared_cos = inner_prod * inner_prod /
		(v1.squared_length() * v2.squared_length());
	if (squared_cos > static_cast<typename Kernel::FT>(1.0)) {
		squared_cos = static_cast<typename Kernel::FT>(1.0);
	}
	if (squared_cos == static_cast<typename Kernel::FT>(1.0)) {
		return NAN;
	}
	else {
		return std::sqrt(squared_cos / (1.0 - squared_cos));
	}
}

template<typename Kernel>
typename Kernel::FT
cotangent(const CGAL::Point_3<Kernel>& p1,
	const CGAL::Point_3<Kernel>& p2,
	const CGAL::Point_3<Kernel>& p3)
{
	return cotangent(p1 - p2, p3 - p2);
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
