#include <catch2/catch.hpp>
#include <Euclid/FeatureDetection/NativeHKS.h>

#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/Descriptor/HKS.h>
#include <Euclid/MeshUtil/CGALMesh.h>
#include <Euclid/Math/Numeric.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Util/Color.h>
#include <Euclid/Util/Serialize.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;
using Vertex = Mesh::Vertex_index;

static void _write_features_to_colored_mesh(
    const std::string& f,
    const std::vector<double>& positions,
    const std::vector<double>& normals,
    const std::vector<unsigned>& indices,
    const std::vector<Vertex>& features)
{
    std::vector<unsigned char> colors(positions.size(), 0);
    for (auto v : features) {
        colors[v.idx() * 3] = 255;
    }
    std::string fout(TMP_DIR);
    fout.append(f);
    Euclid::write_ply<3>(fout, positions, &normals, nullptr, &indices, &colors);
}

TEST_CASE("Feature detection, Native HKS", "[feature][nativehks]")
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

    SECTION("smaller time range")
    {
        Eigen::ArrayXXd hks_all;
        hks.compute(hks_all, 100, 0.1, 4.0);
        auto features =
            Euclid::native_hks_features(mesh, hks_all, hks_all.rows() - 1);
        _write_features_to_colored_mesh("native_hks_features_smaller.ply",
                                        positions,
                                        normals,
                                        indices,
                                        features);
    }

    SECTION("larger time range")
    {
        Eigen::ArrayXXd hks_all;
        hks.compute(hks_all, 100, 0.1, 8.0);
        auto features =
            Euclid::native_hks_features(mesh, hks_all, hks_all.rows() - 1);
        _write_features_to_colored_mesh("native_hks_features_larger.ply",
                                        positions,
                                        normals,
                                        indices,
                                        features);
    }
}
