#include <catch2/catch.hpp>
#include <Euclid/Topology/MeshTopology.h>

#include <string>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/MeshUtil/MeshHelpers.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

TEST_CASE("Topology, MeshTopology", "[topology][meshtopology]")
{
    SECTION("closed mesh")
    {
        std::string filename(DATA_DIR);
        filename.append("kitten.off");
        std::vector<float> positions;
        std::vector<int> indices;
        Euclid::read_off<3>(filename, positions, nullptr, &indices, nullptr);
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);
        REQUIRE(Euclid::num_bondaries(mesh) == 0);
        REQUIRE(Euclid::euler_characteristic(mesh) == 0);
        REQUIRE(Euclid::genus(mesh) == 1);
    }

    SECTION("open mesh")
    {
        std::string filename(DATA_DIR);
        filename.append("beetle.off");
        std::vector<float> positions;
        std::vector<int> indices;
        Euclid::read_off<3>(filename, positions, nullptr, &indices, nullptr);
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);
        REQUIRE(Euclid::num_bondaries(mesh) == 9);
        REQUIRE(Euclid::euler_characteristic(mesh) == -7);
        REQUIRE(Euclid::genus(mesh) == 0);
    }
}
