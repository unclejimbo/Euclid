#include <Euclid/Math/Matrix.h>
#include <catch.hpp>

TEST_CASE("Math, Matrix", "[math][matrix]")
{
    SECTION("sparse matrix")
    {
        using SpMat = Eigen::SparseMatrix<float>;
        std::vector<Eigen::Triplet<float>> values;
        for (int i = 0; i < 10; ++i) {
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
}
