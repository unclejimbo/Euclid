/** View selection.
 *
 *  View selection methods try to find a proper viewpoint to observe the shape.
 *  Within this package, the center of the view sphere is located at the center
 *  of the bounding box of the mesh, and the radius of the view sphere is equal
 *  to the length of diagonal length of the bounding box.
 *
 *  @defgroup PkgView View Selection
 *  @ingroup PkgAnalysis
 */
#pragma once

#include <vector>
#include <CGAL/boost/graph/properties.h>

namespace Euclid
{
/** @{*/

/** View sphere around a mesh.
 *
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
                                        float scale = 3.0f,
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
                                        float scale = 3.0f,
                                        int samples = 1000);

public:
    Mesh mesh;
    Point_3 center;
    FT radius;
};

/** View selection using view entropy.
 *
 *  View entropy uses the solid angle of each face as the probability of its
 *  visibility and computes entropy based on it. It encourages all faces to have
 *  the same projected area.
 *
 *  @param mesh The target mesh model.
 *  @param view_sphere The viewing sphere.
 *  @param view_scores The corresponding view scores.
 *
 *  **Reference**
 *
 *  Vazquez P, Feixas M, Sbert M, and Heidrich W.
 *  Viewpoint Selection using View Entropy.
 *  Proceedings of the Vision Modeling and Visualization Conference 2001.
 */
template<typename Mesh, typename T>
void vs_view_entropy(const Mesh& mesh,
                     const ViewSphere<Mesh>& view_sphere,
                     std::vector<T>& view_scores);

/** View selection using proxy information.
 *
 *  Using the object oriented bounding box as proxies to measure view saliency
 *  and interpolate results to other view samples, which is very fast.
 *
 *  @param mesh The target mesh model.
 *  @param view_sphere The viewing sphere.
 *  @param view_scores The corresponding view scores.
 *  @param weight The weighting of the projected area term, (1 - weight) is the
 *  weighting for the visible ratio term.
 *
 *  #### Reference
 *  Gao T, Wang W, Han H.
 *  Efficient view selection by measuring proxy information[J].
 *  Computer Animation and Virtual Worlds, 2016, 27(3-4): 351-357.
 */
template<typename Mesh, typename T>
void proxy_view_selection(const Mesh& mesh,
                          const ViewSphere<Mesh>& view_sphere,
                          std::vector<T>& view_scores,
                          float weight = 0.5f);

/** @}*/
} // namespace Euclid

#include "src/ViewSphere.cpp"
#include "src/ViewEntropy.cpp"
#include "src/ProxyViewSelection.cpp"
