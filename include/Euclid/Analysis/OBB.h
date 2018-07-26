/** Object oriented bounding box for 3D geometry.
 *
 *  Use pca analysis to generate an object oriented bounding box.
 *  @defgroup PkgOBB OBB
 *  @ingroup PkgAnalysis
 */
#pragma once

#include <array>
#include <vector>

#include <Eigen/Dense>

namespace Euclid
{
/** @{*/

/** Object Oriented Bounding Box.
 *
 *  This class uses right-handed coordinate system.
 */
template<typename Kernel>
class OBB
{
public:
    using Point_3 = typename Kernel::Point_3;
    using Vector_3 = typename Kernel::Vector_3;
    using FT = typename Kernel::FT;

public:
    /** Build OBB for a set of positions.
     *
     */
    explicit OBB(const std::vector<FT>& positions);

    /** Build OBB for a set of points.
     *
     */
    explicit OBB(const std::vector<Point_3>& points);

    /** Build OBB for a range of points.
     *
     */
    template<typename ForwardIterator, typename VPMap>
    OBB(ForwardIterator first, ForwardIterator beyond, VPMap vpmap);

    /** Return the center of the box.
     *
     */
    Point_3 center() const;

    /** Return the unit-length axis.
     *
     *  The accepted values of n are 0, 1 and 2. The axis are sorted in
     *  descending order w.r.t. to the length, so OBB<KerneL>::axis(0) returns
     *  the longest axis, although the direction is arbitrary.
     */
    Vector_3 axis(int n) const;

    /** Return the length of an axis.
     *
     *  The accepted values of n are 0, 1 and 2. The length are sorted in
     *  descending order.
     */
    FT length(int n) const;

private:
    using EigenVec = typename Eigen::Matrix<FT, 3, 1>;

private:
    void _build_obb(const std::vector<EigenVec>& points);

private:
    Point_3 _center;
    std::array<Vector_3, 3> _directions;
};

/** @}*/
} // namespace Euclid

#include "src/OBB.cpp"
