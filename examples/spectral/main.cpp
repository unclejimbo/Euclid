#include <algorithm>
#include <fstream>
#include <utility>
#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Eigen/Core>
#include <Eigen/SparseCore>
#include <MatOp/SparseCholesky.h>
#include <MatOp/SparseSymMatProd.h>
#include <MatOp/SparseSymShiftSolve.h>
#include <SymEigsShiftSolver.h>
#include <SymGEigsSolver.h>
#include <Euclid/Analysis/Descriptor.h>
#include <Euclid/Math/Distance.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Util/Assert.h>
#include <Euclid/Util/Color.h>
#include <Euclid/Util/Timer.h>
#include <igl/is_symmetric.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;
using Mat = Eigen::MatrixXd;
using Vec = Eigen::VectorXd;
using SpMat = Eigen::SparseMatrix<double>;
using Arr = Eigen::ArrayXXd;

std::pair<Vec, Mat> sym_eigs_shift_solve(SpMat cot_mat, SpMat mass_mat, int k)
{
    auto nv = cot_mat.cols();
    SpMat m_mat = mass_mat.unaryExpr(
        [](double v) { return v == 0.0 ? 0.0 : 1.0 / std::sqrt(v); });
    EASSERT(m_mat.coeff(1, 1) == 1.0 / std::sqrt(mass_mat.coeff(1, 1)));
    SpMat laplacian = m_mat * cot_mat * m_mat;

    int convergence = std::min(3 * k + 1, static_cast<int>(nv));
    Spectra::SparseSymShiftSolve<double> op(laplacian);
    Spectra::SymEigsShiftSolver<double,
                                Spectra::LARGEST_MAGN,
                                Spectra::SparseSymShiftSolve<double>>
        eigensolver(&op, k, convergence, 0.0);
    eigensolver.init();
    std::cout << "Solving eigs for symmetric laplacian in shift-invert mode..."
              << std::endl;
    Euclid::Timer t;
    t.tick();
    unsigned n = eigensolver.compute(1000, 1e-10, Spectra::SMALLEST_MAGN);
    auto time = t.tock();
    std::cout << "Time used: " << time << "s" << std::endl;

    EASSERT(n == k); // should converge for small k
    EASSERT(eigensolver.info() == Spectra::SUCCESSFUL);
    EASSERT(eigensolver.eigenvalues()(1) >= 0.0);
    EASSERT(eigensolver.eigenvalues()(2) >= eigensolver.eigenvalues()(1));
    EASSERT(eigensolver.eigenvalues().rows() == k);
    EASSERT(eigensolver.eigenvectors().rows() == nv);
    EASSERT(eigensolver.eigenvectors().cols() == k);
    Vec eigenvalues = eigensolver.eigenvalues();
    Mat eigenfunctions =
        (m_mat * eigensolver.eigenvectors()).householderQr().householderQ();

    std::cout << "smallest eigenvalues:" << std::endl
              << eigenvalues.topLeftCorner(7, 1).transpose() << std::endl
              << "smallest eigenfunctions:" << std::endl
              << eigenfunctions.topLeftCorner(7, 7) << std::endl;

    return std::make_pair(eigenvalues, eigenfunctions);
}

std::pair<Vec, Mat> geigs_solve(SpMat cot_mat, SpMat mass_mat, int k)
{
    auto nv = cot_mat.cols();

    int convergence = std::min(3 * k + 1, static_cast<int>(nv));
    Spectra::SparseSymMatProd<double> op_a(cot_mat);
    Spectra::SparseCholesky<double> op_b(mass_mat);
    Spectra::SymGEigsSolver<double,
                            Spectra::SMALLEST_MAGN,
                            Spectra::SparseSymMatProd<double>,
                            Spectra::SparseCholesky<double>,
                            Spectra::GEIGS_MODE::GEIGS_CHOLESKY>
        eigensolver(&op_a, &op_b, k, convergence);
    eigensolver.init();
    std::cout << "Solving generalized eigs for laplacian..." << std::endl;
    Euclid::Timer t;
    t.tick();
    unsigned n = eigensolver.compute(1000, 1e-10, Spectra::SMALLEST_MAGN);
    auto time = t.tock();
    std::cout << "Time used: " << time << "s" << std::endl;

    EASSERT(n == k); // should converge for small k
    EASSERT(eigensolver.info() == Spectra::SUCCESSFUL);
    EASSERT(eigensolver.eigenvalues()(1) >= 0.0);
    EASSERT(eigensolver.eigenvalues()(2) >= eigensolver.eigenvalues()(1));
    EASSERT(eigensolver.eigenvalues().rows() == k);
    EASSERT(eigensolver.eigenvectors().rows() == nv);
    EASSERT(eigensolver.eigenvectors().cols() == k);
    Vec eigenvalues = eigensolver.eigenvalues();
    Mat eigenfunctions =
        eigensolver.eigenvectors().householderQr().householderQ();

    std::cout << "smallest eigenvalues:" << std::endl
              << eigenvalues.topLeftCorner(7, 1).transpose() << std::endl
              << "smallest eigenfunctions:" << std::endl
              << eigenfunctions.topLeftCorner(7, 7) << std::endl;

    return std::make_pair(eigenvalues, eigenfunctions);
}

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
    auto nv = positions.size() / 3;
    SpMat cot_mat = Euclid::cotangent_matrix(mesh);
    SpMat mass_mat = Euclid::mass_matrix(mesh);

    // solving eigenvalue problem
    int k = 300;
    auto [evalues_shift, efunctions_shift] =
        sym_eigs_shift_solve(cot_mat, mass_mat, k);
    auto [evalues_gen, efunctions_gen] = geigs_solve(cot_mat, mass_mat, k);

    // output eigenfunctions to colors
    eigs_to_color("shift", positions, indices, efunctions_shift);
    eigs_to_color("gen", positions, indices, efunctions_gen);

    // eigen embedding and reconstruction
    eigs_recon("shift", positions, indices, efunctions_shift);
    eigs_recon("gen", positions, indices, efunctions_gen);

    // use eigenstructures to compute heat kernel signature
    Euclid::HKS<Mesh> hks;
    hks.build(mesh, &evalues_shift, &efunctions_shift);
    Eigen::ArrayXXd hks_sigs;
    hks.compute(hks_sigs, 100);
    hks_to_color(positions, indices, hks_sigs, 240); // tip
    hks_to_color(positions, indices, hks_sigs, 3);   // cube corner
    hks_to_csv(hks_sigs);
}
