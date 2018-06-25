#include <Euclid/IO/OffIO.h>
#include <catch.hpp>

#include <config.h>

TEST_CASE("Package: IO/OffIO", "[offio]")
{
    SECTION("Function: read_off and write_off")
    {
        std::string file(DATA_DIR);
        file.append("chair.off");
        std::vector<float> positions;
        std::vector<unsigned> indices;
        Euclid::read_off<3>(file, positions, indices);

        REQUIRE(positions.size() == 2382 * 3);
        REQUIRE(indices.size() == 2234 * 3);
        REQUIRE(positions[0] == 113.772f);
        REQUIRE(indices[0] == 0);

        std::string tmp_file(TMP_DIR);
        tmp_file.append("chair.off");
        Euclid::write_off<3>(tmp_file, positions, indices);
        std::vector<double> new_positions;
        std::vector<int> new_indices;
        Euclid::read_off<3>(tmp_file, new_positions, new_indices);

        REQUIRE(new_positions.size() == positions.size());
        REQUIRE(new_indices.size() == indices.size());
        REQUIRE(new_positions[0] == 113.772);
        REQUIRE(new_indices[0] == 0);
    }
}
