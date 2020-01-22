/** View selection.
 *
 *  View selection methods try to find a proper viewpoint to observe the shape.
 *
 *  @defgroup PkgView View Selection
 *  @ingroup PkgAnalysis
 */
#pragma once

#include <vector>
#include <CGAL/boost/graph/properties.h>

namespace Euclid
{
/**@{ @ingroup PkgViewSelection*/

/**View sphere around a mesh.
 *
 * Within this package, the center of the view sphere is located at the center
 * of the bounding box of the mesh, and the radius of the view sphere is equal
 * to the length of diagonal length of the bounding box.
 */
template<typename Mesh>
class ViewSphere
{
public:
    using Point_3 = typename boost::property_traits<
        typename boost::property_map<Mesh,
                                     boost::vertex_point_t>::type>::value_type;
    using Kernel = typename CGAL::Kernel_traits<Point_3>::Kernel;
    using FT = typename Kernel::FT;

public:
    /** Build a view sphere using subdivision.
     *
     *  The subdivision sphere is pretty regular, but you can't set a specific
     *  number of samples, but rather use the number of iterations of
     *  subdivision to control the coarseness of the sphere.
     *
     *  @param mesh The target mesh object.
     *  @param scale The scale of the view sphere's radius, by 1.0 the sphere
     *  equals to the minimum bounding sphere of the mesh.
     *  @param subdiv The iterations of subdivision.
     */
    static ViewSphere<Mesh> make_subdiv(const Mesh& mesh,
                                        FT scale = 3.0,
                                        int subdiv = 4);

    /** Build a view sphere using uniform distributed random points.
     *
     *  Randomly generating points uniformly on a sphere. A user can set the
     *  desired number of sample points, but the triangulated sphere is not
     *  as regular as a subdivision sphere.
     *
     *  @param mesh The target mesh object.
     *  @param scale The scale of the view sphere's radius, by 1.0 the sphere
     *  equals to the minimum bounding sphere of the mesh.
     *  @param samples The number of random sample points.
     */
    static ViewSphere<Mesh> make_random(const Mesh& mesh,
                                        FT scale = 3.0,
                                        int samples = 1000);

public:
    /** The mesh data structure of the sphere.
     *
     */
    Mesh mesh;

    /** The center of the sphere.
     *
     */
    Point_3 center;

    /** Radius of the sphere.
     *
     */
    FT radius;

    /** The scale of the view sphere's radius relative to the minimum bounding
     *  sphere of the target mesh.
     *
     */
    FT scale;
};

/** @}*/
} // namespace Euclid

#include "src/ViewSphere.cpp"
