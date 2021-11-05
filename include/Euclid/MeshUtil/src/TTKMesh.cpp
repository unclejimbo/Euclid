#include <Euclid/Util/Assert.h>

namespace Euclid
{

namespace _impl
{

#ifdef TTK_CELL_ARRAY_NEW
template<typename Triangulation, typename IT>
void set_input_cells(Triangulation& mesh,
                     std::vector<ttk::LongSimplexId>& connectivity,
                     std::vector<ttk::LongSimplexId>& offset,
                     const std::vector<IT>& indices,
                     int n)
{
    connectivity.resize(indices.size());
    std::copy(indices.begin(), indices.end(), connectivity.begin());
    offset.resize(indices.size() / n + 1);
    for (size_t i = 0; i < offset.size(); ++i) {
        offset[i] = i * n;
    }
    mesh.setInputCells(offset.size() - 1, connectivity.data(), offset.data());
}
#else
template<typename Triangulation, typename IT>
void set_input_cells(Triangulation& mesh,
                     std::vector<ttk::LongSimplexId>& cells,
                     const std::vector<IT>& indices,
                     int n)
{
    cells.reserve(indices.size() / n * (n + 1));
    size_t i = 0;
    while (i < indices.size()) {
        cells.push_back(n);
        for (int j = 0; j < n; ++j) {
            cells.push_back(indices[i + j]);
        }
        i += n;
    }
    mesh.setInputCells(cells.size() / 4, cells.data());
}
#endif

} // namespace _impl

#ifdef TTK_CELL_ARRAY_NEW

template<typename Triangulation, typename IT>
void make_mesh(Triangulation& mesh,
               std::vector<ttk::LongSimplexId>& connectivity,
               std::vector<ttk::LongSimplexId>& offset,
               const std::vector<float>& positions,
               const std::vector<IT>& indices)
{
    mesh.setInputPoints(positions.size() / 3, positions.data(), false);
    _impl::set_input_cells(mesh, connectivity, offset, indices, 3);
}

template<typename Triangulation, typename IT>
void make_mesh(Triangulation& mesh,
               std::vector<ttk::LongSimplexId>& connectivity,
               std::vector<ttk::LongSimplexId>& offset,
               const std::vector<double>& positions,
               const std::vector<IT>& indices)
{
    mesh.setInputPoints(positions.size() / 3, positions.data(), true);
    _impl::set_input_cells(mesh, connectivity, offset, indices, 3);
}

#else

template<typename Triangulation, typename IT>
void make_mesh(Triangulation& mesh,
               std::vector<ttk::LongSimplexId>& cells,
               const std::vector<float>& positions,
               const std::vector<IT>& indices)
{
    mesh.setInputPoints(positions.size() / 3, positions.data(), false);
    _impl::set_input_cells(mesh cells, indices, 3);
}

template<typename Triangulation, typename IT>
void make_mesh(Triangulation& mesh,
               std::vector<ttk::LongSimplexId>& cells,
               const std::vector<double>& positions,
               const std::vector<IT>& indices)
{
    mesh.setInputPoints(positions.size() / 3, positions.data(), true);
    _impl::set_input_cells(mesh, cells, indices, 3);
}

#endif

template<typename Triangulation, typename FT, typename IT>
void extract_mesh(const Triangulation& mesh,
                  std::vector<FT>& positions,
                  std::vector<IT>& indices)
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

    indices.clear();
    indices.reserve(3 * mesh.getNumberOfCells());
    for (int i = 0; i < mesh.getNumberOfCells(); ++i) {
        // EASSERT(mesh.getCellVertexNumber(i) == 3);
        for (int j = 0; j < mesh.getCellVertexNumber(i); ++j) {
            int v;
            mesh.getCellVertex(i, j, v);
            indices.push_back(v);
        }
    }
}

} // namespace Euclid
