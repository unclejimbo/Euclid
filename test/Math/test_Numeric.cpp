#include <Euclid/Math/Numeric.h>
#include <catch.hpp>
#include <iostream>
#include <boost/math/constants/constants.hpp>

TEST_CASE("Package: Math/Numeric", "[numeric]")
{
    SECTION("Floating point equality check")
    {
        float zerof = 0.0f;
        float almost_zerof = std::sin(boost::math::double_constants::pi);
        float onef = 1.0f;
        float almost_onef = 0.0f;
        for (int i = 0; i < 10; ++i) {
            almost_onef += 0.1f;
        }
        double smalld = 1.0e-8;
        double almost_smalld = smalld + 1.0e-16;
        double larged = 1.0e8;
        double almost_larged = larged + 1.0e-8;
        REQUIRE(zerof != almost_zerof);
        REQUIRE(onef != almost_onef);
        REQUIRE(smalld != almost_smalld);
        REQUIRE(larged != almost_larged);

        SECTION("eq_almost is robust in most cases")
        {
            REQUIRE(Euclid::eq_almost(zerof, almost_zerof));
            REQUIRE(Euclid::eq_almost(smalld, almost_smalld));
            REQUIRE(Euclid::eq_almost(onef, almost_onef));
            REQUIRE(Euclid::eq_almost(larged, almost_larged));

            double fail = 1.0e-8;
            double almost_fail = fail + 1.0e-14;
            REQUIRE(!Euclid::eq_almost(fail, almost_fail));
            REQUIRE(Euclid::eq_almost(fail, almost_fail, 1.0e-13));
        }

        SECTION("eq_rel_err is valid when not comparing to small values")
        {
            REQUIRE(!Euclid::eq_rel_err(zerof, almost_zerof));
            REQUIRE(Euclid::eq_rel_err(
                zerof,
                almost_zerof,
                1.0e10f * std::numeric_limits<float>::epsilon()));
            REQUIRE(!Euclid::eq_rel_err(smalld, almost_smalld));
            REQUIRE(Euclid::eq_rel_err(
                smalld,
                almost_smalld,
                1.0e8 * std::numeric_limits<double>::epsilon()));
            REQUIRE(Euclid::eq_rel_err(onef, almost_onef));
            REQUIRE(Euclid::eq_rel_err(larged, almost_larged));
        }

        SECTION("eq_abs_err is only valid when the bound is known")
        {
            REQUIRE(Euclid::eq_abs_err(zerof, almost_zerof));
            REQUIRE(Euclid::eq_abs_err(smalld, almost_smalld));
            REQUIRE(!Euclid::eq_abs_err(onef, almost_onef));
            REQUIRE(Euclid::eq_abs_err(onef, almost_onef, 1.0e-6f));
            REQUIRE(!Euclid::eq_abs_err(larged, almost_larged));
            REQUIRE(Euclid::eq_abs_err(larged, almost_larged, 1.0e-7));
        }

        SECTION("eq_ulp is also robust when not comparing to small values")
        {
            REQUIRE(!Euclid::eq_ulp(zerof, almost_zerof));
            REQUIRE(!Euclid::eq_ulp(smalld, almost_smalld));
            REQUIRE(Euclid::eq_ulp(onef, almost_onef, 1));
            REQUIRE(!Euclid::eq_ulp(onef, almost_onef, 0));
            REQUIRE(Euclid::eq_ulp(larged, almost_larged, 1));
        }
    }

    SECTION("Floating point inequality check")
    {
        float small = 0.1f;
        float large = 1000.0f;
        float margin = 1.0e-15f;

        REQUIRE(!(small + margin < small));
        REQUIRE(Euclid::less_safe(small + margin, small, 1.0e-14f));
        REQUIRE(!(large + margin < large));
        REQUIRE(Euclid::less_safe(large + margin, large));
        REQUIRE(!(small > small + margin));
        REQUIRE(Euclid::greater_safe(small, small + margin, 1.0e-14f));
        REQUIRE(!(large > large + margin));
        REQUIRE(Euclid::greater_safe(large, large + margin));
    }
}
