#include <catch2/catch.hpp>
#include <Euclid/Analysis/Descriptor.h>

#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Math/Distance.h>
#include <Euclid/Util/Serialize.h>
#include <igl/colormap.h>
#include <stb_image_write.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Vector_3 = Kernel::Vector_3;
using Mesh = CGAL::Surface_mesh<Kernel::Point_3>;
using Vertex = Mesh::Vertex_index;

TEST_CASE("Analysis, Descriptor", "[analysis][descriptor]")
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

    auto idx1 = 21785;
    auto v1 = Vertex(idx1); // left-front foot
    auto idx2 = 24103;
    auto v2 = Vertex(idx2); // vertex nearby
    auto idx3 = 36874;
    auto v3 = Vertex(idx3); // right-front foot
    auto idx4 = 16807;
    auto v4 = Vertex(idx4); // back

    SECTION("spin images")
    {
        const unsigned width = 256;

        Euclid::SpinImage<Mesh> si;
        si.build(mesh);

        Eigen::ArrayXd si_v1;
        si.compute(v1, si_v1, 1.0f, width, 60.0f);
        Eigen::ArrayXd si_v2;
        si.compute(v2, si_v2, 1.0f, width, 60.0f);
        Eigen::ArrayXd si_v3;
        si.compute(v3, si_v3, 1.0f, width, 60.0f);
        Eigen::ArrayXd si_v4;
        si.compute(v4, si_v4, 1.0f, width, 60.0f);

        auto d12 = Euclid::l2(si_v1, si_v2);
        auto d13 = Euclid::l2(si_v1, si_v3);
        auto d14 = Euclid::l2(si_v1, si_v4);
        REQUIRE(d12 < d13);
        REQUIRE(d13 < d14);

        Eigen::ArrayXXd si_all;
        si.compute(si_all, 1.0f, width, 60.0f);

        REQUIRE(si_v1.isApprox(si_all.col(idx1)));
        REQUIRE(si_v2.isApprox(si_all.col(idx2)));
        REQUIRE(si_v3.isApprox(si_all.col(idx3)));
        REQUIRE(si_v4.isApprox(si_all.col(idx4)));

        Euclid::SpinImage<Mesh> si1;
        si1.build(mesh, si.vnormals.get(), si.resolution);

        Eigen::ArrayXd si1_v1;
        si1.compute(v1, si1_v1, 1.0f, width, 60.0f);
        Eigen::ArrayXd si1_v2;
        si1.compute(v2, si1_v2, 1.0f, width, 60.0f);
        Eigen::ArrayXd si1_v3;
        si1.compute(v3, si1_v3, 1.0f, width, 60.0f);
        Eigen::ArrayXd si1_v4;
        si1.compute(v4, si1_v4, 1.0f, width, 60.0f);

        REQUIRE(si_v1.isApprox(si1_v1));
        REQUIRE(si_v2.isApprox(si1_v2));
        REQUIRE(si_v3.isApprox(si1_v3));
        REQUIRE(si_v4.isApprox(si1_v4));

        SECTION("output to image")
        {
            {
                auto vmax = si_v1.maxCoeff();
                si_v1 *= 255.0 / vmax;
                std::string fout(TMP_DIR);
                fout.append("spinimage1.png");
                stbi_write_png(fout.c_str(),
                               width,
                               width,
                               1,
                               &si_v1(0),
                               width * sizeof(double));
            }
            {
                auto vmax = si_v2.maxCoeff();
                si_v2 *= 255.0 / vmax;
                std::string fout(TMP_DIR);
                fout.append("spinimage2.png");
                stbi_write_png(fout.c_str(),
                               width,
                               width,
                               1,
                               &si_v2(0),
                               width * sizeof(double));
            }
            {
                auto vmax = si_v3.maxCoeff();
                si_v3 *= 255.0 / vmax;
                std::string fout(TMP_DIR);
                fout.append("spinimage3.png");
                stbi_write_png(fout.c_str(),
                               width,
                               width,
                               1,
                               &si_v3(0),
                               width * sizeof(double));
            }
            {
                auto vmax = si_v4.maxCoeff();
                si_v4 *= 255.0 / vmax;
                std::string fout(TMP_DIR);
                fout.append("spinimage4.png");
                stbi_write_png(fout.c_str(),
                               width,
                               width,
                               1,
                               &si_v4(0),
                               width * sizeof(double));
            }
        }
    }

    SECTION("heat kernel signature")
    {
        constexpr const int ne = 100;
        std::string fcereal(TMP_DIR);
        fcereal.append("hks.cereal");
        Euclid::HKS<Mesh> hks;
        Eigen::VectorXd eigenvalues;
        Eigen::MatrixXd eigenfunctions;
        try {
            Euclid::deserialize(fcereal, eigenvalues, eigenfunctions);
            if (eigenvalues.rows() != ne ||
                eigenvalues.rows() != eigenfunctions.cols() ||
                eigenfunctions.rows() != positions.size() / 3) {
                throw std::runtime_error("Need to be build again.");
            }
            hks.build(mesh, &eigenvalues, &eigenfunctions);
        }
        catch (const std::exception& e) {
            hks.build(mesh, ne);
            Euclid::serialize(fcereal, *hks.eigenvalues, *hks.eigenfunctions);
        }

        SECTION("default time range")
        {
            Eigen::ArrayXXd hks_all;
            hks.compute(hks_all);

            std::vector<double> distances(hks_all.cols());
            for (size_t i = 0; i < hks_all.cols(); ++i) {
                distances[i] = Euclid::chi2(hks_all.col(i), hks_all.col(idx1));
            }
            REQUIRE(distances[idx1] == 0);
            REQUIRE(distances[idx2] < distances[idx3]);
            REQUIRE(distances[idx3] < distances[idx4]);

            // colorize output
            std::vector<unsigned char> colors;
            colors.reserve(hks_all.cols() * 3);
            auto dmax = std::max_element(distances.begin(), distances.end());
            auto dmaxid = dmax - distances.begin();
            for (auto d : distances) {
                double r, g, b;
                igl::colormap(
                    igl::COLOR_MAP_TYPE_JET, (*dmax - d) / *dmax, r, g, b);
                colors.push_back(static_cast<unsigned char>(r * 255));
                colors.push_back(static_cast<unsigned char>(g * 255));
                colors.push_back(static_cast<unsigned char>(b * 255));
            }
            std::string fout(TMP_DIR);
            fout.append("hks1.ply");
            Euclid::write_ply<3>(
                fout, positions, nullptr, nullptr, &indices, &colors);
        }

        SECTION("smaller time range")
        {
            auto c = std::log(10.0);
            auto tmin = c / hks.eigenvalues->coeff(hks.eigenvalues->size() - 1);
            auto tmax = c / hks.eigenvalues->coeff(1);
            Eigen::ArrayXXd hks_all;
            hks.compute(hks_all, 100, tmin, tmax);

            std::vector<double> distances(hks_all.cols());
            for (size_t i = 0; i < hks_all.cols(); ++i) {
                distances[i] = Euclid::chi2(hks_all.col(i), hks_all.col(idx1));
            }
            REQUIRE(distances[idx1] == 0);
            REQUIRE(distances[idx2] < distances[idx3]);
            REQUIRE(distances[idx3] < distances[idx4]);

            // colorize output
            std::vector<unsigned char> colors;
            colors.reserve(hks_all.cols() * 3);
            auto dmax = std::max_element(distances.begin(), distances.end());
            auto dmaxid = dmax - distances.begin();
            for (auto d : distances) {
                double r, g, b;
                igl::colormap(
                    igl::COLOR_MAP_TYPE_JET, (*dmax - d) / *dmax, r, g, b);
                colors.push_back(static_cast<unsigned char>(r * 255));
                colors.push_back(static_cast<unsigned char>(g * 255));
                colors.push_back(static_cast<unsigned char>(b * 255));
            }
            std::string fout(TMP_DIR);
            fout.append("hks2.ply");
            Euclid::write_ply<3>(
                fout, positions, nullptr, nullptr, &indices, &colors);
        }

        SECTION("larger time range")
        {
            auto c = 12 * std::log(10.0);
            auto tmin = c / hks.eigenvalues->coeff(hks.eigenvalues->size() - 1);
            auto tmax = c / hks.eigenvalues->coeff(1);
            Eigen::ArrayXXd hks_all;
            hks.compute(hks_all, 100, tmin, tmax);

            std::vector<double> distances(hks_all.cols());
            for (size_t i = 0; i < hks_all.cols(); ++i) {
                distances[i] = Euclid::chi2(hks_all.col(i), hks_all.col(idx1));
            }
            REQUIRE(distances[idx1] == 0);
            REQUIRE(distances[idx2] < distances[idx3]);
            REQUIRE(distances[idx3] < distances[idx4]);

            // colorize output
            std::vector<unsigned char> colors;
            colors.reserve(hks_all.cols() * 3);
            auto dmax = std::max_element(distances.begin(), distances.end());
            auto dmaxid = dmax - distances.begin();
            for (auto d : distances) {
                double r, g, b;
                igl::colormap(
                    igl::COLOR_MAP_TYPE_JET, (*dmax - d) / *dmax, r, g, b);
                colors.push_back(static_cast<unsigned char>(r * 255));
                colors.push_back(static_cast<unsigned char>(g * 255));
                colors.push_back(static_cast<unsigned char>(b * 255));
            }
            std::string fout(TMP_DIR);
            fout.append("hks3.ply");
            Euclid::write_ply<3>(
                fout, positions, nullptr, nullptr, &indices, &colors);
        }
    }
}
