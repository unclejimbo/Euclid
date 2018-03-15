#include <catch.hpp>
#include <Euclid/Geometry/MeshBuilder.h>

#include <string>
#include <vector>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <Euclid/IO/OffIO.h>

#include "../config.h"

using Kernel = CGAL::Simple_cartesian<double>;
using Point_3 = typename Kernel::Point_3;

TEST_CASE("Package: Geometry/MeshBuilder", "[meshbuilder]")
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

    SECTION("Build a CGAL::Surface_mesh")
    {
        using Mesh = CGAL::Surface_mesh<Point_3>;
        Mesh mesh;
        Euclid::build_mesh<3>(mesh, positions, indices);
        REQUIRE(mesh.number_of_vertices() * 3 == positions.size());
        REQUIRE(mesh.number_of_faces() * 3 == indices.size());
        REQUIRE(mesh.point(Mesh::Vertex_index(0)).x() == positions[0]);
        REQUIRE(mesh.point(Mesh::Vertex_index(0)).y() == positions[1]);
        REQUIRE(mesh.point(Mesh::Vertex_index(0)).z() == positions[2]);
    }

    SECTION("Build a CGAL::Surface_mesh")
    {
        using Mesh = CGAL::Surface_mesh<Point_3>;
        Mesh mesh;
        Euclid::build_mesh<3>(mesh, points, indices);
        REQUIRE(mesh.number_of_vertices() * 3 == positions.size());
        REQUIRE(mesh.number_of_faces() * 3 == indices.size());
        REQUIRE(mesh.point(Mesh::Vertex_index(0)).x() == positions[0]);
        REQUIRE(mesh.point(Mesh::Vertex_index(0)).y() == positions[1]);
        REQUIRE(mesh.point(Mesh::Vertex_index(0)).z() == positions[2]);
    }

    SECTION("Build a CGAL::Polyhedron_3")
    {
        using Mesh = CGAL::Polyhedron_3<Kernel>;
        Mesh mesh;
        Euclid::build_mesh<3>(mesh, positions, indices);
        REQUIRE(mesh.size_of_vertices() * 3 == positions.size());
        REQUIRE(mesh.size_of_facets() * 3 == indices.size());
        REQUIRE(mesh.points_begin()->x() == positions[0]);
        REQUIRE(mesh.points_begin()->y() == positions[1]);
        REQUIRE(mesh.points_begin()->z() == positions[2]);
    }

    SECTION("Build a CGAL::Polyhedron_3")
    {
        using Mesh = CGAL::Polyhedron_3<Kernel>;
        Mesh mesh;
        Euclid::build_mesh<3>(mesh, points, indices);
        REQUIRE(mesh.size_of_vertices() * 3 == positions.size());
        REQUIRE(mesh.size_of_facets() * 3 == indices.size());
        REQUIRE(mesh.points_begin()->x() == positions[0]);
        REQUIRE(mesh.points_begin()->y() == positions[1]);
        REQUIRE(mesh.points_begin()->z() == positions[2]);
    }
}
