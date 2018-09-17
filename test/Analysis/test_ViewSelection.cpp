#include <catch2/catch.hpp>
#include <Euclid/Analysis/ViewSelection.h>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Util/Color.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

static void write_view_sphere(const Euclid::ViewSphere<Mesh>& sphere,
                              const std::string& name)
{
    std::vector<float> spositions;
    std::vector<float> sindices;
    Euclid::extract_mesh<3>(sphere.mesh, spositions, sindices);
    std::string fout(TMP_DIR);
    fout.append(name);
    Euclid::write_off<3>(fout, spositions, nullptr, &sindices, nullptr);
}

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

TEST_CASE("Analysis, ViewSelection", "[analysis][viewselection]")
{
    std::string filename(DATA_DIR);
    filename.append("bunny.off");
    std::vector<float> positions;
    std::vector<unsigned> indices;
    Euclid::read_off<3>(filename, positions, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    SECTION("subdivision view sphere")
    {
        auto sphere = Euclid::ViewSphere<Mesh>::make_subdiv(mesh);
        write_view_sphere(sphere, "view_sphere_subdiv.off");
    }

    SECTION("random view sphere")
    {
        auto sphere = Euclid::ViewSphere<Mesh>::make_random(mesh, 3.0f, 5000);
        write_view_sphere(sphere, "view_sphere_uniform.off");
    }

    SECTION("view entropy")
    {
        auto sphere = Euclid::ViewSphere<Mesh>::make_subdiv(mesh);
        std::vector<float> view_scores;
        Euclid::vs_view_entropy(mesh, sphere, view_scores);
        write_view_sphere(sphere, view_scores, "view_entropy_sphere.ply");
    }

    SECTION("proxy view selection")
    {
        auto sphere = Euclid::ViewSphere<Mesh>::make_subdiv(mesh);
        std::vector<float> view_scores;
        Euclid::proxy_view_selection(mesh, sphere, view_scores);
        write_view_sphere(sphere, view_scores, "proxy_view_sphere.ply");
    }
}
