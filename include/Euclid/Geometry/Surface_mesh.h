/** CGAL::Surface_mesh utilities.
 *
 *  This package contains utilities to work with the
 *  CGAL::Surface_mesh package.
 *  @defgroup PkgSurfaceMesh Surface_mesh
 *  @ingroup PkgGeometry
 */
#pragma once

#include <vector>

#include <CGAL/Surface_mesh.h>
#include <boost/property_map/property_map.hpp>

namespace Euclid
{
/** @{*/

/** Build Surface_mesh from vertices and indices.
 *
 *  @tparam Point_3 A CGAL::Point_3.
 *  @tparam FT A floating point type for position.
 *  @tparam IT An integral type for index.
 *  @param[out] mesh The mesh to be built.
 *  @param[in] vertices A vector of x,y,z coordinates.
 *  @param[in] indices A vector indices.
 *  @return Whether the mesh is constructed successfully.
 */
template<typename Point_3, typename FT, typename IT>
bool build_surface_mesh(CGAL::Surface_mesh<Point_3>& mesh,
                        const std::vector<FT>& vertices,
                        const std::vector<IT>& indices);

/** Build Surface_mesh from vertices and indices.
 *
 *  @tparam Point_3 A CGAL::Point_3 type.
 *  @tparam IT An integral type for index.
 *  @param[out] mesh The mesh to be built.
 *  @param[in] vertices A vector of points.
 *  @param[in] indices A vector indices.
 *  @return Whether the mesh is constructed successfully.
 */
template<typename Point_3, typename IT>
bool build_surface_mesh(CGAL::Surface_mesh<Point_3>& mesh,
                        const std::vector<Point_3>& vertices,
                        const std::vector<IT>& indices);

/** Property map for Surface_mesh.
 *
 */
template<typename ValueType>
class SM_PropertyMap
{
public:
    using value_type = ValueType;
    using reference = const value_type&;
    using key_type = size_t;
    using category = boost::lvalue_property_map_tag;

    explicit SM_PropertyMap(const std::vector<value_type>& values)
        : _values(values)
    {
    }
    reference operator[](key_type key) const { return _values[key]; }

    friend reference get(SM_PropertyMap pmap, key_type key)
    {
        return pmap[key];
    }

private:
    const std::vector<value_type>& _values;
};

/** @}*/
} // namespace Euclid

#include "src/Surface_mesh.cpp"
