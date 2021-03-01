#include <catch2/catch.hpp>
#include <Euclid/Topology/HomotopyGenerator.h>

#include <string>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/MeshUtil/CGALMesh.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

TEST_CASE("Topology, HomotopyGenerator", "[topology][homotopygenerator]")
{
    std::string filename(DATA_DIR);
    filename.append("kitten.off");
    std::vector<float> positions;
    std::vector<int> indices;
    Euclid::read_off<3>(filename, positions, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    SECTION("primal")
    {
        auto src = *(vertices(mesh).first);
        auto generators = Euclid::greedy_homotopy_generators(mesh, src);
        REQUIRE(generators.size() == 2);
        for (const auto& g : generators) {
            REQUIRE(Euclid::is_loop(mesh, g));
        }
    }
}
