#include <Euclid/Analysis/OBB.h>
#include <catch.hpp>

#include <vector>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Point_set_3.h> // CGAL's bug, must be included after a kernel
#include <CGAL/Surface_mesh.h>
#include <Euclid/Geometry/MeshHelpers.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = typename Kernel::Point_3;
using Vector_3 = typename Kernel::Vector_3;
using Surface_mesh = CGAL::Surface_mesh<Point_3>;
using Point_set_3 = CGAL::Point_set_3<Point_3>;

TEST_CASE("Analysis, OBB", "[analysis][obb]")
{
    const std::vector<float> positions{ 0.0f, 0.0f, 0.0f, 3.0f, 0.0f, 0.0f,
                                        3.0f, 2.0f, 0.0f, 0.0f, 2.0f, 0.0f,
                                        0.0f, 0.0f, 1.0f, 3.0f, 0.0f, 1.0f,
                                        3.0f, 2.0f, 1.0f, 0.0f, 2.0f, 1.0f };
    const std::vector<int> indices{ 0, 1, 2, 0, 2, 3, 0, 3, 4, 0, 4, 7,
                                    1, 0, 4, 1, 4, 5, 3, 2, 6, 3, 6, 7,
                                    2, 1, 5, 2, 5, 6, 7, 6, 5, 7, 5, 4 };

    SECTION("raw positions")
    {
        auto obb = Euclid::OBB<Kernel>(positions);

        REQUIRE(obb.center() == Point_3(1.5f, 1.0f, 0.5f));
        REQUIRE(obb.axis(0) == Vector_3(1.0f, 0.0f, 0.0f));
        REQUIRE(obb.axis(1) == Vector_3(0.0f, 1.0f, 0.0f));
        REQUIRE(obb.axis(2) == Vector_3(0.0f, 0.0f, 1.0f));
        REQUIRE(obb.length(0) == 3.0);
        REQUIRE(obb.length(1) == 2.0);
        REQUIRE(obb.length(2) == 1.0);
    }

    SECTION("vector of points")
    {
        std::vector<Point_3> points;
        for (size_t i = 0; i < positions.size(); i += 3) {
            points.emplace_back(
                positions[i], positions[i + 1], positions[i + 2]);
        }
        auto obb = Euclid::OBB<Kernel>(points);

        REQUIRE(obb.center() == Point_3(1.5f, 1.0f, 0.5f));
        REQUIRE(obb.axis(0) == Vector_3(1.0f, 0.0f, 0.0f));
        REQUIRE(obb.axis(1) == Vector_3(0.0f, 1.0f, 0.0f));
        REQUIRE(obb.axis(2) == Vector_3(0.0f, 0.0f, 1.0f));
        REQUIRE(obb.length(0) == 3.0);
        REQUIRE(obb.length(1) == 2.0);
        REQUIRE(obb.length(2) == 1.0);
    }

    SECTION("mesh")
    {
        Surface_mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);
        auto obb = Euclid::OBB<Kernel>(
            mesh.vertices_begin(), mesh.vertices_end(), mesh.points());

        REQUIRE(obb.center() == Point_3(1.5f, 1.0f, 0.5f));
        REQUIRE(obb.axis(0) == Vector_3(1.0f, 0.0f, 0.0f));
        REQUIRE(obb.axis(1) == Vector_3(0.0f, 1.0f, 0.0f));
        REQUIRE(obb.axis(2) == Vector_3(0.0f, 0.0f, 1.0f));
        REQUIRE(obb.length(0) == 3.0);
        REQUIRE(obb.length(1) == 2.0);
        REQUIRE(obb.length(2) == 1.0);
    }

    SECTION("point_set")
    {
        Point_set_3 point_set;
        for (size_t i = 0; i < positions.size(); i += 3) {
            point_set.insert(
                Point_3(positions[i], positions[i + 1], positions[i + 2]));
        }
        auto obb = Euclid::OBB<Kernel>(
            point_set.begin(), point_set.end(), point_set.point_map());

        REQUIRE(obb.center() == Point_3(1.5f, 1.0f, 0.5f));
        REQUIRE(obb.axis(0) == Vector_3(1.0f, 0.0f, 0.0f));
        REQUIRE(obb.axis(1) == Vector_3(0.0f, 1.0f, 0.0f));
        REQUIRE(obb.axis(2) == Vector_3(0.0f, 0.0f, 1.0f));
        REQUIRE(obb.length(0) == 3.0);
        REQUIRE(obb.length(1) == 2.0);
        REQUIRE(obb.length(2) == 1.0);
    }
}
