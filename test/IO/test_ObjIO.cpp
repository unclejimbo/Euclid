#include <catch2/catch.hpp>
#include <Euclid/IO/ObjIO.h>

#include <string>

#include <config.h>

TEST_CASE("IO, ObjIO", "[io][objio]")
{
    SECTION("vertex properties")
    {
        std::string file(DATA_DIR);
        file.append("sphere.obj");
        std::vector<float> positions;
        std::vector<float> texcoords;
        std::vector<float> normals;
        Euclid::read_obj(file, positions, &texcoords, &normals);

        REQUIRE(positions.size() == 382 * 3);
        REQUIRE(texcoords.size() == 439 * 2);
        REQUIRE(normals.size() == 444 * 3);
        REQUIRE(positions[0] == Approx(0.104145));
        REQUIRE(texcoords[0] == Approx(0.0));
        REQUIRE(normals[0] == Approx(0.2104));

        std::string fout(TMP_DIR);
        fout.append("sphere1.obj");
        Euclid::write_obj(fout, positions, &texcoords, &normals);
        std::vector<float> new_positions;
        std::vector<float> new_texcoords;
        std::vector<float> new_normals;
        Euclid::read_obj(fout, new_positions, &new_texcoords, &new_normals);

        REQUIRE(new_positions.size() == positions.size());
        REQUIRE(new_texcoords.size() == texcoords.size());
        REQUIRE(new_normals.size() == normals.size());
        REQUIRE(new_positions[0] == Approx(positions[0]));
        REQUIRE(new_texcoords[0] == Approx(texcoords[0]));
        REQUIRE(new_normals[0] == Approx(normals[0]));
    }

    SECTION("vertices and indices")
    {
        std::string file(DATA_DIR);
        file.append("sphere.obj");
        std::vector<float> positions;
        std::vector<float> texcoords;
        std::vector<float> normals;
        std::vector<unsigned> pindices;
        std::vector<unsigned> tindices;
        std::vector<unsigned> nindices;
        Euclid::read_obj<3>(file,
                            positions,
                            pindices,
                            &texcoords,
                            &tindices,
                            &normals,
                            &nindices);

        REQUIRE(positions.size() == 382 * 3);
        REQUIRE(texcoords.size() == 439 * 2);
        REQUIRE(normals.size() == 444 * 3);
        REQUIRE(pindices.size() == 760 * 3);
        REQUIRE(tindices.size() == pindices.size());
        REQUIRE(nindices.size() == pindices.size());
        REQUIRE(positions[0] == Approx(0.104145));
        REQUIRE(texcoords[0] == Approx(0.0));
        REQUIRE(normals[0] == Approx(0.2104));
        REQUIRE(pindices[0] == 0);
        REQUIRE(tindices[0] == 0);
        REQUIRE(nindices[0] == 0);

        std::string fout(TMP_DIR);
        fout.append("sphere2.obj");
        Euclid::write_obj<3>(fout,
                             positions,
                             pindices,
                             &texcoords,
                             &tindices,
                             &normals,
                             &nindices);
        std::vector<float> new_positions;
        std::vector<float> new_texcoords;
        std::vector<float> new_normals;
        std::vector<unsigned> new_pindices;
        std::vector<unsigned> new_tindices;
        std::vector<unsigned> new_nindices;
        Euclid::read_obj<3>(fout,
                            new_positions,
                            new_pindices,
                            &new_texcoords,
                            &new_tindices,
                            &new_normals,
                            &new_nindices);

        REQUIRE(new_positions.size() == positions.size());
        REQUIRE(new_texcoords.size() == texcoords.size());
        REQUIRE(new_normals.size() == normals.size());
        REQUIRE(new_pindices.size() == pindices.size());
        REQUIRE(new_tindices.size() == tindices.size());
        REQUIRE(new_nindices.size() == nindices.size());
        REQUIRE(new_positions[0] == Approx(positions[0]));
        REQUIRE(new_texcoords[0] == Approx(texcoords[0]));
        REQUIRE(new_normals[0] == Approx(normals[0]));
        REQUIRE(new_pindices[0] == 0);
        REQUIRE(new_tindices[0] == 0);
        REQUIRE(new_nindices[0] == 0);
    }
}
