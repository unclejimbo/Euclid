#include <catch2/catch.hpp>
#include <Euclid/ViewSelection/ViewEntropy.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/MeshUtil/MeshHelpers.h>
#include <Euclid/Util/Color.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

static void write_view_sphere(const Euclid::ViewSphere<Mesh>& sphere,
                              const std::vector<float>& view_scores,
                              const std::string& name)
{
    std::vector<float> spositions;
    std::vector<unsigned> sindices;
    Euclid::extract_mesh<3>(sphere.mesh, spositions, sindices);
    std::vector<unsigned char> colors;
    Euclid::colormap(igl::COLOR_MAP_TYPE_JET, view_scores, colors, true);
    std::string fout(TMP_DIR);
    fout.append(name);
    Euclid::write_ply<3>(
        fout, spositions, nullptr, nullptr, &sindices, &colors);
}

TEST_CASE("ViewSelection, ViewEntropy", "[viewselection][viewentropy]")
{
    std::string filename(DATA_DIR);
    filename.append("bunny.off");
    std::vector<float> positions;
    std::vector<unsigned> indices;
    Euclid::read_off<3>(filename, positions, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    auto sphere = Euclid::ViewSphere<Mesh>::make_subdiv(mesh);
    std::vector<float> view_scores;
    Euclid::view_entropy(mesh, sphere, view_scores);
    write_view_sphere(sphere, view_scores, "view_entropy_sphere.ply");
}
