#include <algorithm>
#include <array>
#define _USE_MATH_DEFINES
#include <cmath>

#include <CGAL/Min_sphere_of_spheres_d.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/convex_hull_3.h>
#include <Euclid/Analysis/OBB.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Geometry/MeshProperties.h>
#include <Euclid/Geometry/PrimitiveGenerator.h>
#include <Euclid/Math/Vector.h>
#include <Euclid/Render/RayTracer.h>

namespace Euclid
{

template<typename Mesh>
ViewSphere<Mesh> ViewSphere<Mesh>::make_subdiv(const Mesh& mesh,
                                               float scale,
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
    result.radius = ms.radius() * scale;
    auto iter = ms.center_cartesian_begin();
    result.center = Point_3(*iter, *(iter + 1), *(iter + 2));

    Euclid::make_subdivision_sphere(
        result.mesh, result.center, result.radius, subdiv);

    return result;
}

template<typename Mesh>
ViewSphere<Mesh> ViewSphere<Mesh>::make_random(const Mesh& mesh,
                                               float scale,
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

template<typename Mesh, typename T>
void proxy_view_selection(const Mesh& mesh,
                          const ViewSphere<Mesh>& view_sphere,
                          std::vector<T>& view_scores,
                          float weight)
{
    using Point_3 = typename boost::property_traits<
        typename boost::property_map<Mesh,
                                     boost::vertex_point_t>::type>::value_type;
    using Kernel = typename CGAL::Kernel_traits<Point_3>::Kernel;
    const int proxies = 6;
    const int width = 256;
    const int height = 256;
    const float least_visible = std::cos(M_PI / 3.0f);
    const float w1 = weight;    // weight for projected areas
    const float w2 = 1.0f - w1; // weight for visible ratios

    auto [vbeg, vend] = vertices(mesh);
    auto mesh_vpmap = get(boost::vertex_point, mesh);
    OBB<Kernel> obb(vbeg, vend, mesh_vpmap);

    // Compute projected area
    std::vector<float> positions;
    std::vector<unsigned> indices;
    extract_mesh<3>(mesh, positions, indices);
    positions.push_back(0.0f); // Embree alignment
    RayTracer raytracer;
    raytracer.attach_geometry_shared(positions, indices);

    std::vector<float> projected_areas(proxies);
    for (size_t i = 0; i < projected_areas.size(); ++i) {
        OrthogonalCamera cam;
        auto view_dir = view_sphere.radius * obb.axis(i % 3);
        if (i >= 3) { view_dir = -view_dir; }
        cam.lookat(cgal_to_eigen<float>(view_sphere.center + view_dir),
                   cgal_to_eigen<float>(view_sphere.center),
                   cgal_to_eigen<float>(obb.axis((i + 1) % 3)));
        cam.set_extent(view_sphere.radius * 2.0f, view_sphere.radius * 2.0f);

        std::vector<unsigned char> pixels(width * height, 0);
        raytracer.render_silhouette(pixels.data(), cam, width, height);
        auto proj = 0;
        for (size_t j = 0; j < pixels.size(); ++j) {
            if (pixels[j] != 0) { ++proj; }
        }
        projected_areas[i] = static_cast<float>(proj) / pixels.size();
    }
    auto max_proj_area =
        *std::max_element(projected_areas.begin(), projected_areas.end());
    for (auto& proj_area : projected_areas) {
        proj_area /= max_proj_area;
    }

    // Compute visible ratio
    std::vector<float> visible_ratios(proxies);
    std::vector<int> n_visible_facets(proxies, 0);
    for (const auto& f : faces(mesh)) {
        auto normal = face_normal(f, mesh);
        if (normal * obb.axis(0) > least_visible) { ++n_visible_facets[0]; }
        if (normal * obb.axis(1) > least_visible) { ++n_visible_facets[1]; }
        if (normal * obb.axis(2) > least_visible) { ++n_visible_facets[2]; }
        if (-normal * obb.axis(0) > least_visible) { ++n_visible_facets[3]; }
        if (-normal * obb.axis(1) > least_visible) { ++n_visible_facets[4]; }
        if (-normal * obb.axis(2) > least_visible) { ++n_visible_facets[5]; }
    }
    float inv_nf = 1.0f / num_faces(mesh);
    std::transform(n_visible_facets.begin(),
                   n_visible_facets.end(),
                   visible_ratios.begin(),
                   [inv_nf](int nf) { return nf * inv_nf; });
    auto max_visible_ratio =
        *std::max_element(visible_ratios.begin(), visible_ratios.end());
    for (auto& visible_ratio : visible_ratios) {
        visible_ratio /= max_visible_ratio;
    }

    // Compute final score
    view_scores.clear();
    view_scores.resize(num_vertices(view_sphere.mesh), 0.0f);
    auto sphere_vpmap = get(boost::vertex_point, view_sphere.mesh);
    auto sphere_vimap = get(boost::vertex_index, view_sphere.mesh);
    for (const auto& v : vertices(view_sphere.mesh)) {
        auto view_dir = normalized(sphere_vpmap[v] - obb.center());
        auto i = sphere_vimap[v];
        view_scores[i] += (w1 * projected_areas[0] + w2 * visible_ratios[0]) *
                          std::max(view_dir * obb.axis(0), 0.0f);
        view_scores[i] += (w1 * projected_areas[1] + w2 * visible_ratios[1]) *
                          std::max(view_dir * obb.axis(1), 0.0f);
        view_scores[i] += (w1 * projected_areas[2] + w2 * visible_ratios[2]) *
                          std::max(view_dir * obb.axis(2), 0.0f);
        view_scores[i] += (w1 * projected_areas[3] + w2 * visible_ratios[3]) *
                          std::max(-view_dir * obb.axis(0), 0.0f);
        view_scores[i] += (w1 * projected_areas[4] + w2 * visible_ratios[4]) *
                          std::max(-view_dir * obb.axis(1), 0.0f);
        view_scores[i] += (w1 * projected_areas[5] + w2 * visible_ratios[5]) *
                          std::max(-view_dir * obb.axis(2), 0.0f);
    }
}

} // namespace Euclid
