#include <catch.hpp>
#include <Euclid/Math/Transformation.h>
#include <CGAL/Simple_cartesian.h>

TEST_CASE("Package: Math/Transformation", "[transformation]") {
    using Kernel = CGAL::Simple_cartesian<float>;
    using Point_3 = typename Kernel::Point_3;

    Point_3 o0(0.0f, 0.0f, 0.0f);
    Point_3 x0(1.0f, 0.0f, 0.0f);
    Point_3 y0(0.0f, 1.0f, 0.0f);
    Point_3 p(1.0f, 2.0f, 3.0f);

    SECTION("Function: transform_between_2_coorad_systems") {
        auto t1 = Euclid::transform_between_2_coord_systems<Kernel>(
            o0, x0, y0, o0, x0, y0);
        auto t2 = Euclid::transform_between_2_coord_systems<Kernel>(
            o0, x0, y0, o0, y0, x0);
        auto t3 = Euclid::transform_between_2_coord_systems<Kernel>(
            o0, y0, x0);
        REQUIRE_THROWS(Euclid::transform_between_2_coord_systems<Kernel>(x0, x0, x0, x0, x0, x0));
        REQUIRE(t1.transform(p) == p);
        REQUIRE(t2.transform(p) == Point_3(2.0f, 1.0f, -3.0f));
        REQUIRE(t3.transform(p) == Point_3(2.0f, 1.0f, -3.0f));
    }
}
