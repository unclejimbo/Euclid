#include <Euclid/Analysis/ViewSelection.h>
#include <catch.hpp>

#include <algorithm>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <igl/colormap.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

TEST_CASE("Package: Analysis/ViewSelection", "[viewselection]")
{
    std::string filename(DATA_DIR);
    filename.append("bunny.off");
    std::vector<float> positions;
    std::vector<unsigned> indices;
    Euclid::read_off<3>(filename, positions, indices);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    SECTION("subdivision view sphere")
    {
        auto sphere = Euclid::ViewSphere<Mesh>::make_subdiv(mesh);
        std::vector<float> spositions;
        std::vector<float> sindices;
        Euclid::extract_mesh<3>(sphere.mesh, spositions, sindices);
        std::string fout(TMP_DIR);
        fout.append("view_sphere_subdiv.off");
        Euclid::write_off<3>(fout, spositions, sindices);
    }

    SECTION("random view sphere")
    {
        auto sphere = Euclid::ViewSphere<Mesh>::make_random(mesh, 3.0f, 5000);
        std::vector<float> spositions;
        std::vector<float> sindices;
        Euclid::extract_mesh<3>(sphere.mesh, spositions, sindices);
        std::string fout(TMP_DIR);
        fout.append("view_sphere_uniform.off");
        Euclid::write_off<3>(fout, spositions, sindices);
    }

    SECTION("proxy view selection")
    {
        auto view_sphere = Euclid::ViewSphere<Mesh>::make_subdiv(mesh);
        std::vector<float> view_scores;
        Euclid::proxy_view_selection(mesh, view_sphere, view_scores);

        std::vector<float> vpositions;
        std::vector<unsigned> vindices;
        Euclid::extract_mesh<3>(view_sphere.mesh, vpositions, vindices);
        auto [smin, smax] =
            std::minmax_element(view_scores.begin(), view_scores.end());
        std::vector<unsigned char> colors;
        colors.reserve(view_scores.size() * 4);
        for (const auto& s : view_scores) {
            auto score = (s - *smin) / (*smax - *smin);
            float r, g, b;
            igl::colormap(igl::COLOR_MAP_TYPE_JET, score, r, g, b);
            colors.push_back(static_cast<unsigned char>(r * 255));
            colors.push_back(static_cast<unsigned char>(g * 255));
            colors.push_back(static_cast<unsigned char>(b * 255));
            colors.push_back(128);
        }
        std::string mesh_out(TMP_DIR);
        mesh_out.append("proxy_view_sphere.ply");
        Euclid::write_ply<3>(
            mesh_out, vpositions, nullptr, nullptr, &vindices, &colors);
    }
}
