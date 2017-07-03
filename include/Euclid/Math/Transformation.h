/************************
* Package Overview:     *
* Space transformations *
************************/
#pragma once
#include <CGAL/Aff_transformation_3.h>

namespace Euclid
{

template<typename Kernel>
bool transform_between_2_coord_systems(
	const typename Kernel::Point_3& from_origin,
	const typename Kernel::Point_3& from_x,
	const typename Kernel::Point_3& from_y,
	const typename Kernel::Point_3& to_origin,
	const typename Kernel::Point_3& to_x,
	const typename Kernel::Point_3& to_y,
	CGAL::Aff_transformation_3<Kernel>& transformation);

} // namespace Euclid

#include "src/Transformation.cpp"
