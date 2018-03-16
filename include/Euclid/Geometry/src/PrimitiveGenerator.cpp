#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Math/Vector.h>
#include <CGAL/subdivision_method_3.h>
#include <algorithm>

namespace Euclid
{

template<typename Mesh, typename Point_3>
void make_subdivision_sphere(
    Mesh& mesh,
    const Point_3& center,
    typename CGAL::Kernel_traits<Point_3>::Kernel::FT radius,
    int iterations)
{
    using FT = typename CGAL::Kernel_traits<Point_3>::Kernel::FT;

    CGAL::make_icosahedron(mesh, center, radius);
    CGAL::Subdivision_method_3::Loop_subdivision(mesh, iterations);

    // Subdivision could cause severe shrinkage,
    // so it needs to be scaled afterwards
    auto vpmap = get(boost::vertex_point, mesh);
    FT r = 0.0;
    for (const auto& v : vertices(mesh)) {
        auto p = vpmap[v];
        r += length(p - CGAL::ORIGIN);
    }
    r /= num_vertices(mesh);
    for (const auto& v : vertices(mesh)) {
        auto x = vpmap[v].x() * radius / r + center.x();
        auto y = vpmap[v].y() * radius / r + center.y();
        auto z = vpmap[v].z() * radius / r + center.z();
        vpmap[v] = Point_3(x, y, z);
    }
}

} // namespace Euclid
