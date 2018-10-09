#include <catch2/catch.hpp>
#include <Euclid/Geometry/Spectral.h>

#include <string>
#include <vector>
#include <iostream>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Eigen/Core>
#include <Euclid/Geometry/MeshHelpers.h>
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
    unsigned k = 30;
    double norm = Eigen::VectorXd::Ones(nv).normalized().norm();

    SECTION("Laplace-Beltrami")
    {
        Eigen::VectorXd lambdas1, lambdas2;
        Eigen::MatrixXd phis1, phis2;

        // solve the symmetric matrix
        auto n1 = Euclid::spectrum(mesh,
                                   k,
                                   lambdas1,
                                   phis1,
                                   Euclid::SpecOp::laplace_beltrami,
                                   Euclid::SpecDecomp::symmetric);

        // solve the generalized eigen problem
        auto n2 = Euclid::spectrum(mesh,
                                   k,
                                   lambdas2,
                                   phis2,
                                   Euclid::SpecOp::laplace_beltrami,
                                   Euclid::SpecDecomp::generalized);

        REQUIRE(n1 == k);
        REQUIRE(n2 == k);
        REQUIRE(lambdas1.size() == k);
        REQUIRE(lambdas1(k - 1) == Approx(lambdas2(k - 1)).epsilon(0.05));
        REQUIRE(phis1.rows() == nv);
        REQUIRE(phis1.cols() == k);
        REQUIRE(phis1.col(0).normalized().norm() == Approx(norm));
        REQUIRE(phis2.col(0).normalized().norm() == Approx(norm));
    }

    SECTION("graph Laplacian")
    {
        Eigen::VectorXd lambdas1, lambdas2;
        Eigen::MatrixXd phis1, phis2;

        // solve the symmetric matrix
        auto n1 = Euclid::spectrum(mesh,
                                   k,
                                   lambdas1,
                                   phis1,
                                   Euclid::SpecOp::graph_laplacian,
                                   Euclid::SpecDecomp::symmetric);

        // solve the generalized eigen problem
        auto n2 = Euclid::spectrum(mesh,
                                   k,
                                   lambdas2,
                                   phis2,
                                   Euclid::SpecOp::graph_laplacian,
                                   Euclid::SpecDecomp::generalized);

        REQUIRE(n1 == k);
        REQUIRE(n2 == k);
        REQUIRE(lambdas1.size() == k);
        REQUIRE(lambdas1(k - 1) == Approx(lambdas2(k - 1)).epsilon(0.05));
        REQUIRE(phis1.rows() == nv);
        REQUIRE(phis1.cols() == k);
        REQUIRE(phis1.col(0).normalized().norm() == Approx(norm));
        REQUIRE(phis2.col(0).normalized().norm() == Approx(norm));
    }
}
