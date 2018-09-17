#include <fstream>
#include <stdexcept>
#include <tuple>

#include "IOHelpers.h"

namespace Euclid
{

namespace _impl
{

inline std::tuple<size_t, size_t, size_t> read_header(std::ifstream& stream)
{
    std::string word;
    stream >> word;

    size_t n_vertices, n_faces, n_edges;
    stream >> n_vertices >> n_faces >> n_edges;

    std::string dummy;
    std::getline(stream, dummy);

    return std::make_tuple(n_vertices, n_faces, n_edges);
}

inline void write_header(std::ofstream& stream, size_t nv, size_t nf)
{
    stream << "OFF" << std::endl;
    stream << nv << " " << nf << " " << 0 << std::endl;
}

template<int N, typename FT, typename IT, typename CT>
void read_off(const std::string& filename,
              std::vector<FT>& positions,
              std::vector<CT>* vcolors,
              std::vector<IT>* findices,
              std::vector<CT>* fcolors)
{
    std::ifstream stream(filename);
    check_fstream(stream, filename);

    auto [nvertices, nfaces, nedges] = read_header(stream);
    positions.resize(nvertices * 3);
    if (vcolors != nullptr) { vcolors->resize(nvertices * 4); }
    if (findices != nullptr) { findices->resize(nfaces * N); }
    if (fcolors != nullptr) { fcolors->resize(nfaces * 4); }

    for (size_t i = 0; i < nvertices; ++i) {
        std::string line;
        std::getline(stream, line);
        auto words = split(line, ' ');
        if (words.size() != 3 && words.size() != 7) {
            std::string err("Invalid off file: ");
            err.append(filename);
            throw std::runtime_error(err);
        }

        for (size_t j = 0; j < 3; ++j) {
            auto p = std::stod(std::string(words[j]));
            positions[3 * i + j] = static_cast<FT>(p);
        }

        if (vcolors != nullptr && words.size() == 7) {
            for (size_t j = 0; j < 4; ++j) {
                auto c = std::stoi(std::string(words[3 + j]));
                (*vcolors)[4 * i + j] = static_cast<CT>(c);
            }
        }
    }
    if (findices != nullptr && nfaces != 0) {
        for (size_t i = 0; i < nfaces; ++i) {
            std::string line;
            std::getline(stream, line);
            auto words = split(line, ' ');
            if (words.size() != N + 1 && words.size() != N + 5) {
                std::string err("Invalid off file: ");
                err.append(filename);
                throw std::runtime_error(err);
            }
            auto n = std::stoi(std::string(words[0]));
            if (n != N) {
                std::string err("Invalid off file: ");
                err.append(filename);
                throw std::runtime_error(err);
            }

            for (size_t j = 0; j < N; ++j) {
                auto idx = std::stoul(std::string(words[j + 1]));
                (*findices)[N * i + j] = static_cast<IT>(idx);
            }

            if (fcolors != nullptr && words.size() == N + 5) {
                for (size_t j = 0; j < 4; ++j) {
                    auto c = std::stoi(std::string(words[j + 1 + N]));
                    (*fcolors)[4 * i + j] = static_cast<CT>(c);
                }
            }
        }
    }

    positions.shrink_to_fit();
    if (vcolors != nullptr) { vcolors->shrink_to_fit(); }
    if (findices != nullptr) { findices->shrink_to_fit(); }
    if (fcolors != nullptr) { fcolors->shrink_to_fit(); }
}

template<int N, typename FT, typename IT, typename CT>
void write_off(const std::string& filename,
               const std::vector<FT>& positions,
               const std::vector<CT>* vcolors,
               const std::vector<IT>* findices,
               const std::vector<CT>* fcolors)
{
    std::ofstream stream(filename);
    check_fstream(stream, filename);

    auto nvertices = positions.size() / 3;
    auto nfaces = 0;
    if (findices != nullptr && N != 0) { nfaces = findices->size() / N; }
    write_header(stream, nvertices, nfaces);

    for (size_t i = 0; i < nvertices; ++i) {
        for (size_t j = 0; j < 3; ++j) {
            stream << positions[3 * i + j] << " ";
        }

        if (vcolors != nullptr) {
            for (size_t j = 0; j < 4; ++j) {
                stream << static_cast<int>((*vcolors)[4 * i + j]) << " ";
            }
        }

        stream << std::endl;
    }
    if (findices != nullptr && nfaces != 0) {
        for (size_t i = 0; i < nfaces; ++i) {
            stream << N << " ";

            for (size_t j = 0; j < N; ++j) {
                stream << (*findices)[N * i + j] << " ";
            }

            if (fcolors != nullptr) {
                for (size_t j = 0; j < 4; ++j) {
                    stream << static_cast<int>((*fcolors)[4 * i + j]) << " ";
                }
            }

            stream << std::endl;
        }
    }
}

} // namespace _impl

template<typename FT>
void read_off(const std::string& filename,
              std::vector<FT>& positions,
              std::nullptr_t vcolors,
              std::nullptr_t findices,
              std::nullptr_t fcolors)
{
    using IT = int;
    using CT = int;
    _impl::read_off<0, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

template<typename FT, typename CT>
void read_off(const std::string& filename,
              std::vector<FT>& positions,
              std::vector<CT>* vcolors,
              std::nullptr_t findices,
              std::nullptr_t fcolors)
{
    using IT = int;
    _impl::read_off<0, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

template<int N, typename FT, typename IT>
void read_off(const std::string& filename,
              std::vector<FT>& positions,
              std::nullptr_t vcolors,
              std::vector<IT>* findices,
              std::nullptr_t fcolors)
{
    using CT = float;
    _impl::read_off<N, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

template<int N, typename FT, typename IT, typename CT>
void read_off(const std::string& filename,
              std::vector<FT>& positions,
              std::vector<CT>* vcolors,
              std::vector<IT>* findices,
              std::nullptr_t fcolors)
{
    _impl::read_off<N, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

template<int N, typename FT, typename IT, typename CT>
void read_off(const std::string& filename,
              std::vector<FT>& positions,
              std::nullptr_t vcolors,
              std::vector<IT>* findices,
              std::vector<CT>* fcolors)
{
    _impl::read_off<N, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

template<typename FT>
void write_off(const std::string& filename,
               const std::vector<FT>& positions,
               std::nullptr_t vcolors,
               std::nullptr_t findices,
               std::nullptr_t fcolors)
{
    using IT = int;
    using CT = int;
    _impl::write_off<0, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

template<typename FT, typename CT>
void write_off(const std::string& filename,
               const std::vector<FT>& positions,
               const std::vector<CT>* vcolors,
               std::nullptr_t findices,
               std::nullptr_t fcolors)
{
    using IT = int;
    _impl::write_off<0, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

template<int N, typename FT, typename IT>
void write_off(const std::string& filename,
               const std::vector<FT>& positions,
               std::nullptr_t vcolors,
               const std::vector<IT>* findices,
               std::nullptr_t fcolors)
{
    using CT = float;
    _impl::write_off<N, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

template<int N, typename FT, typename IT, typename CT>
void write_off(const std::string& filename,
               const std::vector<FT>& positions,
               const std::vector<CT>* vcolors,
               const std::vector<IT>* findices,
               std::nullptr_t fcolors)
{
    _impl::write_off<N, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

template<int N, typename FT, typename IT, typename CT>
void write_off(const std::string& filename,
               const std::vector<FT>& positions,
               std::nullptr_t vcolors,
               const std::vector<IT>* findices,
               const std::vector<CT>* fcolors)
{
    _impl::write_off<N, FT, IT, CT>(
        filename, positions, vcolors, findices, fcolors);
}

} // namespace Euclid
