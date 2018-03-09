#include <catch.hpp>
#include <Euclid/Geometry/Polyhedron_3.h>

#include <CGAL/Simple_cartesian.h>
#include <Euclid/IO/PlyIO.h>

#include "../config.h"

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = typename Kernel::Point_3;
using Polyhedron_3 = typename CGAL::Polyhedron_3<Kernel>;

TEST_CASE("Package: Geometry/test_Polyhedron_3", "[polyhedron_3]")
{
    std::vector<float> positions;
    std::vector<unsigned> indices;
    std::string file_name(DATA_DIR);
    file_name.append("cube_ascii.ply");
    Euclid::read_ply<3>(
        file_name, positions, nullptr, nullptr, &indices, nullptr);

    SECTION("Build Polyhedron_3 from positions")
    {
        Polyhedron_3 mesh;
        Euclid::build_polyhedron_3<3>(mesh, positions, indices);
        REQUIRE(mesh.size_of_vertices() * 3 == positions.size());
        REQUIRE(mesh.size_of_facets() * 3 == indices.size());
    }

    SECTION("Build Polyhedron_3 from points")
    {
        std::vector<Point_3> points;
        for (size_t i = 0; i < positions.size(); i += 3) {
            points.emplace_back(
                positions[i], positions[i + 1], positions[i + 2]);
        }
        CGAL::Polyhedron_3<Kernel> mesh;
        Euclid::build_polyhedron_3<3>(mesh, points, indices);
        REQUIRE(mesh.size_of_vertices() == points.size());
        REQUIRE(mesh.size_of_facets() * 3 == indices.size());
    }
}
