#include <catch2/catch.hpp>
#include <Euclid/Math/Vector.h>

#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>
#include <CGAL/Simple_cartesian.h>

TEST_CASE("Math, Vector", "[math][vector]")
{
    using Kerneld = CGAL::Exact_predicates_inexact_constructions_kernel;
    using Vector_3d = typename Kerneld::Vector_3;
    using Point_3d = typename Kerneld::Point_3;
    using Kernelf = CGAL::Simple_cartesian<float>;
    using Vector_3f = typename Kernelf::Vector_3;
    using Point_3f = typename Kernelf::Point_3;
    const double sqrt2_2 = 0.70710678118;

    auto zerod = Vector_3d(0.0, 0.0, 0.0);
    auto oned = Vector_3d(1.0, 0.0, 0.0);
    auto zerof = Vector_3f(0.0f, 0.0f, 0.0f);
    auto onef = Vector_3f(1.0f, 0.0f, 0.0f);
    auto p1d = Point_3d(0.0, 0.0, 0.0);
    auto p2d = Point_3d(1.0, 0.0, 0.0);
    auto p3d = Point_3d(2.0, 0.0, 0.0);
    auto p4d = Point_3d(0.0, 2.0, 0.0);
    auto p5d = Point_3d(-1.0, 0.0, 0.0);
    auto p6d = Point_3d(1.0, 1.0, 0.0);
    auto p7d = Point_3d(-1.0, 1.0, 0.0);
    auto p1f = Point_3f(0.0f, 0.0f, 0.0f);
    auto p2f = Point_3f(1.0f, 0.0f, 0.0f);
    auto p3f = Point_3f(2.0f, 0.0f, 0.0f);
    auto p4f = Point_3f(0.0f, 2.0f, 0.0f);
    auto p5f = Point_3f(-1.0f, 0.0f, 0.0f);
    auto p6f = Point_3f(1.0f, 1.0f, 0.0f);
    auto p7f = Point_3f(-1.0f, 1.0f, 0.0f);
    auto ezerod = Eigen::Vector3d(0.0, 0.0, 0.0);

    SECTION("length")
    {
        REQUIRE(Euclid::length(zerod) == 0.0);
        REQUIRE(Euclid::length(oned) == 1.0);

        REQUIRE(Euclid::length(zerof) == 0.0f);
        REQUIRE(Euclid::length(onef) == 1.0f);
    };

    SECTION("normalize")
    {
        Euclid::normalize(zerod);
        REQUIRE(Euclid::length(zerod) == 0.0);
        Euclid::normalize(oned);
        REQUIRE(Euclid::length(oned) == 1.0);

        Euclid::normalize(zerof);
        REQUIRE(Euclid::length(zerof) == 0.0f);
        Euclid::normalize(onef);
        REQUIRE(Euclid::length(onef) == 1.0f);
    }

    SECTION("normalized")
    {
        REQUIRE(zerod == Euclid::normalized(zerod));
        REQUIRE(oned == Euclid::normalized(oned));

        REQUIRE(zerof == Euclid::normalized(zerof));
        REQUIRE(onef == Euclid::normalized(onef));
    }

    SECTION("area")
    {
        REQUIRE(Euclid::area(p1d, p1d, p1d) == 0.0);
        REQUIRE(Euclid::area(p1d, p2d, p3d) == 0.0);
        REQUIRE(Euclid::area(p1d, p2d, p4d) == 1.0);

        REQUIRE(Euclid::area(p1f, p1f, p1f) == 0.0f);
        REQUIRE(Euclid::area(p1f, p2f, p3f) == 0.0f);
        REQUIRE(Euclid::area(p1f, p2f, p4f) == 1.0f);
    }

    SECTION("sine")
    {
        REQUIRE(Euclid::sine(p1d, p1d, p1d) == Approx(0.0));
        REQUIRE(Euclid::sine(p2d, p1d, p3d) == Approx(0.0));
        REQUIRE(Euclid::sine(p2d, p1d, p4d) == Approx(1.0));
        REQUIRE(Euclid::sine(p2d, p1d, p5d) == Approx(0.0));
        REQUIRE(Euclid::sine(p2d, p1d, p6d) == Approx(sqrt2_2));
        REQUIRE(Euclid::sine(p2d, p1d, p7d) == Approx(sqrt2_2));

        REQUIRE(Euclid::sine(p1f, p1f, p1f) == Approx(0.0f));
        REQUIRE(Euclid::sine(p2f, p1f, p3f) == Approx(0.0f));
        REQUIRE(Euclid::sine(p2f, p1f, p4f) == Approx(1.0f));
        REQUIRE(Euclid::sine(p2f, p1f, p5f) == Approx(0.0f));
        REQUIRE(Euclid::sine(p2f, p1f, p6f) == Approx(sqrt2_2));
        REQUIRE(Euclid::sine(p2f, p1f, p7f) == Approx(sqrt2_2));
    }

    SECTION("cosine")
    {
        REQUIRE(Euclid::cosine(p1d, p1d, p1d) == Approx(0.0));
        REQUIRE(Euclid::cosine(p2d, p1d, p3d) == Approx(1.0));
        REQUIRE(Euclid::cosine(p2d, p1d, p4d) == Approx(0.0));
        REQUIRE(Euclid::cosine(p2d, p1d, p5d) == Approx(-1.0));
        REQUIRE(Euclid::cosine(p2d, p1d, p6d) == Approx(sqrt2_2));
        REQUIRE(Euclid::cosine(p2d, p1d, p7d) == Approx(-sqrt2_2));

        REQUIRE(Euclid::cosine(p1f, p1f, p1f) == Approx(0.0f));
        REQUIRE(Euclid::cosine(p2f, p1f, p3f) == Approx(1.0f));
        REQUIRE(Euclid::cosine(p2f, p1f, p4f) == Approx(0.0f));
        REQUIRE(Euclid::cosine(p2f, p1f, p5f) == Approx(-1.0f));
        REQUIRE(Euclid::cosine(p2f, p1f, p6f) == Approx(sqrt2_2));
        REQUIRE(Euclid::cosine(p2f, p1f, p7f) == Approx(-sqrt2_2));
    }

    SECTION("tangent")
    {
        REQUIRE(Euclid::tangent(p1d, p1d, p1d) == Approx(0.0));
        REQUIRE(Euclid::tangent(p2d, p1d, p3d) == Approx(0.0));
        REQUIRE(Euclid::tangent(p2d, p1d, p5d) == Approx(0.0));
        REQUIRE(Euclid::tangent(p2d, p1d, p6d) == Approx(1.0));
        REQUIRE(Euclid::tangent(p2d, p1d, p7d) == Approx(-1.0));

        REQUIRE(Euclid::tangent(p1f, p1f, p1f) == Approx(0.0f));
        REQUIRE(Euclid::tangent(p2f, p1f, p3f) == Approx(0.0f));
        REQUIRE(Euclid::tangent(p2f, p1f, p5f) == Approx(0.0f));
        REQUIRE(Euclid::tangent(p2f, p1f, p6f) == Approx(1.0));
        REQUIRE(Euclid::tangent(p2f, p1f, p7f) == Approx(-1.0));
    }

    SECTION("cotangent")
    {
        REQUIRE(Euclid::cotangent(p2d, p1d, p4d) == Approx(0.0));
        REQUIRE(Euclid::cotangent(p2d, p1d, p6d) == Approx(1.0));
        REQUIRE(Euclid::cotangent(p2d, p1d, p7d) == Approx(-1.0));

        REQUIRE(Euclid::cotangent(p2f, p1f, p4f) == Approx(0.0f));
        REQUIRE(Euclid::cotangent(p2f, p1f, p6f) == Approx(1.0f));
        REQUIRE(Euclid::cotangent(p2f, p1f, p7f) == Approx(-1.0f));
    }

    SECTION("cgal_to_eigen")
    {
        Eigen::Vector3d vec1;
        Eigen::VectorXd vec2;
        Euclid::cgal_to_eigen(zerod, vec1);
        Euclid::cgal_to_eigen(zerod, vec2);
        REQUIRE(vec1 == ezerod);
        REQUIRE(vec2 == ezerod);
    }

    SECTION("eigen_to_cgal")
    {
        Point_3d p;
        Vector_3d v;
        Euclid::eigen_to_cgal(ezerod, p);
        Euclid::eigen_to_cgal(ezerod, v);
        REQUIRE(p == p1d);
        REQUIRE(v == zerod);
    }
}
