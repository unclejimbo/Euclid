#include <catch2/catch.hpp>
#include <Euclid/Descriptor/Histogram.h>

TEST_CASE("Descriptor, Histogram", "[distance][histogram]")
{
    SECTION("compare arrays")
    {
        Eigen::Array3f d1(1.0f, 1.0f, 1.0f);
        Eigen::Array3f d2(1.0f, 2.0f, 3.0f);

        REQUIRE(Euclid::l1(d1, d2) == 3.0f);
        REQUIRE(Euclid::l2(d1, d2) == std::sqrt(5.0f));
        REQUIRE(Euclid::chi2(d1, d2) == 8.0f / 3.0f);
        REQUIRE(Euclid::chi2_asym(d1, d2) == 5.0f);
        REQUIRE(Euclid::chi2_asym(d2, d1) == 11.0f / 6.0f);
    }

    SECTION("compare array expressions")
    {
        Eigen::Array3d d1(1.0, 1.0, 1.0);
        Eigen::ArrayXXd d2(3, 2);
        d2 << 1.0, 2.0, 3.0, 1.0, 2.0, 3.0;
        Eigen::Vector3d d3(1.0, 1.0, 1.0);

        REQUIRE(Euclid::l1(d1, d2.col(0)) == 3.0);
        REQUIRE(Euclid::l2(d1, d2.col(1)) == std::sqrt(5.0));
        REQUIRE(Euclid::chi2(d1, d2.col(0)) == 8.0 / 3.0);
        REQUIRE(Euclid::chi2_asym(d1, d2.col(1)) == 5.0);
        REQUIRE(Euclid::chi2_asym(d2.col(0), d3.array()) == 11.0 / 6.0);
    }
}
