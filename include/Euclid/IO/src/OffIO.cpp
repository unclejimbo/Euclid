#include <fstream>

namespace Euclid
{

template<typename T1, typename T2>
void read_off(const std::string& file_name,
    std::vector<T1>& positions,
    std::vector<T2>* indices)
{
    std::ifstream stream(file_name);
    if (!stream.is_open()) {
        std::string err_str("Can't open file ");
        err_str.append(file_name);
        throw std::runtime_error(err_str);
    }

    std::string word;
    stream >> word;
    if (word != "OFF") {
        throw std::runtime_error("Bad off file");
    }

    unsigned n_vertices, n_faces, dummy;
    stream >> n_vertices >> n_faces >> dummy;

    positions.clear();
    positions.resize(n_vertices * 3);
    for (auto i = 0; i < n_vertices; ++i) {
        stream >> positions[3 * i] >>
            positions[3 * i + 1] >>
            positions[3 * i + 2];
    }

    if (n_faces != 0 && indices != nullptr) {
        indices->clear();
        indices->resize(n_faces * 3);
        for (auto i = 0; i < n_faces; ++i) {
            T2 dummy;
            stream >> dummy >>
                (*indices)[3 * i] >>
                (*indices)[3 * i + 1] >>
                (*indices)[3 * i + 2];
        }
    }
}


template<typename T1, typename T2>
void write_off(const std::string& file_name,
    const std::vector<T1>& positions,
    const std::vector<T2>* indices)
{
    std::ofstream stream(file_name);
    if (!stream.is_open()) {
        std::string err_str("Can't open file ");
        err_str.append(file_name);
        throw std::runtime_error(err_str);
    }

    stream << "OFF" << std::endl;

    auto n_vertices = positions.size() / 3;
    auto n_faces = 0;
    if (indices != nullptr) {
        n_faces = indices->size() / 3;
    }
    stream << n_vertices << " " << n_faces << " " << 0 << std::endl;

    for (auto i = 0; i < n_vertices; ++i) {
        stream << positions[i * 3] << " "
            << positions[i * 3 + 1] << " "
            << positions[i * 3 + 2] << std::endl;
    }

    for (auto i = 0; i < n_faces; ++i) {
        stream << 3 << " " <<
            (*indices)[i * 3] << " " <<
            (*indices)[i * 3 + 1] << " " <<
            (*indices)[i * 3 + 2] << std::endl;
    }
}

} // namespace Euclid
