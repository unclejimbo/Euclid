#include <Euclid/Util/Assert.h>
#include <algorithm>
#include <array>
#include <exception>
#include <fstream>
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

/** Check if the endianness should be swaped.*/
static inline bool _should_swap(PlyFormat format)
{
    union
    {
        uint16_t i;
        char c[2];
    } bint = {0x0001};
    auto is_little_endian = (bint.c[0] == 1);
    return (is_little_endian && format == PlyFormat::binary_big_endian) ||
           (!is_little_endian && format == PlyFormat::binary_little_endian);
}

/** Swap endianness for a buffer.*/
static inline void _swap_byte(char* bytes, size_t N)
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
static inline T _get_binary(std::ifstream& stream, bool swap_byte)
{
    union
    {
        T value;
        char bytes[N];
    } bits;
    stream.read(bits.bytes, N);
    if (swap_byte) {
        _impl::_swap_byte(bits.bytes, N);
    }
    return bits.value;
}

/** Put an ascii value to the stream.*/
template<typename T>
static inline void _put_ascii(std::ofstream& stream, T value)
{
    stream << value;
}

/** Put a binary value to the stream.*/
template<typename T, int N>
static inline void _put_binary(std::ofstream& stream, T value, bool swap_byte)
{
    union
    {
        T value;
        char bytes[N];
    } bits;
    bits.value = value;
    if (swap_byte) {
        _impl::_swap_byte(bits.bytes, N);
    }
    stream.write(bits.bytes, N);
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
                              bool is_binary,
                              bool swap_byte) const
{
    reader.read(this, stream, is_binary, swap_byte);
}

void PlyDoubleProperty::apply(PlyWriter& writer,
                              std::ofstream& stream,
                              bool is_binary,
                              bool swap_byte) const
{
    writer.write(this, stream, is_binary, swap_byte);
}

PlyDoubleProperty::value_type PlyDoubleProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyDoubleProperty::value_type PlyDoubleProperty::get_binary(
    std::ifstream& stream,
    bool swap_byte) const
{
    return _impl::_get_binary<value_type, 8>(stream, swap_byte);
}

