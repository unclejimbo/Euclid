/** CGAL::Polyhedron_3 utilities.
 *
 *  This package contains utilities to work with the
 *  CGAL::Polyhedron_3 package.
 *  @defgroup PkgPolyhedron Polyhedron_3
 *  @ingroup PkgGeometry
 */
#pragma once
#include <CGAL/Polyhedron_3.h>

namespace Euclid
{
/** @{*/

/** Triangle mesh builder for Polyhedron_3.
 *
 */
template<typename Polyhedron_3>
class TriMeshBuilder : public CGAL::Modifier_base<typename Polyhedron_3::HalfedgeDS>
{
	using FT = typename Polyhedron_3::Traits::Kernel::FT;
	using HDS = typename Polyhedron_3::HalfedgeDS;
	using Point_3 = typename Polyhedron_3::Traits::Kernel::Point_3;

public:
	/** Build mesh from vertices and indices.
	 *
	 */
	TriMeshBuilder(const std::vector<FT>& vertices, const std::vector<unsigned>& indices);


	/** Build mesh from vertices and indices.
	 *
	 */
	TriMeshBuilder(const std::vector<Point_3>& vertices, const std::vector<unsigned>& indices);


	void operator()(HDS& hds);

private:
	std::vector<FT> _vertices;
	std::vector<unsigned> _indices;
};

/** @}*/
} // namespace Euclid

#include "src/Polyhedron_3.cpp"
