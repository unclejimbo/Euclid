#include <algorithm>
#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <igl/colormap.h>

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
    Euclid::read_off<3>(fin, positions, indices);

    // Generate a CGAL::Surface_mesh
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    // Compute gaussian cuvatrues
    std::vector<float> curvatures;
    for (auto v : vertices(mesh)) {
        curvatures.push_back(Euclid::gaussian_curvature(v, mesh));
    }

    // Turn cuvatures into colors and output to a file
    auto [cmin, cmax] =
        std::minmax_element(curvatures.begin(), curvatures.end());
    std::vector<unsigned char> colors;
    for (auto c : curvatures) {
        auto curvature = (c - *cmin) / (*cmax - *cmin);
        float r, g, b;
        igl::colormap(igl::COLOR_MAP_TYPE_JET, curvature, r, g, b);
        colors.push_back(static_cast<unsigned char>(r * 255));
        colors.push_back(static_cast<unsigned char>(g * 255));
        colors.push_back(static_cast<unsigned char>(b * 255));
    }
    std::string fout(TMP_DIR);
    fout.append("hello_world/bumpy.ply");
    Euclid::write_ply<3>(fout, positions, nullptr, nullptr, &indices, &colors);
}
