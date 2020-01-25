#include <catch2/catch.hpp>
#include <Euclid/Descriptor/HKS.h>

#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/MeshUtil/MeshHelpers.h>
#include <Euclid/Geometry/Spectral.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Descriptor/Histogram.h>
#include <Euclid/Util/Color.h>
#include <Euclid/Util/Serialize.h>
#include <stb_image_write.h>

#include <config.h>

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

TEST_CASE("Descriptor,HKS", "[descriptor][hks]")
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
    std::string fcereal(TMP_DIR);
    fcereal.append("dragon_eigs.cereal");
    Eigen::VectorXd eigenvalues;
    Eigen::MatrixXd eigenfunctions;
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

    Euclid::HKS<Mesh> hks;
    hks.build(mesh, &eigenvalues, &eigenfunctions);

    SECTION("default time range")
    {
        Eigen::ArrayXXd hks_all;
        hks.compute(hks_all);

        std::vector<double> distances(hks_all.cols());
        for (int i = 0; i < hks_all.cols(); ++i) {
            distances[i] = Euclid::chi2(hks_all.col(i), hks_all.col(idx1));
        }
        REQUIRE(distances[idx1] == 0);
        REQUIRE(distances[idx2] < distances[idx3]);
        REQUIRE(distances[idx3] < distances[idx4]);

        _write_distances_to_colored_mesh(
            "hks1.ply", positions, indices, distances);
    }

    SECTION("smaller time range")
    {
        auto c = std::log(10.0);
        auto tmin = c / eigenvalues(eigenvalues.size() - 1);
        auto tmax = c / eigenvalues(1);
        Eigen::ArrayXXd hks_all;
        hks.compute(hks_all, 100, tmin, tmax);

        std::vector<double> distances(hks_all.cols());
        for (int i = 0; i < hks_all.cols(); ++i) {
            distances[i] = Euclid::chi2(hks_all.col(i), hks_all.col(idx1));
        }
        REQUIRE(distances[idx1] == 0);
        REQUIRE(distances[idx2] < distances[idx3]);
        REQUIRE(distances[idx3] < distances[idx4]);

        _write_distances_to_colored_mesh(
            "hks2.ply", positions, indices, distances);
    }

    SECTION("larger time range")
    {
        auto c = 12 * std::log(10.0);
        auto tmin = c / eigenvalues(eigenvalues.size() - 1);
        auto tmax = c / eigenvalues(1);
        Eigen::ArrayXXd hks_all;
        hks.compute(hks_all, 100, tmin, tmax);

        std::vector<double> distances(hks_all.cols());
        for (int i = 0; i < hks_all.cols(); ++i) {
            distances[i] = Euclid::chi2(hks_all.col(i), hks_all.col(idx1));
        }
        REQUIRE(distances[idx1] == 0);
        REQUIRE(distances[idx2] < distances[idx3]);
        REQUIRE(distances[idx3] < distances[idx4]);

        _write_distances_to_colored_mesh(
            "hks3.ply", positions, indices, distances);
    }
}
