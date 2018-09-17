#include <catch2/catch.hpp>
#include <Euclid/Geometry/PrimitiveGenerator.h>

#include <string>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/IO/OffIO.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = typename Kernel::Point_3;

TEST_CASE("Geometry, PrimitiveGenerator", "[geometry][primitivegenerator]")
{
    SECTION("subdivision sphere")
    {
        using Mesh = CGAL::Surface_mesh<Point_3>;
        Mesh mesh;
        Euclid::make_subdivision_sphere(mesh);

        std::vector<float> positions;
        std::vector<unsigned> indices;
        Euclid::extract_mesh<3>(mesh, positions, indices);

        std::string file(TMP_DIR);
        file.append("subdiv_sphere.off");
        Euclid::write_off<3>(file, positions, nullptr, &indices, nullptr);
    }
}
