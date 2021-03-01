#include <stdexcept>
#include <string>
#include <Euclid/Util/Assert.h>

namespace Euclid
{

template<int N, typename DerivedV, typename DerivedF, typename FT, typename IT>
void make_mesh(Eigen::PlainObjectBase<DerivedV>& V,
               Eigen::PlainObjectBase<DerivedF>& F,
               const std::vector<FT>& positions,
               const std::vector<IT>& indices)
{
    static_assert(N >= 3);
    if (positions.empty()) {
        EWARNING("positions is empty.");
        return;
    }
    if (indices.empty()) {
        EWARNING("indices is empty.");
        return;
    }
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Input positions size is not divisible by 3");
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input indices size is not divisible by ");
        err_str.append(std::to_string(N));
        throw std::runtime_error(err_str);
    }

    V.resize(positions.size() / 3, 3);
    F.resize(indices.size() / 3, N);
    for (size_t i = 0; i < positions.size(); i += 3) {
        auto row = i / 3;
        V(row, 0) = positions[i + 0];
        V(row, 1) = positions[i + 1];
        V(row, 2) = positions[i + 2];
    }
    for (size_t i = 0; i < indices.size(); i += N) {
        auto row = i / N;
        for (size_t j = 0; j < N; ++j) {
            F(row, j) = indices[i + j];
        }
    }
}

template<int N, typename DerivedV, typename DerivedF, typename FT, typename IT>
void extract_mesh(const Eigen::MatrixBase<DerivedV>& V,
                  const Eigen::MatrixBase<DerivedF>& F,
                  std::vector<FT>& positions,
                  std::vector<IT>& indices)
{
    positions.clear();
    indices.clear();
    positions.reserve(V.rows() * 3);
    indices.reserve(F.rows() * N);

    for (auto i = 0; i < V.rows(); ++i) {
        positions.push_back(V(i, 0));
        positions.push_back(V(i, 1));
        positions.push_back(V(i, 2));
    }
    for (auto i = 0; i < F.rows(); ++i) {
        for (auto j = 0; j < N; ++j) {
            indices.push_back(F(i, j));
        }
    }
}

template<typename DerivedV, typename FT>
void extract_mesh(const Eigen::MatrixBase<DerivedV>& V,
                  std::vector<FT>& positions)
{
    positions.clear();
    positions.reserve(V.rows() * 3);

    for (auto i = 0; i < V.rows(); ++i) {
        positions.push_back(V(i, 0));
        positions.push_back(V(i, 1));
        positions.push_back(V(i, 2));
    }
}

} // namespace Euclid
