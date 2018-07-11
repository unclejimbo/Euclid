#include <algorithm>
#include <vector>

#include <CGAL/Min_sphere_of_spheres_d.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/convex_hull_3.h>
#include <Euclid/Geometry/PrimitiveGenerator.h>

namespace Euclid
{

template<typename Mesh>
ViewSphere<Mesh> ViewSphere<Mesh>::make_subdiv(const Mesh& mesh,
                                               FT scale,
                                               int subdiv)
{
    using MSTraits = CGAL::Min_sphere_of_spheres_d_traits_3<Kernel, FT>;
    using Min_sphere = CGAL::Min_sphere_of_spheres_d<MSTraits>;
    using Sphere = typename Min_sphere::Sphere;

    auto mesh_vpmap = get(boost::vertex_point, mesh);
    std::vector<Sphere> spheres;
    spheres.reserve(num_vertices(mesh));
    for (const auto& v : vertices(mesh)) {
        spheres.emplace_back(mesh_vpmap[v], 0.0f);
    }
    Min_sphere ms(spheres.begin(), spheres.end());

    ViewSphere<Mesh> result;
    result.scale = scale;
    result.radius = ms.radius() * scale;
    auto iter = ms.center_cartesian_begin();
    result.center = Point_3(*iter, *(iter + 1), *(iter + 2));

    make_subdivision_sphere(result.mesh, result.center, result.radius, subdiv);

    return result;
}

template<typename Mesh>
ViewSphere<Mesh> ViewSphere<Mesh>::make_random(const Mesh& mesh,
                                               FT scale,
                                               int samples)
{
    using MSTraits = CGAL::Min_sphere_of_spheres_d_traits_3<Kernel, FT>;
    using Min_sphere = CGAL::Min_sphere_of_spheres_d<MSTraits>;
    using Sphere = typename Min_sphere::Sphere;

    auto mesh_vpmap = get(boost::vertex_point, mesh);
    std::vector<Sphere> spheres;
    spheres.reserve(num_vertices(mesh));
    for (const auto& v : vertices(mesh)) {
        spheres.emplace_back(mesh_vpmap[v], 0.0f);
    }
    Min_sphere ms(spheres.begin(), spheres.end());

    ViewSphere<Mesh> result;
    result.scale = scale;
    result.radius = ms.radius() * scale;
    auto iter = ms.center_cartesian_begin();
    result.center = Point_3(*iter, *(iter + 1), *(iter + 2));

    auto generator = CGAL::Random_points_on_sphere_3<Point_3>(result.radius);
    std::vector<Point_3> points(samples);
    std::copy_n(generator, samples, points.begin());
    for (auto& p : points) {
        p = p + (result.center - CGAL::ORIGIN);
    }
    CGAL::convex_hull_3(points.begin(), points.end(), result.mesh);

    return result;
}

} // namespace Euclid
