#include <fstream>
#include <stdexcept>
#include <string>
#include <cereal/archives/binary.hpp>
#include <cereal/details/util.hpp>
#include <Euclid/Util/Assert.h>

namespace cereal
{

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

template<typename Derived>
void load(BinaryInputArchive& ar, Eigen::PlainObjectBase<Derived>& mat)
{
    using T = typename Eigen::PlainObjectBase<Derived>::Scalar;
    typename Eigen::PlainObjectBase<Derived>::Index rows, cols;
    ar(rows, cols);
    mat.derived().resize(rows, cols);
    ar(binary_data(mat.data(), rows * cols * sizeof(T)));
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

template<typename Derived>
void serialize(const std::string& filename,
               const Eigen::PlainObjectBase<Derived>& mat)
{
    auto mode =
        std::ios_base::out | std::ios_base::binary | std::ios_base::trunc;
    std::ofstream ofs(filename, mode);
    _impl::check_fstream(ofs, filename);
    cereal::BinaryOutputArchive archive(ofs);
    archive(mat);
}

template<typename Derived>
void deserialize(const std::string& filename,
                 Eigen::PlainObjectBase<Derived>& mat)
{
    auto mode = std::ios_base::in | std::ios_base::binary;
    std::ifstream ifs(filename, mode);
    _impl::check_fstream(ifs, filename);
    cereal::BinaryInputArchive archive(ifs);
    archive(mat);
}

} // namespace Euclid
