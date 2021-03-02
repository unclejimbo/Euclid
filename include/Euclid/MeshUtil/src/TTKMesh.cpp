#include <Euclid/Util/Assert.h>

namespace Euclid
{

template<typename IT>
void make_cells(std::vector<long long>& cells,
                const std::vector<IT>& indices,
                int n)
{
    cells.clear();
    cells.reserve(indices.size() / n * (n + 1));
    size_t i = 0;
    while (i < indices.size()) {
        cells.push_back(n);
        for (int j = 0; j < n; ++j) {
            cells.push_back(indices[i + j]);
        }
        i += n;
    }
}

template<typename IT>
void extract_cells(std::vector<IT>& indices,
                   const std::vector<long long>& cells)
{
    indices.clear();
    size_t i = 0;
    while (i < cells.size()) {
        int n = cells[i++];
        for (int j = 0; j < n; ++j) {
            indices.push_back(cells[i + j]);
        }
        i += n;
    }
}

template<typename Triangulation>
void make_mesh(Triangulation& mesh,
               const std::vector<float>& positions,
               const std::vector<long long>& cells)
{
    mesh.setInputPoints(positions.size() / 3, positions.data(), false);
    mesh.setInputCells(cells.size() / 4, cells.data());
}

template<typename Triangulation>
void make_mesh(Triangulation& mesh,
               const std::vector<double>& positions,
               const std::vector<long long>& cells)
{
    mesh.setInputPoints(positions.size() / 3, positions.data(), true);
    mesh.setInputCells(cells.size() / 4, cells.data());
}

template<typename Triangulation, typename FT>
void extract_mesh(const Triangulation& mesh,
                  std::vector<FT>& positions,
                  std::vector<long long>& cells)
{
    positions.clear();
    positions.resize(3 * mesh.getNumberOfVertices());
    for (int i = 0; i < mesh.getNumberOfVertices(); ++i) {
        float x, y, z;
        mesh.getVertexPoint(i, x, y, z);
        positions[3 * i + 0] = static_cast<FT>(x);
        positions[3 * i + 1] = static_cast<FT>(y);
        positions[3 * i + 2] = static_cast<FT>(z);
    }

    cells.clear();
    cells.reserve(4 * mesh.getNumberOfCells());
    for (int i = 0; i < mesh.getNumberOfCells(); ++i) {
        EASSERT(mesh.getCellVertexNumber(i) == 3);
        cells.push_back(mesh.getCellVertexNumber(i));
        for (int j = 0; j < mesh.getCellVertexNumber(i); ++j) {
            int v;
            mesh.getCellVertex(i, j, v);
            cells.push_back(v);
        }
    }
}

} // namespace Euclid
