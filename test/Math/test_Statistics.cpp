#include <Euclid/Math/Statistics.h>
#include <catch.hpp>

TEST_CASE("Math, Statistics", "[math][statistics]")
{
    SECTION("covariance")
    {
        Eigen::Vector3d one = Eigen::Vector3d::Ones();
        Eigen::MatrixXd cov1;
        Euclid::covariance_matrix(one, cov1);
        REQUIRE(cov1.size() == 1);
        REQUIRE(cov1(0) == 0);

        Eigen::MatrixXf identical = Eigen::MatrixXf::Ones(5, 3);
        Eigen::Matrix3f cov2;
        Euclid::covariance_matrix(identical, cov2);
        REQUIRE(cov2 == Eigen::Matrix3f::Zero());

        Eigen::MatrixXf corelated(3, 2);
        corelated << 0.0f, 2.0f, 1.0f, 1.0f, 2.0f, 0.0f;
        Eigen::MatrixXf cov3;
        Euclid::covariance_matrix(corelated, cov3);
        Eigen::MatrixXf correct_cov3(2, 2);
        correct_cov3 << 2.0f, -2.0f, -2.0f, 2.0f;
        REQUIRE(cov3 == correct_cov3);
    }
}
