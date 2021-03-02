#include <catch2/catch.hpp>
#include <Euclid/MeshUtil/TTKMesh.h>

#include <config.h>
#include <string>
#include <vector>
#include <Euclid/IO/OffIO.h>
#include <ttk/base/ExplicitTriangulation.h>
#include <ttk/base/ScalarFieldSmoother.h>

TEST_CASE("MeshUtil, TTKMesh", "[meshutil][ttkmesh]")
{
    std::vector<double> positions;
    std::vector<unsigned> indices;
    std::string fin(DATA_DIR);
    fin.append("bunny.off");
    Euclid::read_off<3>(fin, positions, nullptr, &indices, nullptr);

    std::vector<long long> cells;
    Euclid::make_cells(cells, indices, 3);

    ttk::ExplicitTriangulation mesh;
    Euclid::make_mesh(mesh, positions, cells);

    ttk::ScalarFieldSmoother smoother;
    smoother.preconditionTriangulation(&mesh);
    smoother.setDimensionNumber(3);
    smoother.setInputDataPointer(positions.data());
    smoother.setOutputDataPointer(positions.data());
    smoother.smooth<double>(&mesh, 3);

    std::vector<double> smooth_positions;
    std::vector<long long> smooth_cells;
    Euclid::extract_mesh(mesh, smooth_positions, smooth_cells);

    std::vector<unsigned> smooth_indices;
    Euclid::extract_cells(smooth_indices, smooth_cells);

    REQUIRE(smooth_positions.size() == positions.size());
    REQUIRE(smooth_cells.size() == cells.size());
    REQUIRE(smooth_indices.size() == indices.size());

    std::string fout(TMP_DIR);
    fout.append("bunny_ttk_smooth.off");
    Euclid::write_off<3>(
        fout, smooth_positions, nullptr, &smooth_indices, nullptr);
}
