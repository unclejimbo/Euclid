#include <algorithm>
#include <fstream>
#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Eigen/Core>
#include <Eigen/Householder>
#include <Eigen/SparseCore>
#include <Euclid/Descriptor/HKS.h>
#include <Euclid/Descriptor/Histogram.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/MeshUtil/CGALMesh.h>
#include <Euclid/Geometry/Spectral.h>
#include <Euclid/Util/Color.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;
using Mat = Eigen::MatrixXd;
using Vec = Eigen::VectorXd;
using SpMat = Eigen::SparseMatrix<double>;
using Arr = Eigen::ArrayXXd;

void eigs_to_color(const std::string& prefix,
                   const std::vector<double>& positions,
                   const std::vector<unsigned>& indices,
                   const Mat& eigenfunctions)
{
    auto nv = positions.size() / 3;
    for (int i = 0; i < 6; ++i) {
        std::vector<double> eigenvector(nv);
        Vec::Map(eigenvector.data(), nv) = eigenfunctions.col(i);
        std::vector<uint8_t> colors;
        Euclid::colormap(igl::COLOR_MAP_TYPE_JET, eigenvector, colors, true);
        std::string fout(TMP_DIR);
        fout.append(prefix)
            .append("_eigenfunction_")
            .append(std::to_string(i))
            .append(".ply");
        Euclid::write_ply<3>(
            fout, positions, nullptr, nullptr, &indices, &colors);
    }
}

void eigs_recon(const std::string& prefix,
                const std::vector<double>& positions,
                const std::vector<unsigned>& indices,
                const Mat& eigenfunctions)
{
    auto nv = positions.size() / 3;
    Eigen::Map<const Mat> xyz(positions.data(), 3, nv);
    EASSERT(xyz(1, 0) == positions[1]);
    EASSERT(xyz(0, 1) == positions[3]);
    Mat embedding = xyz * eigenfunctions;
    std::vector<int> neigs{ 5, 20, 50, 100, 200, 300 };
    for (auto ne : neigs) {
        Mat xyz_recon =
            embedding.leftCols(ne) * eigenfunctions.transpose().topRows(ne);
        std::vector<double> recon(nv * 3);
        Mat::Map(recon.data(), 3, nv) = xyz_recon;
        std::string fout(TMP_DIR);
        fout.append(prefix)
            .append("_recon_")
            .append(std::to_string(ne))
            .append(".ply");
        Euclid::write_ply<3>(fout, recon, nullptr, nullptr, &indices, nullptr);
    }
}

void hks_to_color(const std::vector<double>& positions,
                  const std::vector<unsigned>& indices,
                  const Arr& signatures,
                  int vidx)
{
    std::vector<double> distances(signatures.cols());
    for (int i = 0; i < signatures.cols(); ++i) {
        distances[i] = Euclid::chi2(signatures.col(vidx), signatures.col(i));
    }
    std::vector<uint8_t> colors;
    Euclid::colormap(
        igl::COLOR_MAP_TYPE_JET, distances, colors, true, false, true);
    std::string fout(TMP_DIR);
    fout.append("hks_").append(std::to_string(vidx)).append(".ply");
    Euclid::write_ply<3>(fout, positions, nullptr, nullptr, &indices, &colors);
}

void hks_to_csv(const Arr& signatures)
{
    std::string fout(TMP_DIR);
    fout.append("hks.csv");
    std::ofstream ofs(fout);
    for (int i = 0; i < signatures.rows(); ++i) {
        for (int j = 0; j < signatures.cols(); ++j) {
            ofs << signatures(i, j) << ",";
        }
        ofs << std::endl;
    }
}

int main()
{
    // read and build mesh
    std::vector<double> positions;
    std::vector<unsigned> indices;
    std::string fmesh(DATA_DIR);
    fmesh.append("bumpy.off");
    Euclid::read_off<3>(fmesh, positions, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    // solving eigenvalue problem
    int k = 300;
    Vec lambdas_lbo, lambdas_gl;
    Mat phis_lbo, phis_gl;
    Euclid::spectrum(
        mesh, k, lambdas_lbo, phis_lbo, Euclid::SpecOp::mesh_laplacian);
    Euclid::spectrum(
        mesh, k, lambdas_gl, phis_gl, Euclid::SpecOp::graph_laplacian);

    // eigenvectors of mesh laplacian need to be orthonormalized for recon
    phis_lbo = phis_lbo.householderQr().householderQ();

    // output eigenfunctions to colors
    eigs_to_color("lbo", positions, indices, phis_lbo);
    eigs_to_color("gl", positions, indices, phis_gl);

    // eigen embedding and reconstruction
    eigs_recon("lbo", positions, indices, phis_lbo);
    eigs_recon("gl", positions, indices, phis_gl);

    // use eigenstructures to compute heat kernel signature
    Euclid::HKS<Mesh> hks;
    hks.build(mesh, &lambdas_lbo, &phis_lbo);
    Eigen::ArrayXXd hks_sigs;
    hks.compute(hks_sigs, 100);
    hks_to_color(positions, indices, hks_sigs, 240); // tip
    hks_to_color(positions, indices, hks_sigs, 3);   // cube corner
    hks_to_csv(hks_sigs);
}
