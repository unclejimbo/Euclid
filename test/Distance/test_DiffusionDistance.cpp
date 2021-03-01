#include <catch2/catch.hpp>
#include <Euclid/Distance/DiffusionDistance.h>

#include <vector>
#include <string>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/Geometry/Spectral.h>
#include <Euclid/MeshUtil/CGALMesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Util/Color.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

TEST_CASE("Distance, Diffusion distance", "[distance][diffusiondistance]")
{
    std::vector<float> positions;
    std::vector<unsigned> indices;
    std::string fin(DATA_DIR);
    fin.append("kitten.off");
    Euclid::read_off<3>(fin, positions, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    constexpr const int v1 = 2438;
    constexpr const int v2 = 1482;
    constexpr const int v3 = 946;
    constexpr const double t1 = 0.1;
    constexpr const double t2 = 0.2;
    constexpr const int k = 200;

    // Compute spectral decomposition
    Eigen::VectorXd lambdas;
    Eigen::MatrixXd phis;
    Euclid::spectrum(mesh, k, lambdas, phis);

    // Compute distances
    auto d1 = Euclid::diffusion_distance(lambdas, phis, v1, v2, t1);
    auto d2 = Euclid::diffusion_distance(lambdas, phis, v1, v3, t1);
    auto d3 = Euclid::diffusion_distance(lambdas, phis, v1, v3, t2);
    REQUIRE(d1 < d2);
    REQUIRE(d2 > d3);

    // Colormap
    std::vector<double> distances(mesh.num_vertices());
    for (uint32_t i = 0; i < mesh.num_vertices(); ++i) {
        distances[i] = Euclid::diffusion_distance(lambdas, phis, v1, i, t1);
    }
    std::vector<unsigned char> colors;
    Euclid::colormap(igl::COLOR_MAP_TYPE_PARULA, distances, colors, true, true);
    std::string fout(TMP_DIR);
    fout.append("kitten_diffusion_distance.ply");
    Euclid::write_ply<3>(fout, positions, nullptr, nullptr, &indices, &colors);
}
