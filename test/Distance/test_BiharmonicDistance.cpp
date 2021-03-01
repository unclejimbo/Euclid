#include <catch2/catch.hpp>
#include <Euclid/Distance/BiharmonicDistance.h>

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

TEST_CASE("Distance, Biharmonic distance", "[distance][biharmonicdistance]")
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
    constexpr const int k = 200;

    // Compute spectral decomposition
    Eigen::VectorXd lambdas;
    Eigen::MatrixXd phis;
    Euclid::spectrum(mesh, k, lambdas, phis);

    // Compute distances
    auto d1 = Euclid::biharmonic_distance(lambdas, phis, v1, v2);
    auto d2 = Euclid::biharmonic_distance(lambdas, phis, v1, v3);
    REQUIRE(d1 < d2);

    // Colormap
    std::vector<double> distances(mesh.num_vertices());
    for (uint32_t i = 0; i < mesh.num_vertices(); ++i) {
        distances[i] = Euclid::biharmonic_distance(lambdas, phis, v1, i);
    }
    std::vector<unsigned char> colors;
    Euclid::colormap(igl::COLOR_MAP_TYPE_PARULA, distances, colors, true, true);
    std::string fout(TMP_DIR);
    fout.append("kitten_biharmonic_distance.ply");
    Euclid::write_ply<3>(fout, positions, nullptr, nullptr, &indices, &colors);
}
