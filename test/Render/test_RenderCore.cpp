#include <catch2/catch.hpp>
#include <Euclid/Render/RenderCore.h>

TEST_CASE("Render, RenderCore", "[render][rendercore]")
{
    SECTION("camera")
    {
        const float near = 0.001f;
        const float far = 1000.0f;
        Eigen::Vector3f pos(2.0f, 2.0f, 2.0f);
        Eigen::Vector3f focus(1.0f, 1.0f, 1.0f);
        Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
        Eigen::Vector3f u(1.0f, 0.0f, -1.0f);
        Eigen::Vector3f v(-0.5f, 1.0f, -0.5f);
        Eigen::Vector3f dir(1.0f, 1.0f, 1.0f);
        u.normalize();
        v.normalize();
        dir.normalize();

        Euclid::Camera c1(pos, focus, up, near, far);
        Euclid::Camera c2;
        c2.lookat(pos, focus, up);
        c2.set_range(near, far);

        REQUIRE(c1.pos == pos);
        REQUIRE(c1.u == u);
        REQUIRE(c1.v == v);
        REQUIRE(c1.dir == dir);
        REQUIRE(c1.tnear == near);
        REQUIRE(c1.tfar == far);
        REQUIRE(c2.pos == pos);
        REQUIRE(c2.u == u);
        REQUIRE(c2.v == v);
        REQUIRE(c2.dir == dir);
        REQUIRE(c2.tnear == near);
        REQUIRE(c2.tfar == far);
    }
}
