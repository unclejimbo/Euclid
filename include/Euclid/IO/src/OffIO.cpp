#include <fstream>
#include <tuple>
#include <exception>

namespace Euclid
{

namespace _impl
{

template<typename Stream>
inline void check_stream(Stream& stream, const std::string& file_name)
{
    if (!stream.is_open()) {
        std::string err_str("Can't open file ");
        err_str.append(file_name);
        throw std::runtime_error(err_str);
    }
}

inline std::tuple<size_t, size_t, size_t> read_header(std::ifstream& stream)
{
    std::string word;
    stream >> word;
    if (word != "OFF") {
        throw std::runtime_error("Bad off file");
    }

    size_t n_vertices, n_faces, n_edges;
    stream >> n_vertices >> n_faces >> n_edges;

    return std::make_tuple(n_vertices, n_faces, n_edges);
}

template<int N>
inline void write_header(std::ofstream& stream, size_t nv, size_t nf)
{
    stream << "OFF" << std::endl;
    if (N != 0 && nf % N != 0) {
        std::string err_str("Input buffer size is not divisible by ");
        err_str.append(std::to_string(N));
        throw std::runtime_error(err_str);
    }
    auto n_vertices = nv / 3;
    auto n_faces = N == 0 ? 0 : nf / N;
    stream << n_vertices << " " << n_faces << " " << 0 << std::endl;
}

template<typename T>
inline void read_positions(std::ifstream& stream,
                           size_t count,
                           std::vector<T>& buffer)
{
    buffer.clear();
    buffer.resize(count * 3);
    for (size_t i = 0; i < buffer.size(); ++i) {
        stream >> buffer[i];
    }
}

template<int N, typename T>
inline void read_indices(std::ifstream& stream,
                         size_t count,
                         std::vector<T>& buffer)
{
    buffer.clear();
    buffer.resize(count * N);
    for (size_t i = 0; i < buffer.size(); ++i) {
        if (i % N == 0) {
            T dummy;
            stream >> dummy;
        }
        stream >> buffer[i];
    }
}

template<typename T>
inline void write_positions(std::ofstream& stream, const std::vector<T>& buffer)
{
    for (size_t i = 0; i < buffer.size(); i += 3) {
        stream << buffer[i] << " " << buffer[i + 1] << " " << buffer[i + 2]
               << std::endl;
    }
}

template<int N, typename T>
inline void write_indices(std::ofstream& stream, const std::vector<T>& buffer)
{
    for (size_t i = 0; i < buffer.size(); ++i) {
        if (i % N == 0) {
            stream << N << " ";
        }
        stream << buffer[i];
        if (N != 0 && i % N < N - 1) {
            stream << " ";
        }
        else {
            stream << std::endl;
        }
    }
}

} // namespace _impl

template<typename T>
void read_off(const std::string& file_name, std::vector<T>& positions)
{
    std::ifstream stream(file_name);
    _impl::check_stream(stream, file_name);

    auto[n_vertices, n_faces, dummy] = _impl::read_header(stream);

    _impl::read_positions(stream, n_vertices, positions);
}

template<int N, typename T1, typename T2>
void read_off(const std::string& file_name,
              std::vector<T1>& positions,
              std::vector<T2>& indices)
{
    std::ifstream stream(file_name);
    _impl::check_stream(stream, file_name);

    auto[n_vertices, n_faces, dummy] = _impl::read_header(stream);

    _impl::read_positions(stream, n_vertices, positions);

    _impl::read_indices<N>(stream, n_faces, indices);
}

template<typename T1, typename T2>
void write_off(const std::string& file_name, const std::vector<T1>& positions)
{
    std::ofstream stream(file_name);
    _impl::check_stream(stream, file_name);

    _impl::write_header<0>(stream, positions.size(), 0);

    _impl::write_positions(stream, positions);
}

template<int N, typename T1, typename T2>
void write_off(const std::string& file_name,
               const std::vector<T1>& positions,
               const std::vector<T2>& indices)
{
    std::ofstream stream(file_name);
    _impl::check_stream(stream, file_name);

    _impl::write_header<N>(stream, positions.size(), indices.size());

    _impl::write_positions(stream, positions);

    _impl::write_indices<N>(stream, indices);
}

} // namespace Euclid
