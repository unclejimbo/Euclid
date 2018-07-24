#include <Euclid/Math/Matrix.h>
#include <catch.hpp>

TEST_CASE("Math, Matrix", "[math][matrix]")
{

    SECTION("covariance_matrix")
    {
        auto p1 = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
        auto p2 = Eigen::Vector2d(0.0, 2.0);
        auto p3 = Eigen::Vector2d(1.0, 1.0);
        auto p4 = Eigen::Vector2d(2.0, 0.0);
        std::vector<Eigen::Vector3f> empty;
        std::vector<Eigen::Vector3f> one{ p1 };
        std::vector<Eigen::Vector3f> identical{ p1, p1, p1, p1 };
        std::vector<Eigen::Vector2d> corelated{ p2, p3, p4 };
        auto m0 = Eigen::Matrix3f::Zero();
        auto m1 = Eigen::Matrix2d();
        m1 << 1.0, -1.0, -1.0, 1.0;

        REQUIRE_THROWS(Euclid::covariance_matrix(empty));
        REQUIRE(Euclid::covariance_matrix(one) == m0);
        REQUIRE(Euclid::covariance_matrix(identical) == m0);
        REQUIRE(Euclid::covariance_matrix(corelated) == m1);
    }

    SECTION("sparse matrix")
    {
        using SpMat = Eigen::SparseMatrix<float>;
        std::vector<Eigen::Triplet<float>> values;
        for (size_t i = 0; i < 10; ++i) {
            values.emplace_back(i, i, i + 0.0f);
        }
        SpMat spmat(10, 10);
        spmat.setFromTriplets(values.begin(), values.end());

        SECTION("for_each")
        {
            Euclid::for_each(spmat, [](float& v) { v *= 2; });
            REQUIRE(spmat.nonZeros() == 10);
            REQUIRE(spmat.coeff(1, 1) == 2);
        }

        SECTION("transform")
        {
            SpMat out;
            Euclid::transform(spmat, out, [](float& v) { v *= v; });
            REQUIRE(out.nonZeros() == 10);
            REQUIRE(out.coeff(2, 2) == 4);
            REQUIRE(spmat.coeff(2, 2) == 2);
        }
    }

    SECTION("PCA")
    {
        auto p1 = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
        std::vector<Eigen::Vector3f> identical{ p1, p1, p1, p1 };

        Euclid::PCA<float, 3> pca1(identical);
        REQUIRE(pca1.eigen_value<0>() == 0.0f);
        REQUIRE(pca1.eigen_value<1>() == 0.0f);
        REQUIRE(pca1.eigen_value<2>() == 0.0f);
        REQUIRE(pca1.eigen_vector<0>() == Eigen::Vector3f(1.0f, 0.0f, 0.0f));
        REQUIRE(pca1.eigen_vector<1>() == Eigen::Vector3f(0.0f, 1.0f, 0.0f));
        REQUIRE(pca1.eigen_vector<2>() == Eigen::Vector3f(0.0f, 0.0f, 1.0f));
    }
}
