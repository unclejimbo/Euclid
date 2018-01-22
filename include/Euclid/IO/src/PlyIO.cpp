#include <Euclid/Util/Assert.h>
#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
#include <optional>
#include <sstream>

namespace Euclid
{

namespace _impl
{

/** Read a line in ply header, ignore comments.*/
static inline std::vector<std::string> _read_ply_header_line(
    std::ifstream& stream)
{
    std::vector<std::string> words;
    do {
        words.clear();
        std::string line;
        std::getline(stream, line);
        std::istringstream iss(line);
        std::copy(std::istream_iterator<std::string>(iss),
                  std::istream_iterator<std::string>(),
                  std::back_inserter(words));
    } while (words[0] == "comment");
    return words;
}

/** Check if a file is opened.*/
static inline void _check_stream(const std::ifstream& stream,
                                 const std::string& file_name)
{
    if (!stream.is_open()) {
        std::string err_str("Can't open file ");
        err_str.append(file_name);
        throw std::runtime_error(err_str);
    }
}

/** Check if a file is opened.*/
static inline void _check_stream(const std::ofstream& stream,
                                 const std::string& file_name)
{
    if (!stream.is_open()) {
        std::string err_str("Can't open file ");
        err_str.append(file_name);
        throw std::runtime_error(err_str);
    }
}

/** Check if system is little endian.*/
static inline bool _sys_little_endian()
{
    union
    {
        uint32_t value;
        uint8_t bytes[4];
    } checker = {.value = 0x00000001};
    return checker.bytes[0] == 1;
}

/** Swap endianness for a buffer.*/
static inline void _swap_bytes(char* bytes, size_t N)
{
    for (auto i = 0; i < (N + 1) / 2; ++i) {
        std::swap(bytes[i], bytes[N - 1 - i]);
    }
}

/** Get an ascii value from the stream.*/
template<typename T>
static inline T _get_ascii(std::ifstream& stream)
{
    T value;
    stream >> value;
    return value;
}

/** Get a binary value from the stream.*/
template<typename T, int N>
static inline T _get_binary(std::ifstream& stream,
                            bool file_little_endian,
                            bool sys_little_endian)
{
    static_assert(N <= sizeof(T));

    // Get bytes
    char bytes[N];
    stream.read(bytes, N);

    // Swap if inconsistent
    if (file_little_endian != sys_little_endian && N > 1) {
        _swap_bytes(bytes, N);
    }

    // Convert to number
    union
    {
        T value;
        char bytes[sizeof(T)];
    } number;
    for (auto i = 0; i < sizeof(T); ++i) {
        number.bytes[i] = 0;
    }
    if (sys_little_endian) {
        for (auto i = 0; i < N; ++i) {
            number.bytes[i] = bytes[i];
        }
    }
    else {
        for (auto i = 0; i < N; ++i) {
            number.bytes[sizeof(T) - N + i] = bytes[i];
        }
    }

    return number.value;
}

/** Put an ascii value to the stream.*/
template<typename T>
static inline void _put_ascii(std::ofstream& stream, T value)
{
    stream << value;
}

/** Put a binary value to the stream.*/
template<typename T, int N>
static inline void _put_binary(std::ofstream& stream,
                               T value,
                               bool file_little_endian,
                               bool sys_little_endian)
{
    static_assert(N <= sizeof(T));

    // Convert to bytes
    union
    {
        T value;
        char bytes[sizeof(T)];
    } number;
    number.value = value;

    char bytes[N];
    if (sys_little_endian) {
        for (auto i = 0; i < N; ++i) {
            bytes[i] = number.bytes[i];
        }
    }
    else {
        for (auto i = 0; i < N; ++i) {
            bytes[i] = number.bytes[sizeof(T) - N + i];
        }
    }

    // Swap if inconsistent
    if (file_little_endian != sys_little_endian && N > 1) {
        _swap_bytes(bytes, N);
    }

    // Write bytes
    stream.write(bytes, N);
}

/** Make a specific type of PlyProperty.*/
template<typename T>
static inline std::unique_ptr<PlyProperty> _make_property(
    const std::string& name,
    bool is_list)
{
    return nullptr;
}

template<>
inline std::unique_ptr<PlyProperty> _make_property<double>(
    const std::string& name,
    bool is_list)
{
    return std::make_unique<PlyDoubleProperty>(name, is_list);
}

template<>
inline std::unique_ptr<PlyProperty> _make_property<float>(
    const std::string& name,
    bool is_list)
{
    return std::make_unique<PlyFloatProperty>(name, is_list);
}

template<>
inline std::unique_ptr<PlyProperty> _make_property<int>(const std::string& name,
                                                        bool is_list)
{
    return std::make_unique<PlyIntProperty>(name, is_list);
}

template<>
inline std::unique_ptr<PlyProperty> _make_property<unsigned>(
    const std::string& name,
    bool is_list)
{
    return std::make_unique<PlyUintProperty>(name, is_list);
}

template<>
inline std::unique_ptr<PlyProperty> _make_property<short>(
    const std::string& name,
    bool is_list)
{
    return std::make_unique<PlyShortProperty>(name, is_list);
}

template<>
inline std::unique_ptr<PlyProperty> _make_property<unsigned short>(
    const std::string& name,
    bool is_list)
{
    return std::make_unique<PlyUshortProperty>(name, is_list);
}

template<>
inline std::unique_ptr<PlyProperty> _make_property<char>(
    const std::string& name,
    bool is_list)
{
    return std::make_unique<PlyCharProperty>(name, is_list);
}

template<>
inline std::unique_ptr<PlyProperty> _make_property<unsigned char>(
    const std::string& name,
    bool is_list)
{
    return std::make_unique<PlyUcharProperty>(name, is_list);
}

/** Implementation of reading ply header.*/
static PlyHeader _read_ply_header(std::ifstream& stream)
{
    std::string line;
    std::getline(stream, line);
    if (line != "ply") {
        throw std::runtime_error("Bad ply file");
    }

    PlyFormat format;
    auto words = _read_ply_header_line(stream);
    if (words[0] != "format") {
        throw std::runtime_error("Bad ply file");
    }
    if (words[1] == "ascii") {
        format = PlyFormat::ascii;
    }
    else if (words[1] == "binary_little_endian") {
        format = PlyFormat::binary_little_endian;
    }
    else if (words[1] == "binary_big_endian") {
        format = PlyFormat::binary_big_endian;
    }
    else {
        throw std::runtime_error("Bad ply file");
    }
    PlyHeader header(format);

    words = _read_ply_header_line(stream);
    do {
        PlyElement element(words[1], std::stoi(words[2]));
        do {
            words = _read_ply_header_line(stream);
            if (words[0] == "property" && words[1] != "list") {
                if (words[1] == "double") {
                    auto property =
                        std::make_unique<PlyDoubleProperty>(words[2]);
                    element.add_property(std::move(property));
                }
                else if (words[1] == "float") {
                    auto property =
                        std::make_unique<PlyFloatProperty>(words[2]);
                    element.add_property(std::move(property));
                }
                else if (words[1] == "int") {
                    auto property = std::make_unique<PlyIntProperty>(words[2]);
                }
                else if (words[1] == "uint") {
                    auto property = std::make_unique<PlyUintProperty>(words[2]);
                }
                else if (words[1] == "short") {
                    auto property =
                        std::make_unique<PlyShortProperty>(words[2]);
                }
                else if (words[1] == "ushort") {
                    auto property =
                        std::make_unique<PlyUshortProperty>(words[2]);
                }
                else if (words[1] == "char") {
                    auto property = std::make_unique<PlyCharProperty>(words[2]);
                }
                else if (words[1] == "uchar") {
                    auto property =
                        std::make_unique<PlyUcharProperty>(words[2]);
                    element.add_property(std::move(property));
                }
                else {
                    std::string err_str("Invalid property type ");
                    err_str.append(words[1]);
                    throw std::runtime_error(err_str);
                }
            }
            else if (words[0] == "property" && words[1] == "list") {
                if (words[3] == "double") {
                    auto property =
                        std::make_unique<PlyDoubleProperty>(words[4], true);
                    element.add_property(std::move(property));
                }
                else if (words[3] == "float") {
                    auto property =
                        std::make_unique<PlyFloatProperty>(words[4], true);
                    element.add_property(std::move(property));
                }
                else if (words[3] == "int") {
                    auto property =
                        std::make_unique<PlyIntProperty>(words[4], true);
                    element.add_property(std::move(property));
                }
                else if (words[3] == "uint") {
                    auto property =
                        std::make_unique<PlyUintProperty>(words[4], true);
                    element.add_property(std::move(property));
                }
                else if (words[3] == "short") {
                    auto property =
                        std::make_unique<PlyShortProperty>(words[4], true);
                    element.add_property(std::move(property));
                }
                else if (words[3] == "ushort") {
                    auto property =
                        std::make_unique<PlyUshortProperty>(words[4], true);
                    element.add_property(std::move(property));
                }
                else if (words[3] == "char") {
                    auto property =
                        std::make_unique<PlyCharProperty>(words[4], true);
                    element.add_property(std::move(property));
                }
                else if (words[3] == "uchar") {
                    auto property =
                        std::make_unique<PlyUcharProperty>(words[4], true);
                    element.add_property(std::move(property));
                }
                else {
                    std::string err_str("Invalid property type ");
                    err_str.append(words[3]);
                    throw std::runtime_error(err_str);
                }
            }
            else {
                // Ignore
            }
        } while (words[0] == "property");
        header.add_element(std::move(element));
    } while (words[0] != "end_header");

    return header;
}

/** Implementation of writing the ply header.*/
static void _write_ply_header(std::ofstream& stream, const PlyHeader& header)
{
    stream << "ply" << std::endl
           << "comment Generated by Euclid" << std::endl
           << "format ";
    if (header.format() == PlyFormat::ascii) {
        stream << "ascii ";
    }
    else if (header.format() == PlyFormat::binary_little_endian) {
        stream << "binary_little_endian ";
    }
    else {
        stream << "binary_big_endian ";
    }
    stream << "1.0" << std::endl;
    for (const auto& e : header) {
        stream << "element " << e.name() << " " << e.count() << std::endl;
        for (const auto& p : e) {
            stream << "property ";
            if (p.is_list()) {
                stream << "list uchar ";
            }
            stream << p.type_str() << " " << p.name() << std::endl;
        }
    }
    stream << "end_header" << std::endl;
}

} // namespace _impl

//-----------------PlyProperty----------------------

void PlyDoubleProperty::apply(PlyReader& reader,
                              std::ifstream& stream,
                              PlyFormat format) const
{
    reader.read(this, stream, format);
}

void PlyDoubleProperty::apply(PlyWriter& writer,
                              std::ofstream& stream,
                              PlyFormat format) const
{
    writer.write(this, stream, format);
}

PlyDoubleProperty::value_type PlyDoubleProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyDoubleProperty::value_type PlyDoubleProperty::get_binary(
    std::ifstream& stream,
    bool file_little_endian,
    bool sys_little_endian) const
{
    return _impl::_get_binary<value_type, 8>(
        stream, file_little_endian, sys_little_endian);
}

void PlyDoubleProperty::put_ascii(std::ofstream& stream,
                                  PlyDoubleProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyDoubleProperty::put_binary(std::ofstream& stream,
                                   PlyDoubleProperty::value_type value,
                                   bool file_little_endian,
                                   bool sys_little_endian) const
{
    _impl::_put_binary<value_type, 8>(
        stream, value, file_little_endian, sys_little_endian);
}

void PlyFloatProperty::apply(PlyReader& reader,
                             std::ifstream& stream,
                             PlyFormat format) const
{
    reader.read(this, stream, format);
}

void PlyFloatProperty::apply(PlyWriter& writer,
                             std::ofstream& stream,
                             PlyFormat format) const
{
    writer.write(this, stream, format);
}

PlyFloatProperty::value_type PlyFloatProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyFloatProperty::value_type PlyFloatProperty::get_binary(
    std::ifstream& stream,
    bool file_little_endian,
    bool sys_little_endian) const
{
    return _impl::_get_binary<value_type, 4>(
        stream, file_little_endian, sys_little_endian);
}

void PlyFloatProperty::put_ascii(std::ofstream& stream,
                                 PlyFloatProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyFloatProperty::put_binary(std::ofstream& stream,
                                  PlyFloatProperty::value_type value,
                                  bool file_little_endian,
                                  bool sys_little_endian) const
{
    _impl::_put_binary<value_type, 4>(
        stream, value, file_little_endian, sys_little_endian);
}

void PlyIntProperty::apply(PlyReader& reader,
                           std::ifstream& stream,
                           PlyFormat format) const
{
    reader.read(this, stream, format);
}

void PlyIntProperty::apply(PlyWriter& writer,
                           std::ofstream& stream,
                           PlyFormat format) const
{
    writer.write(this, stream, format);
}

PlyIntProperty::value_type PlyIntProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyIntProperty::value_type PlyIntProperty::get_binary(
    std::ifstream& stream,
    bool file_little_endian,
    bool sys_little_endian) const
{
    return _impl::_get_binary<value_type, 4>(
        stream, file_little_endian, sys_little_endian);
}

void PlyIntProperty::put_ascii(std::ofstream& stream,
                               PlyIntProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyIntProperty::put_binary(std::ofstream& stream,
                                PlyIntProperty::value_type value,
                                bool file_little_endian,
                                bool sys_little_endian) const
{
    _impl::_put_binary<value_type, 4>(
        stream, value, file_little_endian, sys_little_endian);
}

void PlyUintProperty::apply(PlyReader& reader,
                            std::ifstream& stream,
                            PlyFormat format) const
{
    reader.read(this, stream, format);
}

void PlyUintProperty::apply(PlyWriter& writer,
                            std::ofstream& stream,
                            PlyFormat format) const
{
    writer.write(this, stream, format);
}

PlyUintProperty::value_type PlyUintProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyUintProperty::value_type PlyUintProperty::get_binary(
    std::ifstream& stream,
    bool file_little_endian,
    bool sys_little_endian) const
{
    return _impl::_get_binary<value_type, 4>(
        stream, file_little_endian, sys_little_endian);
}

void PlyUintProperty::put_ascii(std::ofstream& stream,
                                PlyUintProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyUintProperty::put_binary(std::ofstream& stream,
                                 PlyUintProperty::value_type value,
                                 bool file_little_endian,
                                 bool sys_little_endian) const
{
    _impl::_put_binary<value_type, 4>(
        stream, value, file_little_endian, sys_little_endian);
}

void PlyShortProperty::apply(PlyReader& reader,
                             std::ifstream& stream,
                             PlyFormat format) const
{
    reader.read(this, stream, format);
}

void PlyShortProperty::apply(PlyWriter& writer,
                             std::ofstream& stream,
                             PlyFormat format) const
{
    writer.write(this, stream, format);
}

PlyShortProperty::value_type PlyShortProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyShortProperty::value_type PlyShortProperty::get_binary(
    std::ifstream& stream,
    bool file_little_endian,
    bool sys_little_endian) const
{
    return _impl::_get_binary<value_type, 2>(
        stream, file_little_endian, sys_little_endian);
}

void PlyShortProperty::put_ascii(std::ofstream& stream,
                                 PlyShortProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyShortProperty::put_binary(std::ofstream& stream,
                                  PlyShortProperty::value_type value,
                                  bool file_little_endian,
                                  bool sys_little_endian) const
{
    _impl::_put_binary<value_type, 2>(
        stream, value, file_little_endian, sys_little_endian);
}

void PlyUshortProperty::apply(PlyReader& reader,
                              std::ifstream& stream,
                              PlyFormat format) const
{
    reader.read(this, stream, format);
}

void PlyUshortProperty::apply(PlyWriter& writer,
                              std::ofstream& stream,
                              PlyFormat format) const
{
    writer.write(this, stream, format);
}

PlyUshortProperty::value_type PlyUshortProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyUshortProperty::value_type PlyUshortProperty::get_binary(
    std::ifstream& stream,
    bool file_little_endian,
    bool sys_little_endian) const
{
    return _impl::_get_binary<value_type, 2>(
        stream, file_little_endian, sys_little_endian);
}

void PlyUshortProperty::put_ascii(std::ofstream& stream,
                                  PlyUshortProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyUshortProperty::put_binary(std::ofstream& stream,
                                   PlyUshortProperty::value_type value,
                                   bool file_little_endian,
                                   bool sys_little_endian) const
{
    _impl::_put_binary<value_type, 2>(
        stream, value, file_little_endian, sys_little_endian);
}

void PlyCharProperty::apply(PlyReader& reader,
                            std::ifstream& stream,
                            PlyFormat format) const
{
    reader.read(this, stream, format);
}

void PlyCharProperty::apply(PlyWriter& writer,
                            std::ofstream& stream,
                            PlyFormat format) const
{
    writer.write(this, stream, format);
}

PlyCharProperty::value_type PlyCharProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyCharProperty::value_type PlyCharProperty::get_binary(
    std::ifstream& stream,
    bool file_little_endian,
    bool sys_little_endian) const
{
    return _impl::_get_binary<value_type, 1>(
        stream, file_little_endian, sys_little_endian);
}

void PlyCharProperty::put_ascii(std::ofstream& stream,
                                PlyCharProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyCharProperty::put_binary(std::ofstream& stream,
                                 PlyCharProperty::value_type value,
                                 bool file_little_endian,
                                 bool sys_little_endian) const
{
    _impl::_put_binary<value_type, 1>(
        stream, value, file_little_endian, sys_little_endian);
}

void PlyUcharProperty::apply(PlyReader& reader,
                             std::ifstream& stream,
                             PlyFormat format) const
{
    reader.read(this, stream, format);
}

void PlyUcharProperty::apply(PlyWriter& writer,
                             std::ofstream& stream,
                             PlyFormat format) const
{
    writer.write(this, stream, format);
}

PlyUcharProperty::value_type PlyUcharProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyUcharProperty::value_type PlyUcharProperty::get_binary(
    std::ifstream& stream,
    bool file_little_endian,
    bool sys_little_endian) const
{
    return _impl::_get_binary<value_type, 1>(
        stream, file_little_endian, sys_little_endian);
}

void PlyUcharProperty::put_ascii(std::ofstream& stream,
                                 PlyUcharProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyUcharProperty::put_binary(std::ofstream& stream,
                                  PlyUcharProperty::value_type value,
                                  bool file_little_endian,
                                  bool sys_little_endian) const
{
    _impl::_put_binary<value_type, 1>(
        stream, value, file_little_endian, sys_little_endian);
}

//-------------------CommonPlyReader------------------------

PlyReader::PlyReader()
{
    _sys_little_endian = _impl::_sys_little_endian();
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::on_read(
    const PlyHeader& header)
{
    for (const auto& e : header) {
        if (e.name() == "vertex") {
            for (auto i = 0; i < e.n_props();) {
                if (e.property(i)->name() == "x") {
                    EASSERT(e.property(i + 1)->name() == "y");
                    EASSERT(e.property(i + 2)->name() == "z");
                    _positions.clear();
                    _positions.reserve(e.count() * 3);
                    _positions.shrink_to_fit();
                    i += 3;
                }
                else if (_normals != nullptr && e.property(i)->name() == "nx") {
                    EASSERT(e.property(i + 1)->name() == "ny");
                    EASSERT(e.property(i + 2)->name() == "nz");
                    _normals->clear();
                    _normals->reserve(e.count() * 3);
                    _normals->shrink_to_fit();
                    i += 3;
                }
                else if (_texcoords != nullptr &&
                         (e.property(i)->name() == "s" ||
                          e.property(i)->name() == "texture_u")) {
                    EASSERT(e.property(i + 1)->name() == "t" ||
                            e.property(i + 1)->name() == "texture_v");
                    _texcoords->clear();
                    _texcoords->reserve(e.count() * 2);
                    _texcoords->shrink_to_fit();
                    i += 2;
                }
                else {
                    ++i;
                }
            }
        }
        else if (e.name() == "face") {
            for (const auto& p : e) {
                if (p.name() == "vertex_index" ||
                    p.name() == "vertex_indices") {
                    _indices->clear();
                    _indices->reserve(e.count() * VN);
                    _indices->shrink_to_fit();
                }
            }
        }
        else {
            // Ignore
        }
    }
}
template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::read(
    const PlyDoubleProperty* property,
    std::ifstream& stream,
    PlyFormat format)
{
    _store_float(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::read(
    const PlyFloatProperty* property,
    std::ifstream& stream,
    PlyFormat format)
{
    _store_float(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::read(
    const PlyIntProperty* property,
    std::ifstream& stream,
    PlyFormat format)
{
    _store_indices(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::read(
    const PlyUintProperty* property,
    std::ifstream& stream,
    PlyFormat format)
{
    _store_indices(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::read(
    const PlyUcharProperty* property,
    std::ifstream& stream,
    PlyFormat format)
{
    _store_color(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
inline void CommonPlyReader<FloatType, IndexType, ColorType, VN>::_store_float(
    const TPlyProperty* property,
    std::ifstream& stream,
    PlyFormat format)
{
    // First read out value then decide what to do with it.
    // By this way we can safely skip unrecognised properties.
    FloatType value;
    if (format == PlyFormat::ascii) {
        auto v = property->get_ascii(stream);
        value = static_cast<FloatType>(v);
    }
    else {
        auto v = property->get_binary(stream,
                                      format == PlyFormat::binary_little_endian,
                                      _sys_little_endian);
        value = static_cast<FloatType>(v);
    }

    if (property->name() == "x" || property->name() == "y" ||
        property->name() == "z") {
        _positions.push_back(value);
    }
    else if (_normals != nullptr &&
             (property->name() == "nx" || property->name() == "ny" ||
              property->name() == "nz")) {
        _normals->push_back(value);
    }
    else if (_texcoords != nullptr &&
             (property->name() == "s" || property->name() == "texture_u" ||
              property->name() == "t" || property->name() == "texture_v")) {
        _texcoords->push_back(value);
    }
    else {
        // Ignore
    }
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
inline void CommonPlyReader<FloatType, IndexType, ColorType, VN>::_store_color(
    const TPlyProperty* property,
    std::ifstream& stream,
    PlyFormat format)
{
    ColorType value;
    if (format == PlyFormat::ascii) {
        auto v = property->get_ascii(stream);
        value = static_cast<ColorType>(v);
    }
    else {
        auto v = property->get_binary(stream,
                                      format == PlyFormat::binary_little_endian,
                                      _sys_little_endian);
        value = static_cast<ColorType>(v);
    }

    if (property->name() == "red" || property->name() == "green" ||
        property->name() == "blue" || property->name() == "alpha") {
        _colors->push_back(value);
    }
    else {
        // Ignore
    }
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
inline void
CommonPlyReader<FloatType, IndexType, ColorType, VN>::_store_indices(
    const TPlyProperty* property,
    std::ifstream& stream,
    PlyFormat format)
{
    if (property->is_list()) {
        std::vector<IndexType> values;
        if (format == PlyFormat::ascii) {
            unsigned count;
            stream >> count;
            if (count != VN) {
                std::string err_str("Number of vertices per face should be ");
                err_str.append(std::to_string(VN));
                err_str.append(", rather than ");
                err_str.append(std::to_string(count));
                throw std::runtime_error(err_str);
            }

            values.resize(count);
            for (auto i = 0; i < count; ++i) {
                auto v = property->get_ascii(stream);
                values[i] = static_cast<IndexType>(v);
            }
        }
        else {
            char byte;
            stream.get(byte);
            auto count = static_cast<unsigned>(byte);
            if (count != VN) {
                std::string err_str("Number of vertices per face should be ");
                err_str.append(std::to_string(VN));
                err_str.append(", rather than ");
                err_str.append(std::to_string(count));
                throw std::runtime_error(err_str);
            }

            values.resize(count);
            for (auto i = 0; i < count; ++i) {
                auto v = property->get_binary(
                    stream,
                    format == PlyFormat::binary_little_endian,
                    _sys_little_endian);
                values[i] = static_cast<IndexType>(v);
            }
        }

        if (_indices != nullptr && (property->name() == "vertex_index" ||
                                    property->name() == "vertex_indices")) {
            std::copy(
                values.begin(), values.end(), std::back_inserter(*_indices));
        }
        else {
            // Ignore
        }
    }
    else {
        // Ignore
    }
}

//------------------CommonPlyWriter-----------------------

PlyWriter::PlyWriter()
{
    _sys_little_endian = _impl::_sys_little_endian();
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::on_write()
{
    _piter = _iiter = _niter = _titer = _citer = 0;
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
PlyHeader CommonPlyWriter<FloatType, IndexType, ColorType, VN>::generate_header(
    PlyFormat format) const
{
    PlyHeader header(format);

    PlyElement ve("vertex", _positions.size() / 3);
    ve.add_property(_impl::_make_property<FloatType>("x", false));
    ve.add_property(_impl::_make_property<FloatType>("y", false));
    ve.add_property(_impl::_make_property<FloatType>("z", false));
    if (_normals != nullptr) {
        ve.add_property(_impl::_make_property<FloatType>("nx", false));
        ve.add_property(_impl::_make_property<FloatType>("ny", false));
        ve.add_property(_impl::_make_property<FloatType>("nz", false));
    }
    if (_texcoords != nullptr) {
        ve.add_property(_impl::_make_property<FloatType>("s", false));
        ve.add_property(_impl::_make_property<FloatType>("t", false));
    }
    if (_colors != nullptr) {
        ve.add_property(_impl::_make_property<unsigned char>("red", false));
        ve.add_property(_impl::_make_property<unsigned char>("green", false));
        ve.add_property(_impl::_make_property<unsigned char>("blue", false));
        if (_has_alpha) {
            ve.add_property(
                _impl::_make_property<unsigned char>("alpha", false));
        }
    }
    header.add_element(std::move(ve));

    if (_indices != nullptr) {
        PlyElement fe("face", _indices->size() / VN);
        fe.add_property(
            _impl::_make_property<IndexType>("vertex_indices", true));
        header.add_element(std::move(fe));
    }

    return header;
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyDoubleProperty* property,
    std::ofstream& stream,
    PlyFormat format)
{
    _write_float(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyFloatProperty* property,
    std::ofstream& stream,
    PlyFormat format)
{
    _write_float(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyIntProperty* property,
    std::ofstream& stream,
    PlyFormat format)
{
    _write_indices(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyUintProperty* property,
    std::ofstream& stream,
    PlyFormat format)
{
    _write_indices(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyUcharProperty* property,
    std::ofstream& stream,
    PlyFormat format)
{
    _write_color(property, stream, format);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::_write_float(
    const TPlyProperty* property,
    std::ofstream& stream,
    PlyFormat format)
{
    std::optional<FloatType> value;
    if (property->name() == "x" || property->name() == "y" ||
        property->name() == "z") {
        value = _positions[_piter++];
    }
    else if (property->name() == "nx" || property->name() == "ny" ||
             property->name() == "nz") {
        EASSERT(_normals != nullptr);
        value = (*_normals)[_niter++];
    }
    else if (property->name() == "s" || property->name() == "texture_u" ||
             property->name() == "t" || property->name() == "texture_v") {
        EASSERT(_texcoords != nullptr);
        value = (*_texcoords)[_titer++];
    }
    else {
        // Ignore
    }

    if (value.has_value()) {
        auto v = static_cast<typename TPlyProperty::value_type>(*value);
        if (format == PlyFormat::ascii) {
            property->put_ascii(stream, v);
        }
        else {
            property->put_binary(stream,
                                 v,
                                 format == PlyFormat::binary_little_endian,
                                 _sys_little_endian);
        }
    }
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::_write_color(
    const TPlyProperty* property,
    std::ofstream& stream,
    PlyFormat format)
{
    std::optional<ColorType> value;
    if (property->name() == "red" || property->name() == "green" ||
        property->name() == "blue" || property->name() == "alpha") {
        EASSERT(_colors != nullptr);
        value = (*_colors)[_citer++];
    }
    else {
        // Ignore
    }

    if (value.has_value()) {
        auto v = static_cast<typename TPlyProperty::value_type>(*value);
        if (format == PlyFormat::ascii) {
            property->put_ascii(stream, v);
        }
        else {
            property->put_binary(stream,
                                 v,
                                 format == PlyFormat::binary_little_endian,
                                 _sys_little_endian);
        }
    }
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::_write_indices(
    const TPlyProperty* property,
    std::ofstream& stream,
    PlyFormat format)
{
    if (property->name() == "vertex_index" ||
        property->name() == "vertex_indices") {
        EASSERT(_indices != nullptr);

        // Write vertex number
        if (format == PlyFormat::ascii) {
            stream << VN << " ";
        }
        else {
            auto vn = static_cast<char>(VN);
            stream.put(vn);
        }

        // Write index
        for (auto i = 0; i < VN; ++i) {
            auto v = (*_indices)[_iiter++];
            if (format == PlyFormat::ascii) {
                property->put_ascii(stream, v);
                if (i < VN - 1) {
                    stream << " ";
                }
            }
            else {
                property->put_binary(stream,
                                     v,
                                     format == PlyFormat::binary_little_endian,
                                     _sys_little_endian);
            }
        }
    }
    else {
        // Ignore
    }
}

//----------------Free Functions-------------------

PlyHeader read_ply_header(const std::string& file_name)
{
    std::ifstream stream(file_name);
    _impl::_check_stream(stream, file_name);

    return _impl::_read_ply_header(stream);
}

void read_ply(const std::string& file_name, PlyReader& reader)
{
    std::ifstream stream(file_name);
    _impl::_check_stream(stream, file_name);

    auto header = _impl::_read_ply_header(stream);
    reader.on_read(header);

    if (header.format() != PlyFormat::ascii) {
        auto pos = stream.tellg();
        stream.close();
        stream.open(file_name, std::ios::binary);
        stream.seekg(pos);
    }

    for (const auto& elem : header) {
        for (auto i = 0; i < elem.count(); ++i) {
            for (const auto& prop : elem) {
                prop.apply(reader, stream, header.format());
            }
        }
    }
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void read_ply(const std::string& file_name,
              std::vector<FloatType>& vertices,
              std::vector<IndexType>* indices,
              std::vector<FloatType>* normals,
              std::vector<FloatType>* texcoords,
              std::vector<ColorType>* colors)
{
    CommonPlyReader<FloatType, IndexType, ColorType, VN> reader(
        vertices, indices, normals, texcoords, colors);
    read_ply(file_name, reader);
}

void write_ply(const std::string& file_name,
               PlyWriter& writer,
               PlyFormat format)
{
    std::ofstream stream(file_name, std::ios::out | std::ios::trunc);
    _impl::_check_stream(stream, file_name);

    writer.on_write();

    auto header = writer.generate_header(format);
    _impl::_write_ply_header(stream, header);

    if (format != PlyFormat::ascii) {
        stream.close();
        stream.open(file_name, std::ios::binary | std::ios::app);
    }

    for (const auto& elem : header) {
        for (auto i = 0; i < elem.count(); ++i) {
            for (auto j = 0; j < elem.n_props(); ++j) {
                const auto prop = elem.property(j);
                prop->apply(writer, stream, format);
                if (format == PlyFormat::ascii && j < elem.n_props() - 1) {
                    stream << " ";
                }
            }
            if (format == PlyFormat::ascii) {
                stream << std::endl;
            }
        }
    }
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void write_ply(const std::string& file_name,
               std::vector<FloatType>& vertices,
               std::vector<IndexType>* indices,
               std::vector<FloatType>* normals,
               std::vector<FloatType>* texcoords,
               std::vector<ColorType>* colors,
               PlyFormat format)
{
    CommonPlyWriter<FloatType, IndexType, ColorType, VN> writer(
        vertices, indices, normals, texcoords, colors);
    write_ply(file_name, writer, format);
}

} // namespace Euclid