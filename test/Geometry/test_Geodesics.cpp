#include <catch2/catch.hpp>
#include <Euclid/Geometry/Geodesics.h>

#include <algorithm>
#include <vector>
#include <string>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Math/Numeric.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Util/Color.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

TEST_CASE("Geometry, Geodesics", "[geometry][geodesics]")
{
    SECTION("geodesics in heat")
    {
        // Read triangle mesh into buffers
        std::vector<float> positions;
        std::vector<unsigned> indices;
        std::string fin(DATA_DIR);
        fin.append("kitten.off");
        Euclid::read_off<3>(fin, positions, indices);

        // Generate a CGAL::Surface_mesh
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);

        // Construct the method
        Euclid::GeodesicsInHeat<Mesh> heat_method;
        heat_method.build(mesh, 4.0f);

        // Compute geodesics
        std::vector<double> geodesics;
        heat_method.compute(Mesh::Vertex_index(0), geodesics);
        REQUIRE(geodesics[0] == 0.0);
        auto gmax1 = *std::max_element(geodesics.begin(), geodesics.end());

        // Change the scale
        heat_method.scale(5.0f);
        heat_method.compute(Mesh::Vertex_index(0), geodesics);
        auto gmax2 = *std::max_element(geodesics.begin(), geodesics.end());
        REQUIRE(Euclid::eq_abs_err(gmax1, gmax2, 1.0));

        // Turn geodesic distances into colors and output to a file
        std::vector<unsigned char> colors;
        Euclid::colormap(
            igl::COLOR_MAP_TYPE_PARULA, geodesics, colors, true, true);

        std::string fout(TMP_DIR);
        fout.append("kitten_geodesics_heat.ply");
        Euclid::write_ply<3>(
            fout, positions, nullptr, nullptr, &indices, &colors);
    }
}
