#include <catch2/catch.hpp>
#include <Euclid/Segmentation/RandomWalk.h>

#include <string>
#include <unordered_map>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/MeshUtil/MeshHelpers.h>
#include <Euclid/Util/Color.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

TEST_CASE("Segmentation, RandomWalk", "[segmentation][randomwalk]")
{
    std::string filename(DATA_DIR);
    filename.append("bunny_vn.ply");
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<unsigned> indices;
    Euclid::read_ply<3>(
        filename, positions, &normals, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    std::vector<unsigned> seeds{ 11586, 5052,  248,  3995,  4709,  10824,
                                 7158,  14057, 1630, 13981, 14179, 4532 };
    std::vector<unsigned> segments;
    Euclid::random_walk_segmentation(mesh, seeds, segments);

    std::unordered_map<unsigned, unsigned> sid;
    for (size_t i = 0; i < seeds.size(); ++i) {
        sid.emplace(seeds[i], i);
    }
    std::vector<unsigned char> color_set;
    Euclid::rnd_colors(seeds.size(), color_set, true);
    std::vector<unsigned char> colors(segments.size() * 4);
    for (size_t i = 0; i < segments.size(); ++i) {
        auto id = sid[segments[i]];
        colors[4 * i + 0] = color_set[3 * id + 0];
        colors[4 * i + 1] = color_set[3 * id + 1];
        colors[4 * i + 2] = color_set[3 * id + 2];
        colors[4 * i + 3] = 255;
    }
    std::string fout(TMP_DIR);
    fout.append("random_walk_segment_mesh.off");
    Euclid::write_off<3>(fout, positions, nullptr, &indices, &colors);
}
