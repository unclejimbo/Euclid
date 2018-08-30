#include <cmath>
#include <vector>

#include <CGAL/Min_sphere_of_spheres_d.h>
#include <CGAL/Plane_3.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Math/Vector.h>
#include <Euclid/Render/RayTracer.h>

namespace Euclid
{

template<typename Mesh, typename T>
void vs_view_entropy(const Mesh& mesh,
                     const ViewSphere<Mesh>& view_sphere,
                     std::vector<T>& view_scores)
{
    using VPMap =
        typename boost::property_map<Mesh, boost::vertex_point_t>::type;
    using Point_3 = typename boost::property_traits<VPMap>::value_type;
    using Kernel = typename CGAL::Kernel_traits<Point_3>::Kernel;

    constexpr const int width = 256;
    constexpr const int height = 256;
    constexpr const int size = width * height;

    auto vs_vpmap = get(boost::vertex_point, view_sphere.mesh);
    Eigen::Vector3f center;
    cgal_to_eigen(view_sphere.center, center);
    float extent = 1.5f * view_sphere.radius;

    std::vector<float> positions;
    std::vector<unsigned> indices;
    extract_mesh<3>(mesh, positions, indices);
    positions.push_back(0.0f); // Embree alignment
    RayTracer raytracer;
    raytracer.attach_geometry_buffers(positions, indices);

    for (const auto& v : vertices(view_sphere.mesh)) {
        std::vector<uint32_t> findices(size);
        Eigen::Vector3f view;
        cgal_to_eigen(get(vs_vpmap, v), view);
        CGAL::Plane_3<Kernel> tangent_plane(
            get(vs_vpmap, v), get(vs_vpmap, v) - view_sphere.center);
        Eigen::Vector3f up;
        cgal_to_eigen(tangent_plane.base1(), up);
        // this algorithm only works with orthogonal projection
        OrthoRayCamera camera(view, center, up, extent, extent);
        raytracer.render_index(findices, camera, width, height);

        // counting pixels belonging to each face, including background
        std::vector<int> proj_areas(num_faces(mesh) + 1, 0);
        for (auto i : findices) {
            ++proj_areas[i];
        }

        // computing view entropy
        auto entropy = static_cast<T>(0.0);
        for (auto a : proj_areas) {
            if (a != 0) {
                auto p = static_cast<T>(a) / size;
                entropy += -p * std::log(p);
            }
        }
        view_scores.push_back(entropy);
    }
}

} // namespace Euclid
