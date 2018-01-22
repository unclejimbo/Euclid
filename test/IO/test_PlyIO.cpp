#include "../config.h"
#include <Euclid/IO/PlyIO.h>
#include <catch.hpp>
#include <iostream>
#include <vector>

TEST_CASE("Package: IO/PlyIO", "[plyio]")
{
    SECTION("Read and write ascii file")
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

        // Read body
        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> texcoords;
        std::vector<unsigned> colors;
        std::vector<unsigned> indices;
        Euclid::read_ply<float, unsigned, unsigned, 3>(
            file, positions, &indices, &normals, &texcoords, &colors);

        REQUIRE(positions.size() == header.element(0).count() * 3);
        REQUIRE(normals.size() == header.element(0).count() * 3);
        REQUIRE(texcoords.size() == header.element(0).count() * 2);
        REQUIRE(colors.size() == header.element(0).count() * 3);
        REQUIRE(indices.size() == header.element(1).count() * 3);
        REQUIRE(positions[0] == 1.0f);
        REQUIRE(normals[0] == 0.0f);
        REQUIRE(texcoords[0] == 0.0);
        REQUIRE(colors[0] == 255);
        REQUIRE(indices[0] == 0);

        // Write file and read back again to test integrity
        std::string tmp_file(TMP_DIR);
        tmp_file.append("cube_ascii.ply");
        Euclid::write_ply<float, unsigned, unsigned, 3>(
            tmp_file,
            positions,
            &indices,
            &normals,
            &texcoords,
            &colors,
            Euclid::PlyFormat::ascii);

        std::vector<double> new_positions;
        std::vector<double> new_normals;
        std::vector<double> new_texcoords;
        std::vector<int> new_colors;
        std::vector<int> new_indices;
        Euclid::read_ply<double, int, int, 3>(tmp_file,
                                              new_positions,
                                              &new_indices,
                                              &new_normals,
                                              &new_texcoords,
                                              &new_colors);

        REQUIRE(positions.size() == new_positions.size());
        REQUIRE(normals.size() == new_normals.size());
        REQUIRE(texcoords.size() == new_texcoords.size());
        REQUIRE(colors.size() == new_colors.size());
        REQUIRE(indices.size() == new_indices.size());
        REQUIRE(positions[0] == new_positions[0]);
        REQUIRE(normals[0] == new_normals[0]);
        REQUIRE(texcoords[0] == new_texcoords[0]);
        REQUIRE(colors[0] == new_colors[0]);
        REQUIRE(indices[0] == new_indices[0]);
    }

    SECTION("Read and write binary file")
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

        // Read body
        std::vector<float> positions;
        std::vector<float> normals;
        std::vector<float> texcoords;
        std::vector<unsigned> colors;
        std::vector<unsigned> indices;
        Euclid::read_ply<float, unsigned, unsigned, 3>(
            file, positions, &indices, &normals, &texcoords, &colors);

        REQUIRE(positions.size() == header.element(0).count() * 3);
        REQUIRE(normals.size() == header.element(0).count() * 3);
        REQUIRE(texcoords.size() == header.element(0).count() * 2);
        REQUIRE(colors.size() == header.element(0).count() * 3);
        REQUIRE(indices.size() == header.element(1).count() * 3);
        REQUIRE(positions[0] == 1.0f);
        REQUIRE(normals[0] == 0.0f);
        REQUIRE(texcoords[0] == 0.0);
        REQUIRE(colors[0] == 255);
        REQUIRE(indices[0] == 0);

        // Write file and read back again to test integrity
        std::string tmp_file(TMP_DIR);
        tmp_file.append("cube_binary_big_endian.ply");
        Euclid::write_ply<float, unsigned, unsigned, 3>(
            tmp_file,
            positions,
            &indices,
            &normals,
            &texcoords,
            &colors,
            Euclid::PlyFormat::binary_big_endian);

        std::vector<double> new_positions;
        std::vector<double> new_normals;
        std::vector<double> new_texcoords;
        std::vector<int> new_colors;
        std::vector<int> new_indices;
        Euclid::read_ply<double, int, int, 3>(tmp_file,
                                              new_positions,
                                              &new_indices,
                                              &new_normals,
                                              &new_texcoords,
                                              &new_colors);

        REQUIRE(positions.size() == new_positions.size());
        REQUIRE(normals.size() == new_normals.size());
        REQUIRE(texcoords.size() == new_texcoords.size());
        REQUIRE(colors.size() == new_colors.size());
        REQUIRE(indices.size() == new_indices.size());
        REQUIRE(positions[0] == new_positions[0]);
        REQUIRE(normals[0] == new_normals[0]);
        REQUIRE(texcoords[0] == new_texcoords[0]);
        REQUIRE(colors[0] == new_colors[0]);
        REQUIRE(indices[0] == new_indices[0]);
    }
}
