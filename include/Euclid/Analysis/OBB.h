/** Object oriented bounding box.
 *
 *  Use pca analysis to generate an object oriented bounding box.
 *  @defgroup PkgOBB OBB
 *  @ingroup PkgAnalysis
 */
#pragma once
#include <CGAL/Point_set_3.h>
#include <vector>
#include <array>

namespace Euclid
{
/** @{*/

/** OBB
 *
 */
template<typename Kernel>
class OBB
{
	using Point_3 = typename Kernel::Point_3;
	using Vector_3 = typename Kernel::Vector_3;
	using FT = typename Kernel::FT;

public:
	/** Build OBB for a mesh.
	 *
	 */
	template<typename Mesh>
	explicit OBB(const Mesh& mesh);


	/** Build OBB for a set of points.
	 *
	 */
	explicit OBB(const std::vector<Point_3>& points);


	/** Build OBB for a range of points.
	 *
	 */
	template<typename ForwardIterator, typename PPMap>
	OBB(ForwardIterator first,
		ForwardIterator beyond,
		PPMap point_pmap);


	/** Build OBB for a CGAL::Point_set_3.
	 *
	 */
	explicit OBB(const CGAL::Point_set_3<Point_3>& point_set);


	/** Return the center of the box.
	 *
	 */
	Point_3 center() const;


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
	template<typename ForwardIterator, typename PPMap>
	void _buildOBB(ForwardIterator first,
		ForwardIterator beyond,
		PPMap point_pmap);

private:
	Point_3 _lbb;
	std::array<Vector_3, 3> _directions;
};

/** @}*/
} // namespace Euclid

#include "src/OBB.cpp"
