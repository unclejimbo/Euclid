#include <Euclid/Analysis/Descriptor.h>
#include <catch.hpp>

#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Math/Distance.h>
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
    filename.append("bunny_vn.ply");
    Euclid::read_ply<3>(
        filename, positions, &normals, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);
    std::vector<Vector_3> vnormals;
    vnormals.reserve(num_vertices(mesh));
    for (size_t i = 0; i < normals.size(); i += 3) {
        vnormals.emplace_back(normals[i], normals[i + 1], normals[i + 2]);
    }

    auto v1 = Vertex(7505); // vertex on ear
    auto v2 = Vertex(695);  // vertex next by
    auto v3 = Vertex(3915); // vertex on the other ear
    auto v4 = Vertex(0);    // vertex far away

    SECTION("spin images")
    {
        const unsigned width = 256;

        Euclid::SpinImage si(mesh);

        Eigen::ArrayXd si1;
        si.compute(v1, si1, 1.0f, width, 60.0f);

        Eigen::ArrayXd si2;
        si.compute(v2, si2, 1.0f, width, 60.0f);

        Eigen::ArrayXd si3;
        si.compute(v3, si3, 1.0f, width, 60.0f);

        Eigen::ArrayXd si4;
        si.compute(v4, si4, 1.0f, width, 60.0f);

        auto d12 = Euclid::l2(si1, si2);
        auto d13 = Euclid::l2(si1, si3);
        auto d14 = Euclid::l2(si1, si4);
        REQUIRE(d12 < d13);
        REQUIRE(d13 < d14);

        {
            auto vmax = si1.maxCoeff();
            si1 *= 255.0 / vmax;
            std::string fout(TMP_DIR);
            fout.append("bunny_spin_image1.png");
            stbi_write_png(
                fout.c_str(), width, width, 1, &si1(0), width * sizeof(double));
        }

        {
            auto vmax = si2.maxCoeff();
            si2 *= 255.0 / vmax;
            std::string fout(TMP_DIR);
            fout.append("bunny_spin_image2.png");
            stbi_write_png(
                fout.c_str(), width, width, 1, &si2(0), width * sizeof(double));
        }

        {
            auto vmax = si3.maxCoeff();
            si3 *= 255.0 / vmax;
            std::string fout(TMP_DIR);
            fout.append("bunny_spin_image3.png");
            stbi_write_png(
                fout.c_str(), width, width, 1, &si3(0), width * sizeof(double));
        }

        {
            auto vmax = si4.maxCoeff();
            si4 *= 255.0 / vmax;
            std::string fout(TMP_DIR);
            fout.append("bunny_spin_image4.png");
            stbi_write_png(
                fout.c_str(), width, width, 1, &si4(0), width * sizeof(double));
        }
    }

    SECTION("spin image with precomputed normals")
    {
        const unsigned width = 256;

        Eigen::ArrayXd simg;
        Euclid::SpinImage si(mesh, 0.0f, &vnormals);
        si.compute(Vertex(0), simg, 1.0f, width, 60.0f);

        auto vmax = simg.maxCoeff();
        simg *= 255.0 / vmax;
        std::string fout(TMP_DIR);
        fout.append("bunny_spin_image5.png");
        stbi_write_png(
            fout.c_str(), width, width, 1, &simg(0), width * sizeof(double));
    }

    SECTION("heat kernel signature")
    {
        Euclid::HKS<Mesh> hks(mesh, 100);

        Eigen::ArrayXd hks1;
        hks.compute(v1, hks1);

        Eigen::ArrayXd hks2;
        hks.compute(v2, hks2);

        Eigen::ArrayXd hks3;
        hks.compute(v3, hks3);

        Eigen::ArrayXd hks4;
        hks.compute(v4, hks4);

        auto d12 = Euclid::l2(hks1, hks2);
        auto d13 = Euclid::l2(hks1, hks3);
        auto d14 = Euclid::l2(hks1, hks4);
        REQUIRE(d12 < d13);
        REQUIRE(d13 < d14);
    }
}
