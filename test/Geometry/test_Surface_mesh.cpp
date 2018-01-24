#include "../config.h"
#include <CGAL/Simple_cartesian.h>
#include <Euclid/Geometry/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <algorithm>
#include <catch.hpp>
#include <string>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = typename Kernel::Point_3;

TEST_CASE("Package: Geometry/Surface_mesh", "[suface_mesh]")
{
    std::vector<double> positions;
    std::vector<unsigned> indices;
    std::string file_name(DATA_DIR);
    file_name.append("chair.off");
    Euclid::read_off<3>(file_name, positions, indices);

    std::vector<Point_3> points;
    for (size_t i = 0; i < positions.size(); i += 3) {
        points.emplace_back(positions[i], positions[i + 1], positions[i + 2]);
    }

    SECTION("build_surface_mesh from positions")
    {
        CGAL::Surface_mesh<Point_3> mesh;
        REQUIRE(Euclid::build_surface_mesh(mesh, positions, indices));
        REQUIRE(mesh.number_of_vertices() * 3 == positions.size());
        REQUIRE(mesh.number_of_faces() * 3 == indices.size());
    }

    SECTION("build_surface_mesh from points")
    {
        CGAL::Surface_mesh<Point_3> mesh;
        REQUIRE(Euclid::build_surface_mesh(mesh, points, indices));
        REQUIRE(mesh.number_of_vertices() * 3 == positions.size());
        REQUIRE(mesh.number_of_faces() * 3 == indices.size());
    }
}
