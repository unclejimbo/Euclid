#include <catch2/catch.hpp>
#include <Euclid/Descriptor/WKS.h>

#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/MeshUtil/CGALMesh.h>
#include <Euclid/Geometry/Spectral.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Descriptor/Histogram.h>
#include <Euclid/Util/Color.h>
#include <stb_image_write.h>

#include <config.h>

#ifdef EUCLID_TEST_ENABLE_CEREAL
#include <Euclid/Util/Serialize.h>
#endif

using Kernel = CGAL::Simple_cartesian<double>;
using Vector_3 = Kernel::Vector_3;
using Mesh = CGAL::Surface_mesh<Kernel::Point_3>;
using Vertex = Mesh::Vertex_index;

static void _write_distances_to_colored_mesh(
    const std::string& f,
    const std::vector<double>& positions,
    const std::vector<unsigned>& indices,
    const std::vector<double>& distances)
{
    std::vector<unsigned char> colors;
    Euclid::colormap(
        igl::COLOR_MAP_TYPE_JET, distances, colors, true, false, true);
    std::string fout(TMP_DIR);
    fout.append(f);
    Euclid::write_ply<3>(fout, positions, nullptr, nullptr, &indices, &colors);
}

TEST_CASE("Descriptor, WKS", "[descriptor][wks]")
{
    std::vector<double> positions;
    std::vector<double> normals;
    std::vector<unsigned> indices;
    std::string filename(DATA_DIR);
    filename.append("dragon.ply");
    Euclid::read_ply<3>(
        filename, positions, &normals, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    auto idx1 = 21785; // left-front foot
    auto idx2 = 24103; // nearby point
    auto idx3 = 36874; // right-front foot
    auto idx4 = 16807; // far away

    constexpr const int ne = 300;
    Eigen::VectorXd eigenvalues;
    Eigen::MatrixXd eigenfunctions;
#ifdef EUCLID_TEST_ENABLE_CEREAL
    std::string fcereal(TMP_DIR);
    fcereal.append("dragon_eigs.cereal");
    try {
        Euclid::deserialize(fcereal, eigenvalues, eigenfunctions);
        if (eigenvalues.rows() != ne ||
            eigenvalues.rows() != eigenfunctions.cols() ||
            eigenfunctions.rows() != static_cast<int>(positions.size()) / 3) {
            throw std::runtime_error("Need to be solved again.");
        }
    }
    catch (const std::exception& e) {
        Euclid::spectrum(mesh, ne, eigenvalues, eigenfunctions);
        Euclid::serialize(fcereal, eigenvalues, eigenfunctions);
    }
#else
    Euclid::spectrum(mesh, ne, eigenvalues, eigenfunctions);
#endif

    Euclid::WKS<Mesh> wks;
    wks.build(mesh, &eigenvalues, &eigenfunctions);

    SECTION("default energy range")
    {
        Eigen::ArrayXXd wks_all;
        wks.compute(wks_all);

        std::vector<double> distances(wks_all.cols());
        for (int i = 0; i < wks_all.cols(); ++i) {
            distances[i] = Euclid::chi2(wks_all.col(i), wks_all.col(idx1));
        }
        REQUIRE(distances[idx1] == 0);
        REQUIRE(distances[idx2] < distances[idx3]);
        REQUIRE(distances[idx3] < distances[idx4]);

        _write_distances_to_colored_mesh(
            "wks1.ply", positions, indices, distances);
    }

    SECTION("smaller energy range")
    {
        Eigen::Matrix3f A;
        Eigen::Vector3f B;
        A << 1.0f, 0.0f, -2.0f, 0.0f, 1.0f, 2.0f, 7.0f, -7.0f, 100.0f;
        B << std::log(std::abs(eigenvalues(1))),
            std::log(eigenvalues(eigenvalues.size() - 1)), 0.0f;
        Eigen::Vector3f x = A.colPivHouseholderQr().solve(B);
        float emin = x(0);
        float emax = x(1) - 0.95f * (x(1) - x(0));
        float sigma = x(2);

        Eigen::ArrayXXd wks_all;
        wks.compute(wks_all, 100, emin, emax, sigma);

        std::vector<double> distances(wks_all.cols());
        for (int i = 0; i < wks_all.cols(); ++i) {
            distances[i] = Euclid::chi2(wks_all.col(i), wks_all.col(idx1));
        }
        REQUIRE(distances[idx1] == 0);
        REQUIRE(distances[idx2] < distances[idx3]);
        REQUIRE(distances[idx3] < distances[idx4]);

        _write_distances_to_colored_mesh(
            "wks2.ply", positions, indices, distances);
    }

    SECTION("larger energy range")
    {
        Eigen::Matrix3f A;
        Eigen::Vector3f B;
        A << 1.0f, 0.0f, -2.0f, 0.0f, 1.0f, 2.0f, 7.0f, -7.0f, 100.0f;
        B << std::log(std::abs(eigenvalues(1))),
            std::log(eigenvalues(eigenvalues.size() - 1)), 0.0f;
        Eigen::Vector3f x = A.colPivHouseholderQr().solve(B);
        float emin = x(0);
        float emax = x(1) + 2.0f * (x(1) - x(0));
        float sigma = x(2);

        Eigen::ArrayXXd wks_all;
        wks.compute(wks_all, 100, emin, emax, sigma);

        std::vector<double> distances(wks_all.cols());
        for (int i = 0; i < wks_all.cols(); ++i) {
            distances[i] = Euclid::chi2(wks_all.col(i), wks_all.col(idx1));
        }
        REQUIRE(distances[idx1] == 0);
        REQUIRE(distances[idx2] < distances[idx3]);
        REQUIRE(distances[idx3] < distances[idx4]);

        _write_distances_to_colored_mesh(
            "wks3.ply", positions, indices, distances);
    }
}
