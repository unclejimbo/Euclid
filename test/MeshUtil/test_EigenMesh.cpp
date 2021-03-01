#include <catch2/catch.hpp>
#include <Euclid/MeshUtil/EigenMesh.h>

#include <string>
#include <vector>

#include <Euclid/IO/OffIO.h>

#include <config.h>

TEST_CASE("MeshUtil, EigenMesh", "[meshutil][eigenmesh]")
{
    std::vector<double> positions;
    std::vector<unsigned> indices;
    std::string file_name(DATA_DIR);
    file_name.append("bunny.off");
    Euclid::read_off<3>(file_name, positions, nullptr, &indices, nullptr);

    Eigen::MatrixXd V;
    Eigen::MatrixXi F;
    Euclid::make_mesh<3>(V, F, positions, indices);

    std::vector<double> new_positions;
    std::vector<unsigned> new_indices;
    Euclid::extract_mesh<3>(V, F, new_positions, new_indices);
    REQUIRE(new_positions == positions);
    REQUIRE(new_indices == indices);

    std::vector<double> only_positions;
    Euclid::extract_mesh(V, only_positions);
    REQUIRE(only_positions == positions);
}
