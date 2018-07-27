#include <Euclid/Math/Distance.h>
#include <catch.hpp>

TEST_CASE("Math, Distance", "[math][distance]")
{
    Eigen::Array3f d1(1.0f, 1.0f, 1.0f);
    Eigen::Array3f d2(1.0f, 2.0f, 3.0f);

    REQUIRE(Euclid::l1(d1, d2) == 3.0f);
    REQUIRE(Euclid::l2(d1, d2) == std::sqrt(5.0f));
    REQUIRE(Euclid::chi2(d1, d2) == 8.0f / 3.0f);
    REQUIRE(Euclid::chi2_asym(d1, d2) == 5.0f);
    REQUIRE(Euclid::chi2_asym(d2, d1) == 11.0f / 6.0f);
}
