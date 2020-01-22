#include <catch2/catch.hpp>
#include <Euclid/BoundingVolume/AABB.h>

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

TEST_CASE("Analysis, AABB", "[analysis][aabb]")
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
        auto aabb = Euclid::AABB<Kernel>(positions);

        REQUIRE(aabb.center() == Point_3(1.5f, 1.0f, 0.5f));
        REQUIRE(aabb.xmin() == 0.0f);
        REQUIRE(aabb.xmax() == 3.0f);
        REQUIRE(aabb.xlen() == 3.0f);
        REQUIRE(aabb.ymin() == 0.0f);
        REQUIRE(aabb.ymax() == 2.0f);
        REQUIRE(aabb.ylen() == 2.0f);
        REQUIRE(aabb.zmin() == 0.0f);
        REQUIRE(aabb.zmax() == 1.0f);
        REQUIRE(aabb.zlen() == 1.0f);
        REQUIRE(aabb.point(0, 1, 0) == Point_3(0.0f, 2.0f, 0.0f));
    }

    SECTION("vector of points")
    {
        std::vector<Point_3> points;
        for (size_t i = 0; i < positions.size(); i += 3) {
            points.emplace_back(
                positions[i], positions[i + 1], positions[i + 2]);
        }
        auto aabb = Euclid::AABB<Kernel>(points);

        REQUIRE(aabb.center() == Point_3(1.5f, 1.0f, 0.5f));
        REQUIRE(aabb.xmin() == 0.0f);
        REQUIRE(aabb.xmax() == 3.0f);
        REQUIRE(aabb.xlen() == 3.0f);
        REQUIRE(aabb.ymin() == 0.0f);
        REQUIRE(aabb.ymax() == 2.0f);
        REQUIRE(aabb.ylen() == 2.0f);
        REQUIRE(aabb.zmin() == 0.0f);
        REQUIRE(aabb.zmax() == 1.0f);
        REQUIRE(aabb.zlen() == 1.0f);
        REQUIRE(aabb.point(0, 1, 0) == Point_3(0.0f, 2.0f, 0.0f));
    }

    SECTION("mesh")
    {
        Surface_mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);
        auto aabb = Euclid::AABB<Kernel>(
            mesh.vertices_begin(), mesh.vertices_end(), mesh.points());

        REQUIRE(aabb.center() == Point_3(1.5f, 1.0f, 0.5f));
        REQUIRE(aabb.xmin() == 0.0f);
        REQUIRE(aabb.xmax() == 3.0f);
        REQUIRE(aabb.xlen() == 3.0f);
        REQUIRE(aabb.ymin() == 0.0f);
        REQUIRE(aabb.ymax() == 2.0f);
        REQUIRE(aabb.ylen() == 2.0f);
        REQUIRE(aabb.zmin() == 0.0f);
        REQUIRE(aabb.zmax() == 1.0f);
        REQUIRE(aabb.zlen() == 1.0f);
        REQUIRE(aabb.point(0, 1, 0) == Point_3(0.0f, 2.0f, 0.0f));
    }

    SECTION("point_set")
    {
        Point_set_3 point_set;
        for (size_t i = 0; i < positions.size(); i += 3) {
            point_set.insert(
                Point_3(positions[i], positions[i + 1], positions[i + 2]));
        }
        auto aabb = Euclid::AABB<Kernel>(
            point_set.begin(), point_set.end(), point_set.point_map());

        REQUIRE(aabb.center() == Point_3(1.5f, 1.0f, 0.5f));
        REQUIRE(aabb.xmin() == 0.0f);
        REQUIRE(aabb.xmax() == 3.0f);
        REQUIRE(aabb.xlen() == 3.0f);
        REQUIRE(aabb.ymin() == 0.0f);
        REQUIRE(aabb.ymax() == 2.0f);
        REQUIRE(aabb.ylen() == 2.0f);
        REQUIRE(aabb.zmin() == 0.0f);
        REQUIRE(aabb.zmax() == 1.0f);
        REQUIRE(aabb.zlen() == 1.0f);
        REQUIRE(aabb.point(0, 1, 0) == Point_3(0.0f, 2.0f, 0.0f));
    }
}
