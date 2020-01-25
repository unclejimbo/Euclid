#pragma once

#include <array>
#include <vector>

namespace Euclid
{
/** @{ @ingroup PkgBoundingVolume*/

/** Axis Aligned Bounding Box.
 *
 *  This class uses right-handed coordinate system.
 */
template<typename Kernel>
class AABB
{
public:
    using Point_3 = typename Kernel::Point_3;
    using FT = typename Kernel::FT;

public:
    /** Build AABB for a set of positions.
     *
     */
    explicit AABB(const std::vector<FT>& positions);

    /** Build AABB for a set of points.
     *
     */
    explicit AABB(const std::vector<Point_3>& points);

    /** Build AABB for a range of points.
     *
     */
    template<typename ForwardIterator, typename VPMap>
    AABB(ForwardIterator first, ForwardIterator beyond, VPMap vpmap);

    /** Return the center of the box.
     *
     */
    Point_3 center() const;

    /** Return x min.
     *
     */
    FT xmin() const;

    /** Return x max.
     *
     */
    FT xmax() const;

    /** Return the length of edge along the x axis.
     *
     */
    FT xlen() const;

    /** Return y min.
     *
     */
    FT ymin() const;

    /** Return y max.
     *
     */
    FT ymax() const;

    /** Return the length of edge along the y axis.
     *
     */
    FT ylen() const;

    /** Return z min.
     *
     */
    FT zmin() const;

    /** Return z max.
     *
     */
    FT zmax() const;

    /** Return the length of edge along the z axis.
     *
     */
    FT zlen() const;

    /** Return the corner point of the box.
     *
     *  @param xlarge Set to true for the larger x-corner, to false otherwise.
     *  @param ylarge Set to true for the larger y-corner, to false otherwise.
     *  @param zlarge Set to true for the larger z-corner, to false otherwise.
     */
    Point_3 point(bool xlarge, bool ylarge, bool zlarge) const;

private:
    void _build_aabb(FT xmin, FT xmax, FT ymin, FT ymax, FT zmin, FT zmax);

private:
    Point_3 _center;
    FT _xlen; // Half length
    FT _ylen;
    FT _zlen;
};

/** @}*/
} // namespace Euclid

#include "src/AABB.cpp"
