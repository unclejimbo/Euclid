#include <catch2/catch.hpp>
#include <Euclid/Geometry/Spectral.h>

#include <string>
#include <vector>
#include <iostream>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Eigen/Core>
#include <Euclid/Math/Numeric.h>
#include <Euclid/MeshUtil/CGALMesh.h>
#include <Euclid/IO/OffIO.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_3 = typename Kernel::Point_3;
using Vector_3 = typename Kernel::Vector_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

TEST_CASE("Geometry, Spectral", "[geometry][spectral]")
{
    std::string fin(DATA_DIR);
    fin.append("bumpy.off");
    std::vector<double> positions;
    std::vector<int> indices;
    Euclid::read_off<3>(fin, positions, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    int nv = positions.size() / 3;
    unsigned k = 20;
    Eigen::VectorXd lambdas1, lambdas2;
    Eigen::MatrixXd phis1, phis2;

    auto n1 = Euclid::spectrum(
        mesh, k, lambdas1, phis1, Euclid::SpecOp::mesh_laplacian);
    auto n2 = Euclid::spectrum(
        mesh, k, lambdas2, phis2, Euclid::SpecOp::graph_laplacian);

    // test size of outputs
    REQUIRE(n1 == k);
    REQUIRE(n2 == k);
    REQUIRE(lambdas1.size() == k);
    REQUIRE(lambdas2.size() == k);
    REQUIRE(phis1.rows() == nv);
    REQUIRE(phis1.cols() == k);
    REQUIRE(phis2.rows() == nv);
    REQUIRE(phis2.cols() == k);

    // the first eigenvalue shoule be 0
    REQUIRE(Euclid::eq_abs_err(lambdas1(0), 0.0, 1e-14));
    REQUIRE(Euclid::eq_abs_err(lambdas2(0), 0.0, 1e-14));

    // the first eigenvector should be ones, but need to be scaled though,
    // so we only test if the entries are equal
    REQUIRE(Euclid::eq_abs_err(
        phis1.col(0).maxCoeff(), phis1.col(0).minCoeff(), 1e-14));
    REQUIRE(Euclid::eq_abs_err(
        phis2.col(0).maxCoeff(), phis2.col(0).minCoeff(), 1e-14));

    // eigenvectors of the mesh laplacian are orthogonal wrt mass weighted
    // inner product
    auto D = Euclid::mass_matrix(mesh);
    auto dot = (phis1.col(1).transpose() * D * phis1.col(10))(0);
    REQUIRE(Euclid::eq_abs_err(dot, 0.0, 1e-14));

    // eigenvectors of the graph laplacian are orthonormal
    REQUIRE(Euclid::eq_abs_err(phis2.col(1).dot(phis2.col(10)), 0.0, 1e-14));
    REQUIRE(
        Euclid::eq_abs_err(phis2.col(1).norm(), phis2.col(10).norm(), 1e-14));
}
