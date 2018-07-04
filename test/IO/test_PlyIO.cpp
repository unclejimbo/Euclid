#include <Euclid/IO/PlyIO.h>
#include <catch.hpp>

#include <iostream>
#include <vector>

#include <config.h>

TEST_CASE("IO, PlyIO", "[io][plyio]")
{
    SECTION("ascii file")
    {
        // Read header
        std::string file(DATA_DIR);
        file.append("cube_ascii.ply");
        auto header = Euclid::read_ply_header(file);

        REQUIRE(header.format() == Euclid::PlyFormat::ascii);
        REQUIRE(header.n_elems() == 2);
        REQUIRE(header.element(0).name() == "vertex");
        REQUIRE(header.element(0).count() == 26);
        REQUIRE(header.element(0).n_props() == 11);
        REQUIRE(header.element(0).property(0)->name() == "x");
        REQUIRE(header.element(1).name() == "face");
        REQUIRE(header.element(1).count() == 12);
        REQUIRE(header.element(1).n_props() == 1);
        REQUIRE(header.element(1).property(0)->name() == "vertex_indices");

        SECTION("positions only")
        {
            // Read body
            std::vector<float> positions;
            Euclid::read_ply<3>(
                file, positions, nullptr, nullptr, nullptr, nullptr);
            REQUIRE(positions.size() == header.element(0).count() * 3);

            // Write file and read back again to test integrity
            std::string tmp_file(TMP_DIR);
            tmp_file.append("cube_ascii.ply");
            Euclid::write_ply<3>(
                tmp_file, positions, nullptr, nullptr, nullptr, nullptr);

            std::vector<double> new_positions;
            Euclid::read_ply<3>(
                tmp_file, new_positions, nullptr, nullptr, nullptr, nullptr);

            REQUIRE(positions.size() == new_positions.size());
            REQUIRE(positions[0] == new_positions[0]);
        }

        SECTION("positions, normals and texcoords")
        {
            // Read body
            std::vector<float> positions;
            std::vector<float> normals;
            std::vector<float> texcoords;
            Euclid::read_ply<3>(
                file, positions, &normals, &texcoords, nullptr, nullptr);
            REQUIRE(positions.size() == header.element(0).count() * 3);

            // Write file and read back again to test integrity
            std::string tmp_file(TMP_DIR);
            tmp_file.append("cube_ascii.ply");
            Euclid::write_ply<3>(
                tmp_file, positions, &normals, &texcoords, nullptr, nullptr);

            std::vector<double> new_positions;
            std::vector<double> new_normals;
            std::vector<double> new_texcoords;
            Euclid::read_ply<3>(tmp_file,
                                new_positions,
                                &new_normals,
                                &new_texcoords,
                                nullptr,
                                nullptr);

            REQUIRE(positions.size() == new_positions.size());
            REQUIRE(positions[0] == new_positions[0]);
            REQUIRE(normals.size() == new_normals.size());
            REQUIRE(texcoords.size() == new_texcoords.size());
            REQUIRE(normals[0] == new_normals[0]);
            REQUIRE(texcoords[0] == new_texcoords[0]);
        }

        SECTION("positions and indices")
        {
            // Read body
            std::vector<float> positions;
            std::vector<unsigned> indices;
            Euclid::read_ply<3>(
                file, positions, nullptr, nullptr, &indices, nullptr);
            REQUIRE(positions.size() == header.element(0).count() * 3);

            // Write file and read back again to test integrity
            std::string tmp_file(TMP_DIR);
            tmp_file.append("cube_ascii.ply");
            Euclid::write_ply<3>(
                tmp_file, positions, nullptr, nullptr, &indices, nullptr);

            std::vector<double> new_positions;
            std::vector<int> new_indices;
            Euclid::read_ply<3>(tmp_file,
                                new_positions,
                                nullptr,
                                nullptr,
                                &new_indices,
                                nullptr);

            REQUIRE(positions.size() == new_positions.size());
            REQUIRE(positions[0] == new_positions[0]);
            REQUIRE(indices.size() == new_indices.size());
            REQUIRE(indices[0] == new_indices[0]);
        }

        SECTION("positions and colors")
        {
            // Read body
            std::vector<float> positions;
            std::vector<unsigned> colors;
            Euclid::read_ply<3>(
                file, positions, nullptr, nullptr, nullptr, &colors);
            REQUIRE(positions.size() == header.element(0).count() * 3);

            // Write file and read back again to test integrity
            std::string tmp_file(TMP_DIR);
            tmp_file.append("cube_ascii.ply");
            Euclid::write_ply<3>(
                tmp_file, positions, nullptr, nullptr, nullptr, &colors);

            std::vector<double> new_positions;
            std::vector<int> new_colors;
            Euclid::read_ply<3>(tmp_file,
                                new_positions,
                                nullptr,
                                nullptr,
                                nullptr,
                                &new_colors);

            REQUIRE(positions.size() == new_positions.size());
            REQUIRE(positions[0] == new_positions[0]);
            REQUIRE(colors.size() == new_colors.size());
            REQUIRE(colors[0] == new_colors[0]);
        }
    }

    SECTION("binary file")
    {
        // Read header
        std::string file(DATA_DIR);
        file.append("cube_binary_little_endian.ply");
        auto header = Euclid::read_ply_header(file);

        REQUIRE(header.format() == Euclid::PlyFormat::binary_little_endian);
        REQUIRE(header.n_elems() == 2);
        REQUIRE(header.element(0).name() == "vertex");
        REQUIRE(header.element(0).count() == 26);
        REQUIRE(header.element(0).n_props() == 11);
        REQUIRE(header.element(0).property(0)->name() == "x");
        REQUIRE(header.element(1).name() == "face");
        REQUIRE(header.element(1).count() == 12);
        REQUIRE(header.element(1).n_props() == 1);
        REQUIRE(header.element(1).property(0)->name() == "vertex_indices");

        SECTION("positions only")
        {
            // Read body
            std::vector<float> positions;
            Euclid::read_ply<3>(
                file, positions, nullptr, nullptr, nullptr, nullptr);
            REQUIRE(positions.size() == header.element(0).count() * 3);

            // Write file and read back again to test integrity
            std::string tmp_file(TMP_DIR);
            tmp_file.append("cube_binary_big_endian.ply");
            Euclid::write_ply<3>(tmp_file,
                                 positions,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 Euclid::PlyFormat::binary_big_endian);

            std::vector<double> new_positions;
            Euclid::read_ply<3>(
                tmp_file, new_positions, nullptr, nullptr, nullptr, nullptr);

            REQUIRE(positions.size() == new_positions.size());
            REQUIRE(positions[0] == new_positions[0]);
        }

        SECTION("positions, normals and texcoords")
        {
            // Read body
            std::vector<float> positions;
            std::vector<float> normals;
            std::vector<float> texcoords;
            Euclid::read_ply<3>(
                file, positions, &normals, &texcoords, nullptr, nullptr);
            REQUIRE(positions.size() == header.element(0).count() * 3);

            // Write file and read back again to test integrity
            std::string tmp_file(TMP_DIR);
            tmp_file.append("cube_binary_big_endian.ply");
            Euclid::write_ply<3>(tmp_file,
                                 positions,
                                 &normals,
                                 &texcoords,
                                 nullptr,
                                 nullptr,
                                 Euclid::PlyFormat::binary_big_endian);

            std::vector<double> new_positions;
            std::vector<double> new_normals;
            std::vector<double> new_texcoords;
            Euclid::read_ply<3>(tmp_file,
                                new_positions,
                                &new_normals,
                                &new_texcoords,
                                nullptr,
                                nullptr);

            REQUIRE(positions.size() == new_positions.size());
            REQUIRE(positions[0] == new_positions[0]);
            REQUIRE(normals.size() == new_normals.size());
            REQUIRE(texcoords.size() == new_texcoords.size());
            REQUIRE(normals[0] == new_normals[0]);
            REQUIRE(texcoords[0] == new_texcoords[0]);
        }

        SECTION("positions and indices")
        {
            // Read body
            std::vector<float> positions;
            std::vector<unsigned> indices;
            Euclid::read_ply<3>(
                file, positions, nullptr, nullptr, &indices, nullptr);
            REQUIRE(positions.size() == header.element(0).count() * 3);

            // Write file and read back again to test integrity
            std::string tmp_file(TMP_DIR);
            tmp_file.append("cube_binary_big_endian.ply");
            Euclid::write_ply<3>(tmp_file,
                                 positions,
                                 nullptr,
                                 nullptr,
                                 &indices,
                                 nullptr,
                                 Euclid::PlyFormat::binary_big_endian);

            std::vector<double> new_positions;
            std::vector<int> new_indices;
            Euclid::read_ply<3>(tmp_file,
                                new_positions,
                                nullptr,
                                nullptr,
                                &new_indices,
                                nullptr);

            REQUIRE(positions.size() == new_positions.size());
            REQUIRE(positions[0] == new_positions[0]);
            REQUIRE(indices.size() == new_indices.size());
            REQUIRE(indices[0] == new_indices[0]);
        }

        SECTION("positions and colors")
        {
            // Read body
            std::vector<float> positions;
            std::vector<unsigned> colors;
            Euclid::read_ply<3>(
                file, positions, nullptr, nullptr, nullptr, &colors);
            REQUIRE(positions.size() == header.element(0).count() * 3);

            // Write file and read back again to test integrity
            std::string tmp_file(TMP_DIR);
            tmp_file.append("cube_binary_big_endian.ply");
            Euclid::write_ply<3>(tmp_file,
                                 positions,
                                 nullptr,
                                 nullptr,
                                 nullptr,
                                 &colors,
                                 Euclid::PlyFormat::binary_big_endian);

            std::vector<double> new_positions;
            std::vector<int> new_colors;
            Euclid::read_ply<3>(tmp_file,
                                new_positions,
                                nullptr,
                                nullptr,
                                nullptr,
                                &new_colors);

            REQUIRE(positions.size() == new_positions.size());
            REQUIRE(positions[0] == new_positions[0]);
            REQUIRE(colors.size() == new_colors.size());
            REQUIRE(colors[0] == new_colors[0]);
        }
    }
}
