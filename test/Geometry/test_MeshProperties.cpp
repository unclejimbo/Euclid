#include <catch.hpp>
#include <Euclid/Geometry/MeshProperties.h>

#include <vector>
#include <string>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polyhedron_3.h>
#include <Eigen/Eigenvalues>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Math/Numeric.h>
#include <igl/invert_diag.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = typename Kernel::Point_3;
using Vector_3 = typename Kernel::Vector_3;
using Mesh = CGAL::Polyhedron_3<Kernel>;

TEST_CASE("Geometry, MeshProperties", "[geometry][meshproperties]")
{
    std::string fcube(DATA_DIR);
    fcube.append("cube_ascii.ply");
    std::vector<float> cpositions;
    std::vector<int> cindices;
    Euclid::read_ply<3>(
        fcube, cpositions, nullptr, nullptr, &cindices, nullptr);
    Mesh cube;
    Euclid::make_mesh<3>(cube, cpositions, cindices);

    std::string fbumpy(DATA_DIR);
    fbumpy.append("bumpy.off");
    std::vector<float> bpositions;
    std::vector<int> bindices;
    Euclid::read_off<3>(fbumpy, bpositions, bindices);
    Mesh bumpy;
    Euclid::make_mesh<3>(bumpy, bpositions, bindices);
    Eigen::MatrixXf bumpy_v;
    Eigen::MatrixXi bumpy_f;
    Euclid::make_mesh<3>(bumpy_v, bumpy_f, bpositions, bindices);

    SECTION("vertex normal using uniform weight")
    {
        std::vector<float> vertex_normals;
        for (const auto& v : vertices(bumpy)) {
            auto vn =
                Euclid::vertex_normal(v, bumpy, Euclid::VertexNormal::uniform);
            vertex_normals.push_back(vn.x());
            vertex_normals.push_back(vn.y());
            vertex_normals.push_back(vn.z());
        }
        for (auto& n : vertex_normals) {
            n = (n + 1.0f) * 127.5f;
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_vn_uniform.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &vertex_normals);
    }

    SECTION("vertex normal using face area as weight")
    {
        std::vector<float> vertex_normals;
        for (const auto& v : vertices(bumpy)) {
            auto vn = Euclid::vertex_normal(
                v, bumpy, Euclid::VertexNormal::face_area);
            vertex_normals.push_back(vn.x());
            vertex_normals.push_back(vn.y());
            vertex_normals.push_back(vn.z());
        }
        for (auto& n : vertex_normals) {
            n = (n + 1.0f) * 127.5f;
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_vn_area.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &vertex_normals);
    }

    SECTION("vertex normal using incident angle as weight")
    {
        std::vector<float> vertex_normals;
        for (const auto& v : vertices(bumpy)) {
            auto vn = Euclid::vertex_normal(
                v, bumpy, Euclid::VertexNormal::incident_angle);
            vertex_normals.push_back(vn.x());
            vertex_normals.push_back(vn.y());
            vertex_normals.push_back(vn.z());
        }
        for (auto& n : vertex_normals) {
            n = (n + 1.0f) * 127.5f;
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_vn_angle.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &vertex_normals);
    }

    SECTION("vertex area using barycentric method")
    {
        std::vector<float> vertex_areas;
        for (const auto& v : vertices(bumpy)) {
            auto va =
                Euclid::vertex_area(v, bumpy, Euclid::VertexArea::barycentric);
            vertex_areas.push_back(va);
            vertex_areas.push_back(0.0f);
            vertex_areas.push_back(0.0f);
        }
        auto amax = *std::max_element(vertex_areas.begin(), vertex_areas.end());
        amax = 1.0f / amax;
        for (auto& a : vertex_areas) {
            a *= amax * 255.0f;
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_va_barycentric.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &vertex_areas);
    }

    SECTION("vertex area using voronoi method")
    {
        std::vector<float> vertex_areas;
        for (const auto& v : vertices(bumpy)) {
            auto va =
                Euclid::vertex_area(v, bumpy, Euclid::VertexArea::voronoi);
            vertex_areas.push_back(va);
            vertex_areas.push_back(0.0f);
            vertex_areas.push_back(0.0f);
        }
        auto amax = *std::max_element(vertex_areas.begin(), vertex_areas.end());
        amax = 1.0f / amax;
        for (auto& a : vertex_areas) {
            a *= amax * 255.0f;
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_va_voronoi.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &vertex_areas);
    }

    SECTION("vertex area using mixed method")
    {
        std::vector<float> vertex_areas;
        for (const auto& v : vertices(bumpy)) {
            auto va = Euclid::vertex_area(v, bumpy, Euclid::VertexArea::mixed);
            vertex_areas.push_back(va);
            vertex_areas.push_back(0.0f);
            vertex_areas.push_back(0.0f);
        }
        auto amax = *std::max_element(vertex_areas.begin(), vertex_areas.end());
        amax = 1.0f / amax;
        for (auto& a : vertex_areas) {
            a *= amax * 255.0f;
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_va_mixed.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &vertex_areas);
    }

    SECTION("edge length and squared length")
    {
        auto [ebeg, eend] = edges(cube);
        auto e = *(++ebeg);
        auto he = halfedge(e, cube);
        REQUIRE(Euclid::eq_almost(Euclid::edge_length(e, cube), 2.0f));
        REQUIRE(Euclid::eq_almost(Euclid::squared_edge_length(e, cube), 4.0f));
        REQUIRE(Euclid::eq_almost(Euclid::edge_length(he, cube), 2.0f));
        REQUIRE(Euclid::eq_almost(Euclid::squared_edge_length(he, cube), 4.0f));
    }

    SECTION("face normal")
    {
        auto [fbeg, fend] = faces(cube);
        auto f = *fbeg;
        REQUIRE(Euclid::face_normal(f, cube) == Vector_3(0.0f, 0.0f, -1.0f));
    }

    SECTION("face area")
    {
        auto [fbeg, fend] = faces(cube);
        auto f = *fbeg;
        REQUIRE(Euclid::face_area(f, cube) == 2.0f);
    }

    SECTION("vertex normal using pre-computed face normals")
    {
        using FNMap = std::unordered_map<Mesh::Facet_handle, Vector_3>;
        FNMap fnmap;
        for (const auto& f : faces(bumpy)) {
            fnmap.insert({ f, Euclid::face_normal(f, bumpy) });
        }

        for (const auto& v : vertices(bumpy)) {
            Euclid::vertex_normal(
                v, bumpy, boost::make_assoc_property_map(fnmap));
        }
    }

    SECTION("use laplacian matrix to compute mean curvature normals")
    {
        auto laplacian = Euclid::laplacian_matrix(bumpy);
        auto mass = Euclid::mass_matrix(bumpy);
        Eigen::SparseMatrix<float> inv_mass;
        igl::invert_diag(mass, inv_mass);

        // mean curvature normal
        Eigen::MatrixXf hn = -inv_mass * (laplacian * bumpy_v);
        Eigen::MatrixXf norms = hn.rowwise().norm();

        std::vector<float> mean_curv_normals(hn.rows() * 3);
        for (int i = 0; i < hn.rows(); ++i) {
            mean_curv_normals[i * 3 + 0] =
                (hn(i, 0) / norms(i) + 1.0f) * 127.5f;
            mean_curv_normals[i * 3 + 1] =
                (hn(i, 1) / norms(i) + 1.0f) * 127.5f;
            mean_curv_normals[i * 3 + 2] =
                (hn(i, 2) / norms(i) + 1.0f) * 127.5f;
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_mean_curvature_normal.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &mean_curv_normals);

        std::vector<float> mean_curvatures(hn.rows() * 3);
        auto nmax = norms.maxCoeff();
        nmax = 255.0 / nmax;
        for (int i = 0; i < hn.rows(); ++i) {
            mean_curvatures[i * 3 + 0] = norms(i) * nmax;
            mean_curvatures[i * 3 + 1] = 0.0f;
            mean_curvatures[i * 3 + 2] = 0.0f;
        }
        std::string fout2(TMP_DIR);
        fout2.append("bumpy_mean_curvature.ply");
        Euclid::write_ply<3>(
            fout2, bpositions, nullptr, nullptr, &bindices, &mean_curvatures);
    }

    SECTION("gaussian curvature")
    {
        std::vector<float> gaussian_curvatures;
        for (const auto& v : vertices(bumpy)) {
            auto c = Euclid::gaussian_curvature(v, bumpy);
            gaussian_curvatures.push_back(c);
        }
        auto gmax = *std::max_element(gaussian_curvatures.begin(),
                                      gaussian_curvatures.end());
        auto gmin = *std::min_element(gaussian_curvatures.begin(),
                                      gaussian_curvatures.end());
        auto gscale = std::max(std::abs(gmax), std::abs(gmin));
        gscale = 255.0 / gscale;
        std::vector<float> colors;
        for (auto g : gaussian_curvatures) {
            if (g > 0.0f) {
                colors.push_back(g * gscale);
                colors.push_back(0.0f);
                colors.push_back(0.0f);
            }
            else if (g == 0.0f) {
                colors.push_back(0.0f);
                colors.push_back(0.0f);
                colors.push_back(0.0f);
            }
            else {
                colors.push_back(0.0f);
                colors.push_back(-g * gscale);
                colors.push_back(0.0f);
            }
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_gaussian_curvature.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &colors);
    }
}
