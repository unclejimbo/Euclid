/** Object oriented bounding box.
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
 *  The length of x, y, z axis decreases successively.
 *  And the coordniate system is right-hand based, thus right means
 *  x, up means y and front means z.
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
    explicit OBB(const std::vector<Point_3>& pointset);

    /** Build OBB for a range of points.
     *
     */
    template<typename ForwardIterator, typename PPMap>
    OBB(ForwardIterator first, ForwardIterator beyond, PPMap point_pmap);

    /** Return the center of the box.
     *
     */
    Point_3 center() const;

    /** Return the unit-length axis.
     *
     */
    template<size_t N>
    Vector_3 axis() const;

    /** Return the length of an axis.
     *
     */
    template<size_t N>
    FT length() const;

    /** Return the left-bottom-back corner of the box.
     *
     */
    Point_3 lbb() const;

    /** Return the left-bottom-front corner of the box.
     *
     */
    Point_3 lbf() const;

    /** Return the left-top-back corner of the box.
     *
     */
    Point_3 ltb() const;

    /** Return the left-top-front corner of the box.
     *
     */
    Point_3 ltf() const;

    /** Return the right-bottom-back corner of the box.
     *
     */
    Point_3 rbb() const;

    /** Return the right-bottom-front corner of the box.
     *
     */
    Point_3 rbf() const;

    /** Return the right-top-back corner of the box.
     *
     */
    Point_3 rtb() const;

    /** Return the right-top-front corner of the box.
     *
     */
    Point_3 rtf() const;

private:
    using EigenVec = typename Eigen::Matrix<FT, 3, 1>;

private:
    void _build_obb(const std::vector<EigenVec>& points);

private:
    Point_3 _lbb;
    std::array<Vector_3, 3> _directions;
};

/** @}*/
} // namespace Euclid

#include "src/OBB.cpp"
