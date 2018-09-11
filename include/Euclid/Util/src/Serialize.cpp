#include <fstream>
#include <stdexcept>
#include <string>
#include <cereal/archives/binary.hpp>
#include <cereal/details/util.hpp>
#include <Euclid/Util/Assert.h>

namespace cereal
{

/** Serialize an Eigen dense object.
 *
 *  Coeffwise access, slow.
 */
template<typename Archive, typename Derived>
void save(Archive& ar, const Eigen::PlainObjectBase<Derived>& mat)
{
    auto d = mat.eval();
    const auto rows = d.rows();
    const auto cols = d.cols();
    ar(rows, cols);
    for (auto i = 0; i < rows; ++i) {
        for (auto j = 0; j < cols; ++j) {
            ar(d(i, j));
        }
    }
}

/** Serialize an Eigen dense object.
 *
 *  Buffer access, fast, only work in binary mode.
 */
template<typename Derived>
void save(BinaryOutputArchive& ar, const Eigen::PlainObjectBase<Derived>& mat)
{
    using T = typename Eigen::PlainObjectBase<Derived>::Scalar;
    auto d = mat.eval();
    const auto rows = d.rows();
    const auto cols = d.cols();
    ar(rows, cols);
    ar(binary_data(d.data(), rows * cols * sizeof(T)));
}

/** Deserialize an Eigen dense object.
 *
 *  Coeffwise access, slow.
 */
template<typename Archive, typename Derived>
void load(Archive& ar, Eigen::PlainObjectBase<Derived>& mat)
{
    typename Eigen::PlainObjectBase<Derived>::Index rows, cols;
    ar(rows, cols);
    mat.derived().resize(rows, cols);
    for (auto i = 0; i < rows; ++i) {
        for (auto j = 0; j < cols; ++j) {
            ar(mat(i, j));
        }
    }
}

/** Deserialize an Eigen dense object.
 *
 *  Buffer access, fast, only work in binary mode.
 */
template<typename Derived>
void load(BinaryInputArchive& ar, Eigen::PlainObjectBase<Derived>& mat)
{
    using T = typename Eigen::PlainObjectBase<Derived>::Scalar;
    typename Eigen::PlainObjectBase<Derived>::Index rows, cols;
    ar(rows, cols);
    mat.derived().resize(rows, cols);
    ar(binary_data(mat.data(), rows * cols * sizeof(T)));
}

/** Serialize an Eigen sparse matrix.
 *
 */
template<typename Archive, typename Scalar, int Options, typename StorageIndex>
void save(Archive& ar,
          const Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat)
{
    using SpMat = Eigen::SparseMatrix<Scalar, Options, StorageIndex>;
    auto rows = static_cast<StorageIndex>(mat.rows());
    auto cols = static_cast<StorageIndex>(mat.cols());
    auto size = static_cast<StorageIndex>(mat.nonZeros());
    ar(rows, cols, size);
    for (StorageIndex i = 0; i < mat.outerSize(); ++i) {
        for (SpMat::InnerIterator it(mat, i); it; ++it) {
            auto row = static_cast<StorageIndex>(it.row());
            auto col = static_cast<StorageIndex>(it.col());
            auto value = it.value();
            ar(row, col, value);
        }
    }
}

/** Deserialize an Eigen sparse matrix.
 *
 */
template<typename Archive, typename Scalar, int Options, typename StorageIndex>
void load(Archive& ar, Eigen::SparseMatrix<Scalar, Options, StorageIndex>& mat)
{
    using SpMat = Eigen::SparseMatrix<Scalar, Options, StorageIndex>;
    using Triplet = Eigen::Triplet<Scalar, StorageIndex>;
    StorageIndex rows, cols, size;
    ar(rows, cols, size);
    mat.resize(rows, cols);
    std::vector<Triplet> triplets(size);
    for (StorageIndex i = 0; i < size; ++i) {
        StorageIndex row, col;
        Scalar value;
        ar(row, col, value);
        triplets[i] = Triplet(row, col, value);
    }
    mat.setFromTriplets(triplets.begin(), triplets.end());
}

} // namespace cereal

namespace Euclid
{

namespace _impl
{

template<typename Stream>
void check_fstream(Stream& stream, const std::string& filename)
{
    if (!stream.is_open()) {
        std::string err("Can't open file ");
        err.append(filename);
        throw std::runtime_error(err);
    }
}

} // namespace _impl

template<typename... T>
void serialize(const std::string& filename, const T&... data)
{
    auto mode =
        std::ios_base::out | std::ios_base::binary | std::ios_base::trunc;
    std::ofstream ofs(filename, mode);
    _impl::check_fstream(ofs, filename);
    cereal::BinaryOutputArchive archive(ofs);
    archive(data...);
}

template<typename... T>
void deserialize(const std::string& filename, T&... data)
{
    auto mode = std::ios_base::in | std::ios_base::binary;
    std::ifstream ifs(filename, mode);
    _impl::check_fstream(ifs, filename);
    cereal::BinaryInputArchive archive(ifs);
    archive(data...);
}

} // namespace Euclid
