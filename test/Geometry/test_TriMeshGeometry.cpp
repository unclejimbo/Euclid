#include <catch2/catch.hpp>
#include <Euclid/Geometry/TriMeshGeometry.h>

#include <string>
#include <vector>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <Eigen/SparseCholesky>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/MeshUtil/MeshHelpers.h>
#include <Euclid/Util/Color.h>
#include <igl/invert_diag.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = typename Kernel::Point_3;
using Vector_3 = typename Kernel::Vector_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

TEST_CASE("Geometry, TriMeshGeometry", "[geometry][trimeshgeometry]")
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
    Euclid::read_off<3>(fbumpy, bpositions, nullptr, &bindices, nullptr);
    Mesh bumpy;
    Euclid::make_mesh<3>(bumpy, bpositions, bindices);
    Eigen::MatrixXf bumpy_v;
    Eigen::MatrixXi bumpy_f;
    Euclid::make_mesh<3>(bumpy_v, bumpy_f, bpositions, bindices);

    SECTION("vertex normal")
    {
        auto fnormals = Euclid::face_normals(bumpy);

        SECTION("uniform")
        {
            auto w = Euclid::VertexNormal::uniform;
            std::vector<float> vnormals1;
            for (auto v : vertices(bumpy)) {
                auto vn1 = Euclid::vertex_normal(v, bumpy, w);
                auto vn2 = Euclid::vertex_normal(v, bumpy, fnormals, w);
                REQUIRE(vn1 == vn2);
                vnormals1.push_back(vn1.x());
                vnormals1.push_back(vn1.y());
                vnormals1.push_back(vn1.z());
            }
            auto vnormals2 = Euclid::vertex_normals(bumpy, fnormals, w);
            REQUIRE(vnormals1.size() == vnormals2.size() * 3);
            REQUIRE(vnormals1[0] == vnormals2[0].x());
            REQUIRE(vnormals1[1] == vnormals2[0].y());
            REQUIRE(vnormals1[2] == vnormals2[0].z());

            for (auto& n : vnormals1) {
                n = (n + 1.0f) * 127.5f;
            }
            std::string fout(TMP_DIR);
            fout.append("bumpy_vn_uniform.ply");
            Euclid::write_ply<3>(
                fout, bpositions, nullptr, nullptr, &bindices, &vnormals1);
        }

        SECTION("face_area")
        {
            auto w = Euclid::VertexNormal::face_area;
            std::vector<float> vnormals1;
            for (auto v : vertices(bumpy)) {
                auto vn1 = Euclid::vertex_normal(v, bumpy, w);
                auto vn2 = Euclid::vertex_normal(v, bumpy, fnormals, w);
                REQUIRE(vn1 == vn2);
                vnormals1.push_back(vn1.x());
                vnormals1.push_back(vn1.y());
                vnormals1.push_back(vn1.z());
            }
            auto vnormals2 = Euclid::vertex_normals(bumpy, fnormals, w);
            REQUIRE(vnormals1.size() == vnormals2.size() * 3);
            REQUIRE(vnormals1[0] == vnormals2[0].x());
            REQUIRE(vnormals1[1] == vnormals2[0].y());
            REQUIRE(vnormals1[2] == vnormals2[0].z());

            for (auto& n : vnormals1) {
                n = (n + 1.0f) * 127.5f;
            }
            std::string fout(TMP_DIR);
            fout.append("bumpy_vn_area.ply");
            Euclid::write_ply<3>(
                fout, bpositions, nullptr, nullptr, &bindices, &vnormals1);
        }

        SECTION("incident angle")
        {
            auto w = Euclid::VertexNormal::incident_angle;
            std::vector<float> vnormals1;
            for (auto v : vertices(bumpy)) {
                auto vn1 = Euclid::vertex_normal(v, bumpy, w);
                auto vn2 = Euclid::vertex_normal(v, bumpy, fnormals, w);
                REQUIRE(vn1 == vn2);
                vnormals1.push_back(vn1.x());
                vnormals1.push_back(vn1.y());
                vnormals1.push_back(vn1.z());
            }
            auto vnormals2 = Euclid::vertex_normals(bumpy, fnormals, w);
            REQUIRE(vnormals1.size() == vnormals2.size() * 3);
            REQUIRE(vnormals1[0] == vnormals2[0].x());
            REQUIRE(vnormals1[1] == vnormals2[0].y());
            REQUIRE(vnormals1[2] == vnormals2[0].z());

            for (auto& n : vnormals1) {
                n = (n + 1.0f) * 127.5f;
            }
            std::string fout(TMP_DIR);
            fout.append("bumpy_vn_angle.ply");
            Euclid::write_ply<3>(
                fout, bpositions, nullptr, nullptr, &bindices, &vnormals1);
        }
    }

    SECTION("veretx area")
    {
        SECTION("barycentric")
        {
            auto w = Euclid::VertexArea::barycentric;
            std::vector<float> vareas1;
            for (auto v : vertices(bumpy)) {
                auto va = Euclid::vertex_area(v, bumpy, w);
                vareas1.push_back(va);
                vareas1.push_back(0.0f);
                vareas1.push_back(0.0f);
            }
            auto vareas2 = Euclid::vertex_areas(bumpy, w);
            REQUIRE(vareas1.size() == vareas2.size() * 3);
            REQUIRE(vareas1[0] == vareas2[0]);

            auto amax = *std::max_element(vareas1.begin(), vareas1.end());
            amax = 1.0f / amax;
            for (auto& a : vareas1) {
                a *= amax * 255.0f;
            }
            std::string fout(TMP_DIR);
            fout.append("bumpy_va_barycentric.ply");
            Euclid::write_ply<3>(
                fout, bpositions, nullptr, nullptr, &bindices, &vareas1);
        }

        SECTION("voronoi")
        {
            auto w = Euclid::VertexArea::voronoi;
            std::vector<float> vareas1;
            for (auto v : vertices(bumpy)) {
                auto va = Euclid::vertex_area(v, bumpy, w);
                vareas1.push_back(va);
                vareas1.push_back(0.0f);
                vareas1.push_back(0.0f);
            }
            auto vareas2 = Euclid::vertex_areas(bumpy, w);
            REQUIRE(vareas1.size() == vareas2.size() * 3);
            REQUIRE(vareas1[0] == vareas2[0]);

            auto amax = *std::max_element(vareas1.begin(), vareas1.end());
            amax = 1.0f / amax;
            for (auto& a : vareas1) {
                a *= amax * 255.0f;
            }
            std::string fout(TMP_DIR);
            fout.append("bumpy_va_voronoi.ply");
            Euclid::write_ply<3>(
                fout, bpositions, nullptr, nullptr, &bindices, &vareas1);
        }

        SECTION("mixed voronoi")
        {
            auto w = Euclid::VertexArea::mixed_voronoi;
            std::vector<float> vareas1;
            for (auto v : vertices(bumpy)) {
                auto va = Euclid::vertex_area(v, bumpy, w);
                vareas1.push_back(va);
                vareas1.push_back(0.0f);
                vareas1.push_back(0.0f);
            }
            auto vareas2 = Euclid::vertex_areas(bumpy, w);
            REQUIRE(vareas1.size() == vareas2.size() * 3);
            REQUIRE(vareas1[0] == vareas2[0]);

            auto amax = *std::max_element(vareas1.begin(), vareas1.end());
            amax = 1.0f / amax;
            for (auto& a : vareas1) {
                a *= amax * 255.0f;
            }
            std::string fout(TMP_DIR);
            fout.append("bumpy_va_mixed.ply");
            Euclid::write_ply<3>(
                fout, bpositions, nullptr, nullptr, &bindices, &vareas1);
        }
    }

    SECTION("edge length and squared length")
    {
        auto [ebeg, eend] = edges(cube);
        auto e = *(++ebeg);
        auto he = halfedge(e, cube);
        auto elens = Euclid::edge_lengths(cube);
        auto sq_elens = Euclid::squared_edge_lengths(cube);

        REQUIRE(Euclid::edge_length(e, cube) == Approx(2.0f));
        REQUIRE(Euclid::edge_length(he, cube) == Approx(2.0f));
        REQUIRE(elens[1] == Approx(2.0f));
        REQUIRE(elens.size() == num_edges(cube));

        REQUIRE(Euclid::squared_edge_length(e, cube) == Approx(4.0f));
        REQUIRE(Euclid::squared_edge_length(he, cube) == Approx(4.0f));
        REQUIRE(sq_elens[1] == Approx(4.0f));
        REQUIRE(sq_elens.size() == num_edges(cube));
    }

    SECTION("face normal")
    {
        auto [fbeg, fend] = faces(cube);
        auto f = *fbeg;
        REQUIRE(Euclid::face_normal(f, cube) == Vector_3(0.0f, 0.0f, -1.0f));

        auto fnormals = Euclid::face_normals(cube);
        REQUIRE(fnormals.size() == num_faces(cube));
        REQUIRE(fnormals[0] == Vector_3(0.0f, 0.0f, -1.0f));
    }

    SECTION("face area")
    {
        auto [fbeg, fend] = faces(cube);
        auto f = *fbeg;
        auto fareas = Euclid::face_areas(cube);

        REQUIRE(Euclid::face_area(f, cube) == 2.0f);
        REQUIRE(fareas.size() == num_faces(cube));
        REQUIRE(fareas[0] == 2.0f);
    }

    SECTION("face barycenter")
    {
        auto [fbeg, fend] = faces(cube);
        auto f = *fbeg;
        auto c = Euclid::barycenter(f, cube);
        auto centroids = Euclid::barycenters(cube);

        REQUIRE(c.x() == Approx(-1.0 / 3.0));
        REQUIRE(c.y() == Approx(1.0 / 3.0));
        REQUIRE(c.z() == -1.0);
        REQUIRE(centroids[0] == c);
    }

    SECTION("gaussian curvature")
    {
        std::vector<float> gaussian_curvatures;
        for (auto v : vertices(bumpy)) {
            auto c = Euclid::gaussian_curvature(v, bumpy);
            gaussian_curvatures.push_back(c);
        }

        auto [vbeg, vend] = vertices(bumpy);
        auto curvatures = Euclid::gaussian_curvatures(bumpy);
        REQUIRE(Euclid::gaussian_curvature(*vbeg, bumpy) == curvatures[0]);
        REQUIRE(curvatures.size() == num_vertices(bumpy));

        std::vector<uint8_t> colors;
        Euclid::colormap(igl::COLOR_MAP_TYPE_JET, curvatures, colors, true);
        std::string fout(TMP_DIR);
        fout.append("bumpy_gaussian_curvature.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &colors);
    }

    SECTION("mean curvature w/ laplace beltrami operator")
    {
        auto laplacian = Euclid::cotangent_matrix(bumpy);
        auto mass = Euclid::mass_matrix(bumpy);
        Eigen::SparseMatrix<float> inv_mass;
        igl::invert_diag(mass, inv_mass);
        Eigen::MatrixXf hn = inv_mass * laplacian * bumpy_v;
        Eigen::VectorXf norms = hn.rowwise().norm();

        std::vector<float> mean_curvatures(hn.rows() * 3);
        auto nmax = norms.maxCoeff();
        nmax = 255.0 / nmax;
        for (int i = 0; i < hn.rows(); ++i) {
            mean_curvatures[i * 3 + 0] = norms(i) * nmax;
            mean_curvatures[i * 3 + 1] = 0.0f;
            mean_curvatures[i * 3 + 2] = 0.0f;
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_mean_curvature_lbo.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &mean_curvatures);
    }

    SECTION("mean curvature w/ graph laplacian operator")
    {
        auto [adj, degree] = Euclid::adjacency_matrix(bumpy);
        auto laplacian = degree - adj;
        Eigen::SparseMatrix<float> inv_degree;
        igl::invert_diag(degree, inv_degree);
        Eigen::MatrixXf hn = inv_degree * laplacian * bumpy_v;
        Eigen::VectorXf norms = hn.rowwise().norm();

        std::vector<float> mean_curvatures(hn.rows() * 3);
        auto nmax = norms.maxCoeff();
        nmax = 255.0 / nmax;
        for (int i = 0; i < hn.rows(); ++i) {
            mean_curvatures[i * 3 + 0] = norms(i) * nmax;
            mean_curvatures[i * 3 + 1] = 0.0f;
            mean_curvatures[i * 3 + 2] = 0.0f;
        }
        std::string fout(TMP_DIR);
        fout.append("bumpy_mean_curvature_gl.ply");
        Euclid::write_ply<3>(
            fout, bpositions, nullptr, nullptr, &bindices, &mean_curvatures);
    }
}
