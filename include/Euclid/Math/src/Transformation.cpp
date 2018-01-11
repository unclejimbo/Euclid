#include <Euclid/Math/Vector.h>
#include <exception>

namespace Euclid
{

template<typename Kernel>
inline CGAL::Aff_transformation_3<Kernel>
transform_between_2_coord_systems(
	const typename Kernel::Point_3& from_origin,
	const typename Kernel::Point_3& from_x,
	const typename Kernel::Point_3& from_y,
	const typename Kernel::Point_3& to_origin,
	const typename Kernel::Point_3& to_x,
	const typename Kernel::Point_3& to_y)
{
	using Transform = CGAL::Aff_transformation_3<Kernel>;

	if (CGAL::collinear(from_origin, from_x, from_y)) {
		throw std::invalid_argument("Inputs are collinear!");
	}
	if (CGAL::collinear(to_origin, to_x, to_y)) {
		throw std::invalid_argument("Inputs are collinear!");
	}

	auto x1 = Euclid::normalized(from_x - from_origin);
	auto y1 = Euclid::normalized(from_y - from_origin);
	auto z1 = CGAL::cross_product(x1, y1);
	auto x2 = Euclid::normalized(to_x - to_origin);
	auto y2 = Euclid::normalized(to_y - to_origin);
	auto z2 = CGAL::cross_product(x2, y2);

	Transform tf(x1.x(), x1.y(), x1.z(),
		y1.x(), y1.y(), y1.z(),
		z1.x(), z1.y(), z1.z());
	Transform tt(x2.x(), x2.y(), x2.z(),
		y2.x(), y2.y(), y2.z(),
		z2.x(), z2.y(), z2.z());

	auto transformation = Transform(CGAL::Translation(), from_origin - to_origin);
	transformation = tt * tf.inverse() * transformation;

	return transformation;
}

template<typename Kernel>
inline CGAL::Aff_transformation_3<Kernel>
transform_between_2_coord_systems(
	const typename Kernel::Point_3& to_origin,
	const typename Kernel::Point_3& to_x,
	const typename Kernel::Point_3& to_y)
{
	using Transform = CGAL::Aff_transformation_3<Kernel>;

	if (CGAL::collinear(to_origin, to_x, to_y)) {
		throw std::invalid_argument("Inputs are collinear!");
	}

	auto x2 = Euclid::normalized(to_x - to_origin);
	auto y2 = Euclid::normalized(to_y - to_origin);
	auto z2 = CGAL::cross_product(x2, y2);

	Transform tt(x2.x(), x2.y(), x2.z(),
		y2.x(), y2.y(), y2.z(),
		z2.x(), z2.y(), z2.z());

	auto transformation = Transform(CGAL::Translation(), CGAL::ORIGIN - to_origin);
	transformation = tt * transformation;

	return transformation;
}

}
