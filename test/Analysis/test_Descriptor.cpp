#include <Euclid/Analysis/Descriptor.h>
#include <catch.hpp>

#include <algorithm>
#include <cmath>
#include <vector>
#include <boost/property_map/property_map.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <stb_image_write.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Vector_3 = Kernel::Vector_3;
using Mesh = CGAL::Surface_mesh<Kernel::Point_3>;
using Vertex = Mesh::Vertex_index;

inline float l2_distance(const std::vector<float>& d1,
                         const std::vector<float>& d2)
{
    float dist = 0.0f;
    for (size_t i = 0; i < d1.size(); ++i) {
        auto d = d1[i] - d2[i];
        dist += d * d;
    }
    return std::sqrt(dist);
}

TEST_CASE("Analysis, Descriptor", "[analysis][descriptor]")
{
    std::vector<float> positions;
    std::vector<float> normals;
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

    SECTION("spin images")
    {
        const unsigned width = 256;

        Euclid::SpinImage si(mesh);

        // vertex on ear
        std::vector<float> si1;
        si.compute(Vertex(7505), si1, 1.0f, width, 60.0f);

        // vertex next by
        std::vector<float> si2;
        si.compute(Vertex(695), si2, 1.0f, width, 60.0f);

        // vertex on the other ear
        std::vector<float> si3;
        si.compute(Vertex(3915), si3, 1.0f, width, 60.0f);

        // vertex far awary
        std::vector<float> si4;
        si.compute(Vertex(0), si4, 1.0f, width, 60.0f);

        auto d12 = l2_distance(si1, si2);
        auto d13 = l2_distance(si1, si3);
        auto d14 = l2_distance(si1, si4);
        REQUIRE(d12 < d13);
        REQUIRE(d13 < d14);

        {
            auto vmax = *std::max_element(si1.begin(), si1.end());
            for (auto& v : si1) {
                v *= 255 / vmax;
            }
            std::string fout(TMP_DIR);
            fout.append("bunny_spin_image1.png");
            stbi_write_png(fout.c_str(),
                           width,
                           width,
                           1,
                           si1.data(),
                           width * sizeof(float));
        }

        {
            auto vmax = *std::max_element(si2.begin(), si2.end());
            for (auto& v : si2) {
                v *= 255 / vmax;
            }
            std::string fout(TMP_DIR);
            fout.append("bunny_spin_image2.png");
            stbi_write_png(fout.c_str(),
                           width,
                           width,
                           1,
                           si2.data(),
                           width * sizeof(float));
        }

        {
            auto vmax = *std::max_element(si3.begin(), si3.end());
            for (auto& v : si3) {
                v *= 255 / vmax;
            }
            std::string fout(TMP_DIR);
            fout.append("bunny_spin_image3.png");
            stbi_write_png(fout.c_str(),
                           width,
                           width,
                           1,
                           si3.data(),
                           width * sizeof(float));
        }

        {
            auto vmax = *std::max_element(si4.begin(), si4.end());
            for (auto& v : si4) {
                v *= 255 / vmax;
            }
            std::string fout(TMP_DIR);
            fout.append("bunny_spin_image4.png");
            stbi_write_png(fout.c_str(),
                           width,
                           width,
                           1,
                           si4.data(),
                           width * sizeof(float));
        }
    }

    SECTION("spin image with precomputed normals")
    {
        const unsigned width = 256;

        std::vector<float> spin_image;
        Euclid::SpinImage si(mesh, 0.0f, &vnormals);
        si.compute(Vertex(0), spin_image, 1.0f, width, 60.0f);

        auto vmax = *std::max_element(spin_image.begin(), spin_image.end());
        for (auto& v : spin_image) {
            v *= 255 / vmax;
        }
        std::string fout(TMP_DIR);
        fout.append("bunny_spin_image5.png");
        stbi_write_png(fout.c_str(),
                       width,
                       width,
                       1,
                       spin_image.data(),
                       width * sizeof(float));
    }
}
