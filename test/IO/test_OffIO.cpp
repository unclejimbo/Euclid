#include <catch2/catch.hpp>
#include <Euclid/IO/OffIO.h>

#include <vector>

#include <config.h>

TEST_CASE("IO, OffIO", "[io][offio]")
{

    SECTION("positions")
    {
        std::string file(DATA_DIR);
        file.append("cube_vc.off");
        std::vector<float> positions;
        Euclid::read_off(file, positions, nullptr, nullptr, nullptr);

        REQUIRE(positions.size() == 26 * 3);
        REQUIRE(positions[0] == 1.0f);

        std::string tmp_file(TMP_DIR);
        tmp_file.append("cube_vc.off");
        Euclid::write_off(tmp_file, positions, nullptr, nullptr, nullptr);
        std::vector<double> new_positions;
        Euclid::read_off(tmp_file, new_positions, nullptr, nullptr, nullptr);

        REQUIRE(new_positions.size() == positions.size());
        REQUIRE(new_positions[0] == positions[0]);
    }

    SECTION("positions and vcolors")
    {
        std::string file(DATA_DIR);
        file.append("cube_vc.off");
        std::vector<float> positions;
        std::vector<unsigned char> vcolors;
        Euclid::read_off(file, positions, &vcolors, nullptr, nullptr);

        REQUIRE(positions.size() == 26 * 3);
        REQUIRE(vcolors.size() == 26 * 4);
        REQUIRE(positions[0] == 1.0f);
        REQUIRE(vcolors[0] == 255);

        std::string tmp_file(TMP_DIR);
        tmp_file.append("cube_vc.off");
        Euclid::write_off(tmp_file, positions, &vcolors, nullptr, nullptr);
        std::vector<double> new_positions;
        std::vector<int> new_vcolors;
        Euclid::read_off(
            tmp_file, new_positions, &new_vcolors, nullptr, nullptr);

        REQUIRE(new_positions.size() == positions.size());
        REQUIRE(new_vcolors.size() == vcolors.size());
        REQUIRE(new_positions[0] == positions[0]);
        REQUIRE(new_vcolors[0] == vcolors[0]);
    }

    SECTION("positions and indices")
    {
        std::string file(DATA_DIR);
        file.append("cube_vc.off");
        std::vector<float> positions;
        std::vector<unsigned> indices;
        Euclid::read_off<3>(file, positions, nullptr, &indices, nullptr);

        REQUIRE(positions.size() == 26 * 3);
        REQUIRE(indices.size() == 12 * 3);
        REQUIRE(positions[0] == 1.0f);
        REQUIRE(indices[0] == 0);

        std::string tmp_file(TMP_DIR);
        tmp_file.append("cube_vc.off");
        Euclid::write_off<3>(tmp_file, positions, nullptr, &indices, nullptr);
        std::vector<double> new_positions;
        std::vector<int> new_indices;
        Euclid::read_off<3>(
            tmp_file, new_positions, nullptr, &new_indices, nullptr);

        REQUIRE(new_positions.size() == positions.size());
        REQUIRE(new_indices.size() == indices.size());
        REQUIRE(new_positions[0] == positions[0]);
        REQUIRE(new_indices[0] == indices[0]);
    }

    SECTION("positions, indices and vcolors")
    {
        std::string file(DATA_DIR);
        file.append("cube_vc.off");
        std::vector<float> positions;
        std::vector<unsigned char> vcolors;
        std::vector<unsigned> indices;
        Euclid::read_off<3>(file, positions, &vcolors, &indices, nullptr);

        REQUIRE(positions.size() == 26 * 3);
        REQUIRE(vcolors.size() == 26 * 4);
        REQUIRE(indices.size() == 12 * 3);
        REQUIRE(positions[0] == 1.0f);
        REQUIRE(vcolors[0] == 255);
        REQUIRE(indices[0] == 0);

        std::string tmp_file(TMP_DIR);
        tmp_file.append("cube_vc.off");
        Euclid::write_off<3>(tmp_file, positions, &vcolors, &indices, nullptr);
        std::vector<double> new_positions;
        std::vector<int> new_vcolors;
        std::vector<int> new_indices;
        Euclid::read_off<3>(
            tmp_file, new_positions, &new_vcolors, &new_indices, nullptr);

        REQUIRE(new_positions.size() == positions.size());
        REQUIRE(new_vcolors.size() == vcolors.size());
        REQUIRE(new_indices.size() == indices.size());
        REQUIRE(new_positions[0] == positions[0]);
        REQUIRE(new_vcolors[0] == vcolors[0]);
        REQUIRE(new_indices[0] == indices[0]);
    }

    SECTION("positions, indices and fcolors")
    {
        std::string file(DATA_DIR);
        file.append("cube_fc.off");
        std::vector<float> positions;
        std::vector<unsigned char> fcolors;
        std::vector<unsigned> indices;
        Euclid::read_off<3>(file, positions, nullptr, &indices, &fcolors);

        REQUIRE(positions.size() == 26 * 3);
        REQUIRE(fcolors.size() == 12 * 4);
        REQUIRE(indices.size() == 12 * 3);
        REQUIRE(positions[0] == 1.0f);
        REQUIRE(fcolors[0] == 0);
        REQUIRE(indices[0] == 0);

        std::string tmp_file(TMP_DIR);
        tmp_file.append("cube_fc.off");
        Euclid::write_off<3>(tmp_file, positions, nullptr, &indices, &fcolors);
        std::vector<double> new_positions;
        std::vector<int> new_fcolors;
        std::vector<int> new_indices;
        Euclid::read_off<3>(
            tmp_file, new_positions, nullptr, &new_indices, &new_fcolors);

        REQUIRE(new_positions.size() == positions.size());
        REQUIRE(new_fcolors.size() == fcolors.size());
        REQUIRE(new_indices.size() == indices.size());
        REQUIRE(new_positions[0] == positions[0]);
        REQUIRE(new_fcolors[0] == fcolors[0]);
        REQUIRE(new_indices[0] == indices[0]);
    }
}
