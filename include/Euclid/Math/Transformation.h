/** Euclidean transformations.
 *
 * This package contains functions for geometric transformation
 * in the Euclidean space.
 * @defgroup PkgTransformation Transformation
 * @ingroup PkgMath
 */
#pragma once

#include <CGAL/Aff_transformation_3.h>

namespace Euclid
{
/** @{*/

/** Transform between two coordinate systems.
 *
 */
template<typename Kernel>
CGAL::Aff_transformation_3<Kernel> transform_between_2_coord_systems(
    const typename Kernel::Point_3& from_origin,
    const typename Kernel::Point_3& from_x,
    const typename Kernel::Point_3& from_y,
    const typename Kernel::Point_3& to_origin,
    const typename Kernel::Point_3& to_x,
    const typename Kernel::Point_3& to_y);

/** Transform from the "standard" world coordinate system.
 *
 *  The standard coordinate system is assumed to be (1, 0, 0),
 *  (0, 1, 0), (0, 0, 1) under the world coordinate system
 *  that the arguments are defined.
 */
template<typename Kernel>
CGAL::Aff_transformation_3<Kernel> transform_from_world_coord(
    const typename Kernel::Point_3& to_origin,
    const typename Kernel::Point_3& to_x,
    const typename Kernel::Point_3& to_y);

/** Transform to the "standard" world coordinate system.
 *
 *  The standard coordinate system is assumed to be (1, 0, 0),
 *  (0, 1, 0), (0, 0, 1) under the world coordinate system
 *  that the arguments are defined.
 */
template<typename Kernel>
CGAL::Aff_transformation_3<Kernel> transform_to_world_coord(
    const typename Kernel::Point_3& from_origin,
    const typename Kernel::Point_3& from_x,
    const typename Kernel::Point_3& from_y);

/** @}*/
} // namespace Euclid

#include "src/Transformation.cpp"
