#pragma once

#include <array>
#include <vector>

#include <Eigen/Core>

namespace Euclid
{
/** @{ @ingroup PkgBoundingVolume*/

/** Object oriented bounding box.
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
    /** Build OBB for a set of raw positions.
     *
     */
    void build(const std::vector<FT>& positions);

    /** Build OBB for a range of points.
     *
     */
    template<typename ForwardIterator, typename VPMap>
    void build(ForwardIterator first, ForwardIterator beyond, VPMap vpmap);

    /** Build OBB for points represented by a matrix.
     *
     */
    template<typename Derived>
    void build(const Eigen::MatrixBase<Derived>& v);

    /** Return the center of the box.
     *
     */
    Point_3 center() const;

    /** Return the unit-length axis.
     *
     *  The accepted values of n are 0, 1 and 2. The axis are sorted in
     *  descending order w.r.t. to the length, so OBB<KerneL>::axis(0) returns
     *  the longest axis, although the sign is arbitrary.
     */
    Vector_3 axis(int n) const;

    /** Return the length of an axis.
     *
     *  The accepted values of n are 0, 1 and 2. The length are sorted in
     *  descending order.
     */
    FT length(int n) const;

private:
    template<typename Derived>
    void _build(const Eigen::MatrixBase<Derived>& points);

private:
    Point_3 _center;
    std::array<Vector_3, 3> _directions;
};

/** @}*/
} // namespace Euclid

#include "src/OBB.cpp"
