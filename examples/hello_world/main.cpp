#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Util/Color.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

int main()
{
    // Read triangle mesh into buffers
    std::vector<float> positions;
    std::vector<unsigned> indices;
    std::string fin(DATA_DIR);
    fin.append("bumpy.off");
    Euclid::read_off<3>(fin, positions, nullptr, &indices, nullptr);

    // Generate a CGAL::Surface_mesh
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    // Compute gaussian curvatures
    std::vector<float> curvatures;
    for (auto v : vertices(mesh)) {
        curvatures.push_back(Euclid::gaussian_curvature(v, mesh));
    }

    // Turn curvatures into colors and output to a file
    std::vector<unsigned char> colors;
    Euclid::colormap(igl::COLOR_MAP_TYPE_JET, curvatures, colors, true);
    std::string fout(TMP_DIR);
    fout.append("hello_world/bumpy.ply");
    Euclid::write_ply<3>(fout, positions, nullptr, nullptr, &indices, &colors);
}
