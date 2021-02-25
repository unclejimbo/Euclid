#include <catch2/catch.hpp>
#include <Euclid/MeshUtil/MeshHelpers.h>

#include <string>
#include <vector>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <CGAL/Polyhedron_items_with_id_3.h>
#include <Euclid/IO/OffIO.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_3 = typename Kernel::Point_3;

TEST_CASE("MeshUtil, MeshHelpers", "[meshutil][meshhelpers]")
{
    std::vector<double> positions;
    std::vector<unsigned> indices;
    std::string file_name(DATA_DIR);
    file_name.append("bunny.off");
    Euclid::read_off<3>(file_name, positions, nullptr, &indices, nullptr);

    std::vector<Point_3> points;
    for (size_t i = 0; i < positions.size(); i += 3) {
        points.emplace_back(positions[i], positions[i + 1], positions[i + 2]);
    }

    SECTION("CGAL::Surface_mesh with raw positions")
    {
        using Mesh = CGAL::Surface_mesh<Point_3>;
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);
        std::vector<double> new_positions;
        std::vector<unsigned> new_indices;
        Euclid::extract_mesh<3>(mesh, new_positions, new_indices);

        REQUIRE(new_positions == positions);
        REQUIRE(new_indices == indices);
    }

    SECTION("CGAL::Surface_mesh with points")
    {
        using Mesh = CGAL::Surface_mesh<Point_3>;
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, points, indices);
        std::vector<Point_3> new_points;
        std::vector<unsigned> new_indices;
        Euclid::extract_mesh<3>(mesh, new_points, new_indices);

        REQUIRE(new_points == points);
        REQUIRE(new_indices == indices);
    }

    SECTION("CGAL::Polyhedron_3 with raw positions")
    {
        using Mesh =
            CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3>;
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);
        CGAL::set_halfedgeds_items_id(mesh);
        std::vector<double> new_positions;
        std::vector<unsigned> new_indices;
        Euclid::extract_mesh<3>(mesh, new_positions, new_indices);

        REQUIRE(new_positions == positions);
        REQUIRE(new_indices == indices);
    }

    SECTION("CGAL::Polyhedron_3 with points")
    {
        using Mesh =
            CGAL::Polyhedron_3<Kernel, CGAL::Polyhedron_items_with_id_3>;
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, points, indices);
        CGAL::set_halfedgeds_items_id(mesh);
        std::vector<Point_3> new_points;
        std::vector<unsigned> new_indices;
        Euclid::extract_mesh<3>(mesh, new_points, new_indices);

        REQUIRE(new_points == points);
        REQUIRE(new_indices == indices);
    }

    SECTION("Eigen::Matrix")
    {
        Eigen::MatrixXd V;
        Eigen::MatrixXi F;
        Euclid::make_mesh<3>(V, F, positions, indices);

        std::vector<double> new_positions;
        std::vector<unsigned> new_indices;
        Euclid::extract_mesh<3>(V, F, new_positions, new_indices);
        REQUIRE(new_positions == positions);
        REQUIRE(new_indices == indices);

        std::vector<double> only_positions;
        Euclid::extract_mesh(V, only_positions);
        REQUIRE(only_positions == positions);
    }

    SECTION("n-ring")
    {
        using Mesh = CGAL::Surface_mesh<Point_3>;
        using Vertex = typename Mesh::Vertex_index;
        Vertex target(410);
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);

        const std::unordered_set<Vertex> onering{ Vertex(40),   Vertex(1420),
                                                  Vertex(1429), Vertex(153),
                                                  Vertex(1209), Vertex(322) };
        const std::unordered_set<Vertex> tworing{
            Vertex(40),   Vertex(1420), Vertex(1429), Vertex(153),
            Vertex(1209), Vertex(322),  Vertex(131),  Vertex(1365),
            Vertex(8),    Vertex(1325), Vertex(101),  Vertex(1400),
            Vertex(381),  Vertex(469),  Vertex(1435), Vertex(1351)
        };

        auto vertices0 = Euclid::nring_vertices(target, mesh, 0);
        REQUIRE(vertices0.empty());

        auto vertices1 = Euclid::nring_vertices(target, mesh, 1);
        REQUIRE(vertices1.size() == onering.size());
        for (auto v : vertices1) {
            REQUIRE(onering.find(v) != onering.end());
        }

        auto vertices2 = Euclid::nring_vertices(target, mesh, 2);
        REQUIRE(vertices2.size() == tworing.size());
        for (auto v : vertices2) {
            REQUIRE(tworing.find(v) != tworing.end());
        }
    }

    SECTION("common edge")
    {
        using Mesh = CGAL::Surface_mesh<Point_3>;
        using Halfedge = Mesh::Halfedge_index;
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);
        Halfedge h1(0);
        Halfedge h2(1);
        Halfedge h3(100);
        auto f1 = mesh.face(h1);
        auto f2 = mesh.face(h2);
        auto f3 = mesh.face(h3);

        auto [ha, hb] = Euclid::find_common_edge(mesh, f1, f2);
        REQUIRE(ha == h1);
        REQUIRE(hb == h2);

        auto [hc, hd] = Euclid::find_common_edge(mesh, f1, f3);
        REQUIRE(!hc.is_valid());
        REQUIRE(!hd.is_valid());
    }
}
