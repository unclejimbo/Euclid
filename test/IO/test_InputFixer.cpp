#include <catch.hpp>
#include <Euclid/IO/InputFixer.h>

TEST_CASE("Package: IO/InputFixer", "[input_fixer]")
{
    SECTION("Fix vertex duplication")
    {
        std::vector<double> one_point{ 0.0, 0.0, 0.0 };
        REQUIRE(Euclid::remove_duplicate_vertices(one_point) == 0);
        REQUIRE(one_point.size() == 3);

        std::vector<double> duplication{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0,
                                         2.0, 2.0, 3.0, 3.0, 3.0, 4.0, 4.0,
                                         4.0, 4.0, 4.0, 4.0, 0.0, 0.0, 0.0 };
        REQUIRE(Euclid::remove_duplicate_vertices(duplication) == 3);
        REQUIRE(duplication.size() == 12);
    }

    SECTION("Fix vertex duplication with indices")
    {
        std::vector<double> positions{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0,
                                       2.0, 2.0, 3.0, 3.0, 3.0, 4.0, 4.0,
                                       4.0, 4.0, 4.0, 4.0, 0.0, 0.0, 0.0,
                                       7.0, 7.0, 7.0, 8.0, 8.0, 8.0 };
        std::vector<int> indices{ 0, 1, 2, 3, 4, 5, 6, 7, 8 };
        auto removed_positions = positions;
        auto removed_indices = indices;
        REQUIRE(Euclid::remove_duplicate_vertices(removed_positions,
                                                  removed_indices) == 3);
        REQUIRE(removed_indices.size() == indices.size());
        for (size_t i = 0; i < indices.size(); ++i) {
            REQUIRE(positions[3 * indices[i]] ==
                    removed_positions[3 * removed_indices[i]]);
        }
    }

    SECTION("Fix face duplication")
    {
        std::vector<size_t> indices{ 0, 1, 2, 0, 2, 1, 1, 2, 0, 2, 1, 0 };
        REQUIRE(Euclid::remove_duplicate_faces<3>(indices) == 2);
        REQUIRE(indices.size() == 6);
        REQUIRE(indices[0] == 0);
        REQUIRE(indices[3] == 0);
    }

    SECTION("Fix unreferenced vertices")
    {
        std::vector<double> positions{ 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 2.0,
                                       2.0, 2.0, 3.0, 3.0, 3.0, 4.0, 4.0,
                                       4.0, 4.0, 4.0, 4.0, 0.0, 0.0, 0.0,
                                       7.0, 7.0, 7.0, 8.0, 8.0, 8.0 };
        std::vector<unsigned> indices{ 0, 1, 2, 0, 5, 6, 0, 3, 7 };
        auto fixed_positions = positions;
        auto fixed_indices = indices;
        REQUIRE(Euclid::remove_unreferenced_vertices<3>(fixed_positions,
                                                        fixed_indices) == 2);
        REQUIRE(indices.size() == fixed_indices.size());
        REQUIRE(fixed_positions.size() == positions.size() - 6);
        for (size_t i = 0; i < indices.size(); ++i) {
            REQUIRE(fixed_positions[fixed_indices[i] * 3] ==
                    positions[indices[i] * 3]);
        }
    }
}
