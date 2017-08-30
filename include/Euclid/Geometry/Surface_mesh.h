/**********************************************************
* Package Overview:                                       *
* This package contains helper functions for Surface_mesh *
**********************************************************/
#pragma once
#include <CGAL/Surface_mesh.h>
#include <boost/property_map/property_map.hpp>
#include <vector>

namespace Euclid
{

template<typename Point_3>
bool build_surface_mesh(
	CGAL::Surface_mesh<Point_3>& mesh,
	const std::vector<typename CGAL::Kernel_traits<Point_3>::Kernel::FT>& vertices,
	const std::vector<unsigned>& indices);

template<typename Point_3>
bool build_surface_mesh(
	CGAL::Surface_mesh<Point_3>& mesh,
	const std::vector<Point_3>& vertices,
	const std::vector<unsigned>& indices);

template<typename ValueType>
class SM_PropertyMap
{
public:
	using value_type = ValueType;
	using reference = const value_type&;
	using key_type = size_t;
	using category = boost::lvalue_property_map_tag;

	explicit SM_PropertyMap(const std::vector<value_type>& values)
		: _values(values) {}
	reference operator[](key_type key) const
	{
		return _values[key];
	}
	friend reference get(SM_PropertyMap pmap, key_type key)
	{
		return pmap[key];
	}

private:
	const std::vector<value_type>& _values;
};

} // namespace Euclid

#include "src/Surface_mesh.cpp"