void PlyDoubleProperty::put_ascii(std::ofstream& stream,
                                  PlyDoubleProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyDoubleProperty::put_binary(std::ofstream& stream,
                                   PlyDoubleProperty::value_type value,
                                   bool swap_byte) const
{
    _impl::_put_binary<value_type, 8>(stream, value, swap_byte);
}

void PlyFloatProperty::apply(PlyReader& reader,
                             std::ifstream& stream,
                             bool is_binary,
                             bool swap_byte) const
{
    reader.read(this, stream, is_binary, swap_byte);
}

void PlyFloatProperty::apply(PlyWriter& writer,
                             std::ofstream& stream,
                             bool is_binary,
                             bool swap_byte) const
{
    writer.write(this, stream, is_binary, swap_byte);
}

PlyFloatProperty::value_type PlyFloatProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyFloatProperty::value_type PlyFloatProperty::get_binary(std::ifstream& stream,
                                                          bool swap_byte) const
{
    return _impl::_get_binary<value_type, 4>(stream, swap_byte);
}

void PlyFloatProperty::put_ascii(std::ofstream& stream,
                                 PlyFloatProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyFloatProperty::put_binary(std::ofstream& stream,
                                  PlyFloatProperty::value_type value,
                                  bool swap_byte) const
{
    _impl::_put_binary<value_type, 4>(stream, value, swap_byte);
}

void PlyIntProperty::apply(PlyReader& reader,
                           std::ifstream& stream,
                           bool is_binary,
                           bool swap_byte) const
{
    reader.read(this, stream, is_binary, swap_byte);
}

void PlyIntProperty::apply(PlyWriter& writer,
                           std::ofstream& stream,
                           bool is_binary,
                           bool swap_byte) const
{
    writer.write(this, stream, is_binary, swap_byte);
}

PlyIntProperty::value_type PlyIntProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyIntProperty::value_type PlyIntProperty::get_binary(std::ifstream& stream,
                                                      bool swap_byte) const
{
    return _impl::_get_binary<value_type, 4>(stream, swap_byte);
}

void PlyIntProperty::put_ascii(std::ofstream& stream,
                               PlyIntProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyIntProperty::put_binary(std::ofstream& stream,
                                PlyIntProperty::value_type value,
                                bool swap_byte) const
{
    _impl::_put_binary<value_type, 4>(stream, value, swap_byte);
}

void PlyUintProperty::apply(PlyReader& reader,
                            std::ifstream& stream,
                            bool is_binary,
                            bool swap_byte) const
{
    reader.read(this, stream, is_binary, swap_byte);
}

void PlyUintProperty::apply(PlyWriter& writer,
                            std::ofstream& stream,
                            bool is_binary,
                            bool swap_byte) const
{
    writer.write(this, stream, is_binary, swap_byte);
}

PlyUintProperty::value_type PlyUintProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyUintProperty::value_type PlyUintProperty::get_binary(std::ifstream& stream,
                                                        bool swap_byte) const
{
    return _impl::_get_binary<value_type, 4>(stream, swap_byte);
}

void PlyUintProperty::put_ascii(std::ofstream& stream,
                                PlyUintProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyUintProperty::put_binary(std::ofstream& stream,
                                 PlyUintProperty::value_type value,
                                 bool swap_byte) const
{
    _impl::_put_binary<value_type, 4>(stream, value, swap_byte);
}

void PlyShortProperty::apply(PlyReader& reader,
                             std::ifstream& stream,
                             bool is_binary,
                             bool swap_byte) const
{
    reader.read(this, stream, is_binary, swap_byte);
}

void PlyShortProperty::apply(PlyWriter& writer,
                             std::ofstream& stream,
                             bool is_binary,
                             bool swap_byte) const
{
    writer.write(this, stream, is_binary, swap_byte);
}

PlyShortProperty::value_type PlyShortProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyShortProperty::value_type PlyShortProperty::get_binary(std::ifstream& stream,
                                                          bool swap_byte) const
{
    return _impl::_get_binary<value_type, 2>(stream, swap_byte);
}

void PlyShortProperty::put_ascii(std::ofstream& stream,
                                 PlyShortProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyShortProperty::put_binary(std::ofstream& stream,
                                  PlyShortProperty::value_type value,
                                  bool swap_byte) const
{
    _impl::_put_binary<value_type, 2>(stream, value, swap_byte);
}

void PlyUshortProperty::apply(PlyReader& reader,
                              std::ifstream& stream,
                              bool is_binary,
                              bool swap_byte) const
{
    reader.read(this, stream, is_binary, swap_byte);
}

void PlyUshortProperty::apply(PlyWriter& writer,
                              std::ofstream& stream,
                              bool is_binary,
                              bool swap_byte) const
{
    writer.write(this, stream, is_binary, swap_byte);
}

PlyUshortProperty::value_type PlyUshortProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyUshortProperty::value_type PlyUshortProperty::get_binary(
    std::ifstream& stream,
    bool swap_byte) const
{
    return _impl::_get_binary<value_type, 2>(stream, swap_byte);
}

void PlyUshortProperty::put_ascii(std::ofstream& stream,
                                  PlyUshortProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyUshortProperty::put_binary(std::ofstream& stream,
                                   PlyUshortProperty::value_type value,
                                   bool swap_byte) const
{
    _impl::_put_binary<value_type, 2>(stream, value, swap_byte);
}

void PlyCharProperty::apply(PlyReader& reader,
                            std::ifstream& stream,
                            bool is_binary,
                            bool swap_byte) const
{
    reader.read(this, stream, is_binary, swap_byte);
}

void PlyCharProperty::apply(PlyWriter& writer,
                            std::ofstream& stream,
                            bool is_binary,
                            bool swap_byte) const
{
    writer.write(this, stream, is_binary, swap_byte);
}

PlyCharProperty::value_type PlyCharProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyCharProperty::value_type PlyCharProperty::get_binary(std::ifstream& stream,
                                                        bool swap_byte) const
{
    return _impl::_get_binary<value_type, 1>(stream, swap_byte);
}

void PlyCharProperty::put_ascii(std::ofstream& stream,
                                PlyCharProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyCharProperty::put_binary(std::ofstream& stream,
                                 PlyCharProperty::value_type value,
                                 bool swap_byte) const
{
    _impl::_put_binary<value_type, 1>(stream, value, swap_byte);
}

void PlyUcharProperty::apply(PlyReader& reader,
                             std::ifstream& stream,
                             bool is_binary,
                             bool swap_byte) const
{
    reader.read(this, stream, is_binary, swap_byte);
}

void PlyUcharProperty::apply(PlyWriter& writer,
                             std::ofstream& stream,
                             bool is_binary,
                             bool swap_byte) const
{
    writer.write(this, stream, is_binary, swap_byte);
}

PlyUcharProperty::value_type PlyUcharProperty::get_ascii(
    std::ifstream& stream) const
{
    return _impl::_get_ascii<value_type>(stream);
}

PlyUcharProperty::value_type PlyUcharProperty::get_binary(std::ifstream& stream,
                                                          bool swap_byte) const
{
    return _impl::_get_binary<value_type, 1>(stream, swap_byte);
}

void PlyUcharProperty::put_ascii(std::ofstream& stream,
                                 PlyUcharProperty::value_type value) const
{
    _impl::_put_ascii(stream, value);
}

void PlyUcharProperty::put_binary(std::ofstream& stream,
                                  PlyUcharProperty::value_type value,
                                  bool swap_byte) const
{
    _impl::_put_binary<value_type, 1>(stream, value, swap_byte);
}

//-------------------CommonPlyReader------------------------

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
    bool is_binary,
    bool swap_byte)
{
    _store_float(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::read(
    const PlyFloatProperty* property,
    std::ifstream& stream,
    bool is_binary,
    bool swap_byte)
{
    _store_float(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::read(
    const PlyIntProperty* property,
    std::ifstream& stream,
    bool is_binary,
    bool swap_byte)
{
    _store_indices(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::read(
    const PlyUintProperty* property,
    std::ifstream& stream,
    bool is_binary,
    bool swap_byte)
{
    _store_indices(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyReader<FloatType, IndexType, ColorType, VN>::read(
    const PlyUcharProperty* property,
    std::ifstream& stream,
    bool is_binary,
    bool swap_byte)
{
    _store_color(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
inline void CommonPlyReader<FloatType, IndexType, ColorType, VN>::_store_float(
    const TPlyProperty* property,
    std::ifstream& stream,
    bool is_binary,
    bool swap_byte)
{
    // First read out value then decide what to do with it.
    // By this way we can safely skip unrecognised properties.
    FloatType value;
    if (!is_binary) {
        auto v = property->get_ascii(stream);
        value = static_cast<FloatType>(v);
    }
    else {
        auto v = property->get_binary(stream, swap_byte);
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
    bool is_binary,
    bool swap_byte)
{
    ColorType value;
    if (!is_binary) {
        auto v = property->get_ascii(stream);
        value = static_cast<ColorType>(v);
    }
    else {
        auto v = property->get_binary(stream, swap_byte);
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
    bool is_binary,
    bool swap_byte)
{
    if (property->is_list()) {
        std::vector<IndexType> values;
        if (!is_binary) {
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
                auto v = property->get_binary(stream, swap_byte);
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

    return std::move(header);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyDoubleProperty* property,
    std::ofstream& stream,
    bool is_binary,
    bool swap_byte)
{
    _write_float(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyFloatProperty* property,
    std::ofstream& stream,
    bool is_binary,
    bool swap_byte)
{
    _write_float(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyIntProperty* property,
    std::ofstream& stream,
    bool is_binary,
    bool swap_byte)
{
    _write_indices(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyUintProperty* property,
    std::ofstream& stream,
    bool is_binary,
    bool swap_byte)
{
    _write_indices(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::write(
    const PlyUcharProperty* property,
    std::ofstream& stream,
    bool is_binary,
    bool swap_byte)
{
    _write_color(property, stream, is_binary, swap_byte);
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::_write_float(
    const TPlyProperty* property,
    std::ofstream& stream,
    bool is_binary,
    bool swap_byte)
{
    FloatType value;
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

    auto v = static_cast<typename TPlyProperty::value_type>(value);
    if (!is_binary) {
        property->put_ascii(stream, v);
    }
    else {
        property->put_binary(stream, v, swap_byte);
    }
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::_write_color(
    const TPlyProperty* property,
    std::ofstream& stream,
    bool is_binary,
    bool swap_byte)
{
    ColorType value;
    if (property->name() == "red" || property->name() == "green" ||
        property->name() == "blue" || property->name() == "alpha") {
        EASSERT(_colors != nullptr);
        value = (*_colors)[_citer++];
    }
    else {
        // Ignore
    }

    auto v = static_cast<typename TPlyProperty::value_type>(value);
    if (!is_binary) {
        property->put_ascii(stream, v);
    }
    else {
        property->put_binary(stream, v, swap_byte);
    }
}

template<typename FloatType, typename IndexType, typename ColorType, int VN>
template<typename TPlyProperty>
void CommonPlyWriter<FloatType, IndexType, ColorType, VN>::_write_indices(
    const TPlyProperty* property,
    std::ofstream& stream,
    bool is_binary,
    bool swap_byte)
{
    if (property->name() == "vertex_index" ||
        property->name() == "vertex_indices") {
        EASSERT(_indices != nullptr);

        // Write vertex number
        auto vn = static_cast<unsigned char>(VN);
        if (!is_binary) {
            stream << VN << " ";
        }
        else {
            auto vn = static_cast<char>(VN);
            stream.put(vn);
        }

        // Write index
        for (auto i = 0; i < VN; ++i) {
            auto v = (*_indices)[_iiter++];
            if (!is_binary) {
                property->put_ascii(stream, v);
                if (i < VN - 1) {
                    stream << " ";
                }
            }
            else {
                property->put_binary(stream, v, swap_byte);
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

    bool is_binary = false;
    bool swap_byte = false;
    if (header.format() != PlyFormat::ascii) {
        auto pos = stream.tellg();
        stream.close();
        stream.open(file_name, std::ios::binary);
        stream.seekg(pos);
        is_binary = true;
        swap_byte = _impl::_should_swap(header.format());
    }

    for (const auto& elem : header) {
        for (auto i = 0; i < elem.count(); ++i) {
            for (const auto& prop : elem) {
                prop.apply(reader, stream, is_binary, swap_byte);
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

    bool is_binary = false;
    bool swap_byte = false;
    if (format != PlyFormat::ascii) {
        stream.close();
        stream.open(file_name, std::ios::binary | std::ios::app);
        is_binary = true;
        swap_byte = _impl::_should_swap(format);
    }

    for (const auto& elem : header) {
        for (auto i = 0; i < elem.count(); ++i) {
            for (auto j = 0; j < elem.n_props(); ++j) {
                const auto prop = elem.property(j);
                prop->apply(writer, stream, is_binary, swap_byte);
                if (!is_binary && j < elem.n_props() - 1) {
                    stream << " ";
                }
            }
            if (!is_binary) {
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
