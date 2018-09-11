#include <catch2/catch.hpp>
#include <Euclid/Util/Serialize.h>

#include <cereal/archives/json.hpp>
#include <cereal/types/vector.hpp>
#include <igl/isdiag.h>
#include <igl/speye.h>

#include <config.h>

TEST_CASE("Util, Serialize", "[util][serialize]")
{
    SECTION("Eigen::Matrix")
    {
        Eigen::MatrixXf from = Eigen::MatrixXf::Random(1000, 1000);
        Eigen::MatrixXf to;
        std::string file(TMP_DIR);
        file.append("matrix.cereal");

        Euclid::serialize(file, from);
        Euclid::deserialize(file, to);

        REQUIRE(from.size() == to.size());
        REQUIRE(from(0, 0) == to(0, 0));
    }

    SECTION("Eigen::Array")
    {
        Eigen::ArrayXXd from = Eigen::ArrayXXd::Random(1000, 1000);
        Eigen::ArrayXXd to;
        std::string file(TMP_DIR);
        file.append("array.cereal");

        Euclid::serialize(file, from);
        Euclid::deserialize(file, to);

        REQUIRE(from.size() == to.size());
        REQUIRE(from(0, 0) == to(0, 0));
    }

    SECTION("Eigen::SparseMatrix")
    {
        Eigen::SparseMatrix<float> from, to;
        igl::speye(1000, 1000, from);
        std::string file(TMP_DIR);
        file.append("spmat.cereal");

        Euclid::serialize(file, from);
        Euclid::deserialize(file, to);

        REQUIRE(from.nonZeros() == to.nonZeros());
        REQUIRE(igl::isdiag(to));
    }

    SECTION("mix with stl")
    {
        std::vector<Eigen::Vector3f> from(1000), to;
        for (auto& v : from) {
            v = Eigen::Vector3f::Random();
        }
        std::string file(TMP_DIR);
        file.append("stl.cereal");

        Euclid::serialize(file, from);
        Euclid::deserialize(file, to);

        REQUIRE(from.size() == to.size());
        REQUIRE(from[0] == to[0]);
    }

    SECTION("multiple values")
    {
        int from_i = -5, to_i;
        std::vector<float> from_vec(3, 1.0f), to_vec;
        Eigen::Matrix3d from_mat, to_mat;
        from_mat << 1.0, 2.0, 3.0, 4.0, 5.0, 6.0, 7.0, 8.0, 9.0;
        std::string file(TMP_DIR);
        file.append("multiple.cereal");

        Euclid::serialize(file, from_i, from_vec, from_mat);
        Euclid::deserialize(file, to_i, to_vec, to_mat);

        REQUIRE(from_i == to_i);
        REQUIRE(from_vec == to_vec);
        REQUIRE(from_mat == to_mat);
    }

    SECTION("serialize to json")
    {
        Eigen::Vector3f from, to;
        from.setRandom();
        std::string file(TMP_DIR);
        file.append("vec.cereal.json");

        {
            auto mode = std::ios_base::out | std::ios_base::trunc;
            std::ofstream ofs(file, mode);
            cereal::JSONOutputArchive archive(ofs);
            archive(cereal::make_nvp("Vec3", from));
        }

        {
            auto mode = std::ios_base::in;
            std::ifstream ifs(file, mode);
            cereal::JSONInputArchive archive(ifs);
            archive(cereal::make_nvp("Vec3", to));
        }

        REQUIRE(from.size() == to.size());
        REQUIRE(from == to);
    }
}
