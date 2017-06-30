/*********************************************************
* Package Overview                                       *
* (O)beject (O)rientated (B)oundingbox uses pca to build *
* an object aligned bounding box                         *
*********************************************************/
#pragma once
#include <CGAL/Point_set_3.h>
#include <vector>
#include <array>

namespace Euclid
{

template<typename Kernel>
class OBB
{
	using Point_3 = typename Kernel::Point_3;
	using Vector_3 = typename Kernel::Vector_3;
	using FT = typename Kernel::FT;

public:
	template<typename Mesh>
	explicit OBB(const Mesh& mesh);

	explicit OBB(const std::vector<Point_3>& points);

	template<typename ForwardIterator, typename PPMap>
	OBB(ForwardIterator first,
		ForwardIterator beyond,
		PPMap point_pmap);

	explicit OBB(const CGAL::Point_set_3<Point_3>& point_set);

	Point_3 center() const;
	Point_3 lbb() const;
	Point_3 lbf() const;
	Point_3 ltb() const;
	Point_3 ltf() const;
	Point_3 rbb() const;
	Point_3 rbf() const;
	Point_3 rtb() const;
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

} // namespace Euclid

#include "src/OBB.cpp"
