#include <Euclid/Analysis/Descriptor.h>
#include <catch.hpp>

#include <algorithm>
#include <vector>
#include <unordered_map>
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
    std::unordered_map<Vertex, Vector_3> vnmap;
    for (size_t i = 0; i < normals.size(); i += 3) {
        vnmap[Vertex(static_cast<uint32_t>(i / 3))] =
            Vector_3(normals[i], normals[i + 1], normals[i + 2]);
    }

    SECTION("spin images")
    {
        const unsigned width = 256;
        std::vector<float> spin_image;
        Euclid::spin_image(mesh,
                           boost::make_assoc_property_map(vnmap),
                           Vertex(0),
                           spin_image,
                           1.0f,
                           width,
                           60.0f);

        auto vmax = *std::max_element(spin_image.begin(), spin_image.end());
        for (auto& v : spin_image) {
            v *= 255 / vmax;
        }
        std::string fout(TMP_DIR);
        fout.append("bunny_spin_image.png");
        stbi_write_png(fout.c_str(),
                       width,
                       width,
                       1,
                       spin_image.data(),
                       width * sizeof(float));
    }
}
