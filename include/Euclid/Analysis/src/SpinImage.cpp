#include <tuple>

#define _USE_MATH_DEFINES
#include <cmath>

#include <Euclid/Geometry/MeshProperties.h>
#include <Euclid/Math/Vector.h>
#include <Euclid/Math/Transformation.h>

namespace Euclid
{

template<typename Mesh, typename VertexNormalMap, typename T>
void spin_image(const Mesh& mesh,
                const VertexNormalMap& vnmap,
                const typename boost::graph_traits<Mesh>::vertex_descriptor& v,
                std::vector<T>& spin_img,
                float bin_size,
                int image_width,
                float support_angle)
{
    using VPMap =
        typename boost::property_map<Mesh, boost::vertex_point_t>::type;
    using Point_3 = typename boost::property_traits<VPMap>::value_type;
    using Kernel = typename CGAL::Kernel_traits<Point_3>::Kernel;
    using FT = typename Kernel::FT;
    auto vpmap = get(boost::vertex_point, mesh);
    const auto to_radian = M_PI / 180.0f;

    if (support_angle < 0.0f || support_angle > 180.0f) {
        throw std::invalid_argument(
            "Support angle should be in range [0, 180]");
    }

    // Calculate the resolution of mesh
    FT median = 0.0;
    for (const auto& e : edges(mesh)) {
        median += edge_length(e, mesh);
    }
    median /= static_cast<FT>(num_edges(mesh));
    auto bin_width = median * static_cast<FT>(bin_size);

    // Transform the coordinate system so that vi is at origin and the vertex
    // normal points in the y axis, while the x and z axes are arbitrary
    const auto& pi = vpmap[v];
    const auto& ni = vnmap[v];
    CGAL::Plane_3<Kernel> plane(pi, ni);
    auto tangent = normalized(plane.base1());
    auto transform =
        transform_from_world_coord<Kernel>(pi, pi + tangent, pi + ni);

    // Find all vertices that lie in the support and compute the spin image
    spin_img.resize((image_width + 1) * (image_width + 1), 0.0);
    for (const auto& vj : vertices(mesh)) {
        if (ni * vnmap[vj] <= std::cos(support_angle * to_radian)) { continue; }

        auto pj = transform(vpmap[vj]);

        auto alpha = std::sqrt(pj.x() * pj.x() + pj.z() * pj.z()) / bin_width;
        auto col = static_cast<int>(std::floor(alpha));
        if (col >= image_width) { continue; }
        alpha -= col;

        auto beta = pj.y() / bin_width;
        auto beta_max = image_width * 0.5f;
        auto row = static_cast<int>(std::floor(beta_max - beta));
        if (row >= image_width || row < 0) { continue; }
        beta = beta_max - beta - row;

        // Bilinear interpolation
        spin_img[row * image_width + col] += (1.0f - alpha) * (1.0f - beta);
        spin_img[row * image_width + col + 1] += alpha * (1.0f - beta);
        spin_img[(row + 1) * image_width + col] += (1.0f - alpha) * beta;
        spin_img[(row + 1) * image_width + col + 1] += alpha * beta;
    }
}

} // namespace Euclid

#undef _USE_MATH_DEFINES
