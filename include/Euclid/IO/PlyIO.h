/** Ply I/O.
 *
 *  Ply file conceptually consists of geomety elements, e.g. vertex and face.
 *  And each element has one or more properties, e.g. a vertex has x, y, z
 *  coordinates. As a result, ply is quite versatile and you can store many
 *  properties using this format. For the sake of simplicity however, you should
 *  consider using the free read and write functions provided in this module
 *  since they can deal with the most common type of ply file used in geometry
 *  processing. When you need to customize the ply format, you can try to extend
 *  the abstract interface PlyReader and PlyWriter.
 *  @defgroup PkgPlyIO Ply I/O
 *  @ingroup PkgIO
 */
#pragma once

#include <iterator>
#include <memory>
#include <string>
#include <type_traits>
#include <vector>

namespace Euclid
{
// Forward declaration
class PlyReader;
class PlyWriter;

/** @{*/

/** Ply file format.
 *
 *  Ply can be stored in 3 file formats, i.e. ascii text,
 *  little endian and big endian binary formats.
 */
enum class PlyFormat
{
    ascii,                /**< text mode.*/
    binary_little_endian, /**< little endian binary mode.*/
    binary_big_endian     /**< big endian binary mode.*/
};

/** Base class for ply property.
 *
 *  Property represents a value, or a list of values in a ply file.
 *  Every element in ply has one or more properties.
 *  Each property should  have a name and a type.
 *  Note that the type of a property acts more like an indicator
 *  for its bit width rather than a type specifier, for example
 *  unsigned char values represent numbers ranging from 0 to 255,
 *  rather than a single character as comprehended in c++.
 *  In practice, you should use one of the derived typed-classes
 *  and they are the only supported types by ply specification.
 */
class PlyProperty
{
public:
    explicit PlyProperty(const std::string& name) : _name(name) {}

    PlyProperty(const std::string& name, bool is_list)
        : _name(name), _is_list(is_list)
    {}

    virtual ~PlyProperty() = default;

    PlyProperty(const PlyProperty&) = default;

    PlyProperty(PlyProperty&&) = default;

    PlyProperty& operator=(const PlyProperty&) = default;

    PlyProperty& operator=(PlyProperty&&) = default;

    /** Return the name of this property.
     *
     */
    const std::string& name() const
    {
        return _name;
    }

    /** Return if this property is a list property.
     *
     */
    bool is_list() const
    {
        return _is_list;
    }

    /** Return the string for this type in the header.
     *
     */
    virtual std::string type_str() const = 0;

    /** Apply a PlyReader visitor on this property.
     *
     */
    virtual void apply(PlyReader& reader,
                       std::ifstream& stream,
                       PlyFormat format) const = 0;

    /** Apply a PlyWriter visitor on this property.
     *
     */
    virtual void apply(PlyWriter& writer,
                       std::ofstream& stream,
                       PlyFormat format) const = 0;

private:
    std::string _name;
    bool _is_list = false;
};

/** A double ply property.
 *
 */
class PlyDoubleProperty : public PlyProperty
{
public:
    using value_type = double;

public:
    explicit PlyDoubleProperty(const std::string& name) : PlyProperty(name) {}

    PlyDoubleProperty(const std::string& name, bool is_list)
        : PlyProperty(name, is_list)
    {}

    std::string type_str() const override
    {
        return "double";
    }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               PlyFormat format) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               PlyFormat format) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream,
                          bool file_little_endian,
                          bool sys_little_endian) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool file_little_endian,
                    bool sys_little_endian) const;
};

/** A float ply property.
 *
 */
class PlyFloatProperty : public PlyProperty
{
public:
    using value_type = float;

public:
    explicit PlyFloatProperty(const std::string& name) : PlyProperty(name) {}

    PlyFloatProperty(const std::string& name, bool is_list)
        : PlyProperty(name, is_list)
    {}

    std::string type_str() const override
    {
        return "float";
    }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               PlyFormat format) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               PlyFormat format) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream,
                          bool file_little_endian,
                          bool sys_little_endian) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool file_little_endian,
                    bool sys_little_endian) const;
};

/** An int ply property.
 *
 */
class PlyIntProperty : public PlyProperty
{
public:
    using value_type = int;

public:
    explicit PlyIntProperty(const std::string& name) : PlyProperty(name) {}

    PlyIntProperty(const std::string& name, bool is_list)
        : PlyProperty(name, is_list)
    {}

    std::string type_str() const override
    {
        return "int";
    }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               PlyFormat format) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               PlyFormat format) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream,
                          bool file_little_endian,
                          bool sys_little_endian) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool file_little_endian,
                    bool sys_little_endian) const;
};

/** An unsigned int ply property.
 *
 */
class PlyUintProperty : public PlyProperty
{
public:
    using value_type = unsigned;

public:
    explicit PlyUintProperty(const std::string& name) : PlyProperty(name) {}

    PlyUintProperty(const std::string& name, bool is_list)
        : PlyProperty(name, is_list)
    {}

    std::string type_str() const override
    {
        return "uint";
    }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               PlyFormat format) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               PlyFormat format) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream,
                          bool file_little_endian,
                          bool sys_little_endian) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool file_little_endian,
                    bool sys_little_endian) const;
};

/** A short ply property.
 *
 */
class PlyShortProperty : public PlyProperty
{
public:
    using value_type = int;

public:
    explicit PlyShortProperty(const std::string& name) : PlyProperty(name) {}

    PlyShortProperty(const std::string& name, bool is_list)
        : PlyProperty(name, is_list)
    {}

    std::string type_str() const override
    {
        return "short";
    }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               PlyFormat format) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               PlyFormat format) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream,
                          bool file_little_endian,
                          bool sys_little_endian) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool file_little_endian,
                    bool sys_little_endian) const;
};

/** An unsigned short ply property.
 *
 */
class PlyUshortProperty : public PlyProperty
{
public:
    using value_type = unsigned;

public:
    explicit PlyUshortProperty(const std::string& name) : PlyProperty(name) {}

    PlyUshortProperty(const std::string& name, bool is_list)
        : PlyProperty(name, is_list)
    {}

    std::string type_str() const override
    {
        return "ushort";
    }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               PlyFormat format) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               PlyFormat format) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream,
                          bool file_little_endian,
                          bool sys_little_endian) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool file_little_endian,
                    bool sys_little_endian) const;
};

/** A char ply property.
 *
 */
class PlyCharProperty : public PlyProperty
{
public:
    using value_type = int;

public:
    explicit PlyCharProperty(const std::string& name) : PlyProperty(name) {}

    PlyCharProperty(const std::string& name, bool is_list)
        : PlyProperty(name, is_list)
    {}

    std::string type_str() const override
    {
        return "char";
    }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               PlyFormat format) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               PlyFormat format) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream,
                          bool file_little_endian,
                          bool sys_little_endian) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool file_little_endian,
                    bool sys_little_endian) const;
};

/** An unsigned char ply property.
 *
 */
class PlyUcharProperty : public PlyProperty
{
public:
    using value_type = unsigned;

public:
    explicit PlyUcharProperty(const std::string& name) : PlyProperty(name) {}

    PlyUcharProperty(const std::string& name, bool is_list)
        : PlyProperty(name, is_list)
    {}

    std::string type_str() const override
    {
        return "uchar";
    }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               PlyFormat format) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               PlyFormat format) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream,
                          bool file_little_endian,
                          bool sys_little_endian) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool file_little_endian,
                    bool sys_little_endian) const;
};

/** Ply element.
 *
 *  An element represents a basic geometric primitive in a ply file.
 *  An element has a name, the number of instances, and a list of
 *  properties describing its values.
 */
class PlyElement
{
public:
    /** Iterate through the properties in this element.
     *
     */
    class iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = PlyProperty;
        using reference = value_type&;
        using pointer = value_type*;
        using difference_type = int;
        using unique_iter = std::vector<std::unique_ptr<PlyProperty>>::iterator;

        explicit iterator(unique_iter iter) : _iter(iter) {}

        iterator& operator++()
        {
            ++_iter;
            return *this;
        }

        iterator operator++(int)
        {
            auto cpy = *this;
            ++(*this);
            return cpy;
        }

        reference operator*()
        {
            return **_iter;
        }

        bool operator!=(const iterator& rhs)
        {
            return _iter != rhs._iter;
        }

    private:
        unique_iter _iter;
    };

    /** Iterate through the properties in this element.
     *
     */
    class const_iterator
    {
    public:
        using iterator_category = std::forward_iterator_tag;
        using value_type = const PlyProperty;
        using reference = value_type&;
        using pointer = value_type*;
        using difference_type = int;
        using unique_iter =
            std::vector<std::unique_ptr<PlyProperty>>::const_iterator;

        explicit const_iterator(unique_iter iter) : _iter(iter) {}

        const_iterator& operator++()
        {
            ++_iter;
            return *this;
        }

        const_iterator operator++(int)
        {
            auto cpy = *this;
            ++(*this);
            return cpy;
        }

        reference operator*()
        {
            return **_iter;
        }

        bool operator!=(const const_iterator& rhs)
        {
            return _iter != rhs._iter;
        }

    private:
        unique_iter _iter;
    };

    /** Create a PlyElement with name and instance count.
     *
     */
    PlyElement(const std::string& name, unsigned count)
        : _name(name), _count(count)
    {}

    ~PlyElement() = default;

    PlyElement(const PlyElement& rhs) = delete;

    PlyElement& operator=(const PlyElement& rhs) = delete;

    PlyElement(PlyElement&& rhs)
    {
        _name = rhs._name;
        _count = rhs._count;
        std::copy(std::make_move_iterator(rhs._properties.begin()),
                  std::make_move_iterator(rhs._properties.end()),
                  std::back_inserter(_properties));
    }

    PlyElement& operator=(PlyElement&& rhs)
    {
        _name = rhs._name;
        _count = rhs._count;
        std::copy(std::make_move_iterator(rhs._properties.begin()),
                  std::make_move_iterator(rhs._properties.end()),
                  std::back_inserter(_properties));
        return *this;
    }

    /** Return the name of the element.
     *
     */
    const std::string& name() const
    {
        return _name;
    }

    /** Return the instance count of this element in file.
     *
     */
    unsigned count() const
    {
        return _count;
    }

    /** Add a property to this element.
     *
     */
    void add_property(std::unique_ptr<PlyProperty> property)
    {
        _properties.push_back(std::move(property));
    }

    /** Get the property by index.
     *
     */
    PlyProperty* property(size_t i)
    {
        return _properties[i].get();
    }

    /** Get the property by index.
     *
     */
    const PlyProperty* property(size_t i) const
    {
        return _properties[i].get();
    }

    /** Return the size of the property list.
     *
     */
    size_t n_props() const
    {
        return _properties.size();
    }

    /** Return the begin iterator of the property vector.
     *
     */
    iterator begin()
    {
        return iterator(_properties.begin());
    }

    /** Return the begin const_iterator of the property vector.
     *
     */
    const_iterator begin() const
    {
        return const_iterator(_properties.cbegin());
    }

    /** Return the end iterator of the property vector.
     *
     */
    iterator end()
    {
        return iterator(_properties.end());
    }

    /** Return the end const_iterator of the property vector.
     *
     */
    const_iterator end() const
    {
        return const_iterator(_properties.cend());
    }

private:
    std::string _name;
    unsigned _count;
    std::vector<std::unique_ptr<PlyProperty>> _properties;
};

/** Ply header.
 *
 *  Hold the PlyFormat and a list of PlyElements.
 */
class PlyHeader
{
public:
    /** Create a PlyHeader with format.
     *
     */
    explicit PlyHeader(PlyFormat format) : _format(format) {}

    PlyHeader(const PlyHeader&) = delete;

    PlyHeader(PlyHeader&&) = default;

    PlyHeader& operator=(const PlyHeader&) = delete;

    PlyHeader& operator=(PlyHeader&&) = default;

    /** Return the format.
     *
     */
    PlyFormat format() const
    {
        return _format;
    }

    /** Set the format.
     *
     */
    void set_format(PlyFormat format)
    {
        _format = format;
    }

    /** Get element by index.
     *
     */
    PlyElement& element(size_t i)
    {
        return _elements[i];
    }

    /** Get element by index.
     *
     */
    const PlyElement& element(size_t i) const
    {
        return _elements[i];
    }

    /** Add an element to this header.
     *
     */
    void add_element(PlyElement&& element)
    {
        _elements.push_back(std::move(element));
    }

    /** Return the number of elements in the header specification.
     *
     */
    size_t n_elems() const
    {
        return _elements.size();
    }

    /** Return the begin iterator of the elements.
     *
     */
    decltype(auto) begin()
    {
        return _elements.cbegin();
    }

    /** Return the begin iterator of the elements.
     *
     */
    decltype(auto) begin() const
    {
        return _elements.begin();
    }

    /** Return the end iterator of the elements.
     *
     */
    decltype(auto) end()
    {
        return _elements.end();
    }

    /** Return the end iterator of the elements.
     *
     */
    decltype(auto) end() const
    {
        return _elements.cend();
    }

private:
    PlyFormat _format;
    std::vector<PlyElement> _elements;
};

/** An abstrct class for ply reader.
 *
 *  This class serves as an abstract base class for a ply file reader. A ply
 *  reader is supposed to be supplied to the function read_ply to customize the
 *  process of reading a ply file. A PlyReader is used in the following way,
 *
 *  1. Header is read and parsed into elements and properties in read_ply.
 *  2. read_ply will call PlyReader::on_read.
 *  3. read_ply will then call PlyReader::read to do the actual reading.
 *
 *  **Note**
 *
 *  You should use the CommonPlyReader class to read the most common ply format.
 *
 *  @sa read_ply CommonPlyReader
 */
class PlyReader
{
public:
    PlyReader();

    virtual ~PlyReader() = default;

    /** On reading the header.
     *
     *  This function is called right after the header specification is
     *  retrieved from the file. Override this function if you wish to do some
     *  preprocessing before reading the ply body, e.g. allocating space for
     *  buffers.
     */
    virtual void on_read(const PlyHeader&) {}

    /** Read a PlyDoubleProperty.
     *
     */
    virtual void read(const PlyDoubleProperty*, std::ifstream&, PlyFormat) {}

    /** Read a PlyFloatProperty.
     *
     */
    virtual void read(const PlyFloatProperty*, std::ifstream&, PlyFormat) {}

    /** Read a PlyIntProperty.
     *
     */
    virtual void read(const PlyIntProperty*, std::ifstream&, PlyFormat) {}

    /** Read a PlyUintProperty.
     *
     */
    virtual void read(const PlyUintProperty*, std::ifstream&, PlyFormat) {}

    /** Read a PlyShortProperty.
     *
     */
    virtual void read(const PlyShortProperty*, std::ifstream&, PlyFormat) {}

    /** Read a PlyUshortProperty.
     *
     */
    virtual void read(const PlyUshortProperty*, std::ifstream&, PlyFormat) {}

    /** Read a PlyCharProperty.
     *
     */
    virtual void read(const PlyCharProperty*, std::ifstream&, PlyFormat) {}

    /** Read a PlyUcharProperty.
     *
     */
    virtual void read(const PlyUcharProperty*, std::ifstream&, PlyFormat) {}

protected:
    bool _sys_little_endian;
};

/** An abstract ply writer.
 *
 *  This class serves as an abstract base class for a ply file writer. A ply
 *  writer is supposed to be supplied to the function write_ply to customize the
 *  process of writing a ply file. A PlyWriter is used in the following way,
 *
 *  1. write_ply will call PlyWriter::on_write.
 *  2. Header will be write to the file.
 *  3. write_ply will then call PlyWriter::write to do the actual writing.
 *
 *  **Note**
 *
 *  You should use the CommonPlyWriter class to write the most common ply
 *  format.
 *
 *  @sa write_ply CommonPlyWriter
 */
class PlyWriter
{
public:
    PlyWriter();

    virtual ~PlyWriter() = default;

    /** On writing the ply file.
     *
     *  This function is called right before writing the ply file.
     *  Override this function if you wish to do some preprocessing,
     *  e.g. resetting the inner iterator.
     */
    virtual void on_write() {}

    /** Generate the ply header.
     *
     *  The PlyWriter is in charge of generating the header
     *  based on the data given.
     */
    virtual PlyHeader generate_header(PlyFormat format) const
    {
        PlyHeader header(format);
        return header;
    };

    /** Write a PlyDoubleProperty.
     *
     */
    virtual void write(const PlyDoubleProperty*, std::ofstream&, PlyFormat) {}

    /** Write a PlyFloatProperty.
     *
     */
    virtual void write(const PlyFloatProperty*, std::ofstream&, PlyFormat) {}

    /** Write a PlyIntProperty.
     *
     */
    virtual void write(const PlyIntProperty*, std::ofstream&, PlyFormat) {}

    /** Write a PlyUintProperty.
     *
     */
    virtual void write(const PlyUintProperty*, std::ofstream&, PlyFormat) {}

    /** Write a PlyShortProperty.
     *
     */
    virtual void write(const PlyShortProperty*, std::ofstream&, PlyFormat) {}

    /** Write a PlyUshortProperty.
     *
     */
    virtual void write(const PlyUshortProperty*, std::ofstream&, PlyFormat) {}

    /** Write a PlyCharProperty.
     *
     */
    virtual void write(const PlyCharProperty*, std::ofstream&, PlyFormat) {}

    /** Write a PlyUcharProperty.
     *
     */
    virtual void write(const PlyUcharProperty*, std::ofstream&, PlyFormat) {}

protected:
    bool _sys_little_endian;
};

/** A ply reader for a common set of properties.
 *
 *  The most common ply files have the following header specification,
 *  ```
 *  element vertex
 *  property float/double x            // vertex position
 *  property float/double y
 *  property float/double z
 *  property float/double nx           // vertex normal, optional
 *  property float/double ny
 *  property float/double nz
 *  property float/double s/texture_u  // texture coordinate, optional
 *  property float/double y/texture_v
 *  property uchar red                 // vertex color, optional
 *  property uchar green
 *  property uchar blue
 *  property uchar alpha               // alpha is optional for color
 *  element face
 *  property list uchar int/uint vertex_index/vertex_indices // optional
 *  ```
 *  So this class provides a native implementation to read this type of ply
 *  files. Note that it's assumed that all faces have the same number of
 *  vertices.
 */
template<int VN, typename FloatType, typename IndexType, typename ColorType>
class CommonPlyReader : public PlyReader
{
public:
    /** Constructor.
     *
     *  The values of the properties are stored into the buffers supplied here.
     *  Set to nullptr to omit the values of that property.
     */
    CommonPlyReader(std::vector<FloatType>& positions,
                    std::vector<FloatType>* normals = nullptr,
                    std::vector<FloatType>* texcoords = nullptr,
                    std::vector<IndexType>* indices = nullptr,
                    std::vector<ColorType>* colors = nullptr)
        : _positions(positions), _normals(normals), _texcoords(texcoords),
          _indices(indices), _colors(colors)
    {}

    void on_read(const PlyHeader& header) override;

    void read(const PlyDoubleProperty* property,
              std::ifstream& stream,
              PlyFormat format) override;

    void read(const PlyFloatProperty* property,
              std::ifstream& stream,
              PlyFormat format) override;

    void read(const PlyIntProperty* property,
              std::ifstream& stream,
              PlyFormat format) override;

    void read(const PlyUintProperty* property,
              std::ifstream& stream,
              PlyFormat format) override;

    void read(const PlyUcharProperty* property,
              std::ifstream& stream,
              PlyFormat format) override;

private:
    template<typename TPlyProperty>
    void _store_float(const TPlyProperty* property,
                      std::ifstream& stream,
                      PlyFormat format);

    template<typename TPlyProperty>
    void _store_color(const TPlyProperty* property,
                      std::ifstream& stream,
                      PlyFormat format);

    template<typename TPlyProperty>
    void _store_indices(const TPlyProperty* property,
                        std::ifstream& stream,
                        PlyFormat format);

private:
    std::vector<FloatType>& _positions;
    std::vector<FloatType>* _normals = nullptr;
    std::vector<FloatType>* _texcoords = nullptr;
    std::vector<IndexType>* _indices = nullptr;
    std::vector<ColorType>* _colors = nullptr;
};

/** A ply writer for a common set of properties.
 *
 *  The most common ply files have the following header specification,
 *  ```
 *  element vertex
 *  property float/double x            // vertex position
 *  property float/double y
 *  property float/double z
 *  property float/double nx           // vertex normal, optional
 *  property float/double ny
 *  property float/double nz
 *  property float/double s/texture_u  // texture coordinate, optional
 *  property float/double y/texture_v
 *  property uchar red                 // vertex color, optional
 *  property uchar green
 *  property uchar blue
 *  property uchar alpha               // alpha is optional for color
 *  element face
 *  property list uchar int/uint vertex_index/vertex_indices // optional
 *  ```
 *  So this class provides a native implementation to write this type of ply
 *  files. Note that it's assumed that all faces have the same number of
 *  vertices.
 */
template<int VN, typename FloatType, typename IndexType, typename ColorType>
class CommonPlyWriter : public PlyWriter
{
public:
    /** Constructor.
     *
     *  The values of the properties are read from the buffers supplied here.
     *  Set to nullptr to omit the values of that property.
     */
    CommonPlyWriter(const std::vector<FloatType>& positions,
                    const std::vector<FloatType>* normals = nullptr,
                    const std::vector<FloatType>* texcoords = nullptr,
                    const std::vector<IndexType>* indices = nullptr,
                    const std::vector<ColorType>* colors = nullptr)
        : _positions(positions), _normals(normals), _texcoords(texcoords),
          _indices(indices), _colors(colors)
    {
        auto n = positions.size() / 3;
        if (_normals != nullptr && _normals->size() != n * 3) {
            throw std::invalid_argument(
                "Size of normals not compatible with positions");
        }
        if (_texcoords != nullptr && _texcoords->size() != n * 2) {
            throw std::invalid_argument(
                "Size of texcoords not compatible with positions");
        }
        if (_colors != nullptr) {
            if (_colors->size() == n * 3) {
                _has_alpha = false;
            }
            else if (_colors->size() == n * 4) {
                _has_alpha = true;
            }
            else {
                throw std::invalid_argument(
                    "Size of colors not compatible with positions");
            }
        }
        if (_indices != nullptr && _indices->size() % VN != 0) {
            throw std::invalid_argument(
                "Size of indices not compatible with VN");
        }
    }

    void on_write() override;

    PlyHeader generate_header(PlyFormat format) const override;

    void write(const PlyDoubleProperty* property,
               std::ofstream& stream,
               PlyFormat format) override;

    void write(const PlyFloatProperty* property,
               std::ofstream& stream,
               PlyFormat format) override;

    void write(const PlyIntProperty* property,
               std::ofstream& stream,
               PlyFormat format) override;

    void write(const PlyUintProperty* property,
               std::ofstream& stream,
               PlyFormat format) override;

    void write(const PlyUcharProperty* property,
               std::ofstream& stream,
               PlyFormat format) override;

private:
    template<typename TPlyProperty>
    void _write_float(const TPlyProperty* property,
                      std::ofstream& stream,
                      PlyFormat format);

    template<typename TPlyProperty>
    void _write_color(const TPlyProperty* property,
                      std::ofstream& stream,
                      PlyFormat format);

    template<typename TPlyProperty>
    void _write_indices(const TPlyProperty* property,
                        std::ofstream& stream,
                        PlyFormat format);

private:
    const std::vector<FloatType>& _positions;
    const std::vector<FloatType>* _normals = nullptr;
    const std::vector<FloatType>* _texcoords = nullptr;
    const std::vector<IndexType>* _indices = nullptr;
    const std::vector<ColorType>* _colors = nullptr;
    bool _has_alpha = false;
    size_t _piter = 0;
    size_t _iiter = 0;
    size_t _niter = 0;
    size_t _titer = 0;
    size_t _citer = 0;
};

/** Read ply header.
 *
 *  @sa PlyHeader
 */
PlyHeader read_ply_header(const std::string& filename);

/** Read ply file using a custom PlyReader.
 *
 *  Read a custom ply file format by providing a PlyReader. If you are reading a
 *  common ply file, consider using other overloadings specifically designed to
 *  read common ply properties like positions, normals, texcoords, indices and
 *  colors.
 */
void read_ply(const std::string& filename, PlyReader& reader);

/** Read ply file using CommonPlyReader.
 *
 *  A convenient function to read the most common ply properties like below,
 *  ```
 *  element vertex
 *  property float/double x            // vertex position
 *  property float/double y
 *  property float/double z
 *  property float/double nx           // vertex normal, optional
 *  property float/double ny
 *  property float/double nz
 *  property float/double s/texture_u  // texture coordinate, optional
 *  property float/double y/texture_v
 *  property uchar red                 // vertex color, optional
 *  property uchar green
 *  property uchar blue
 *  property uchar alpha               // alpha is optional for color
 *  element face
 *  property list uchar int/uint vertex_index/vertex_indices // optional
 *  ```
 *
 *  @tparam VN Number of vertices per face.
 *  @tparam FloatType Type of floating point value.
 *  @tparam IndexType Type of index value.
 *  @tparam ColorType Type of color value.
 *  @tparam Interface User should never specify the type for it.
 *
 *  @param filename Input file name.
 *  @param positions Vertex positions.
 *  @param normals Vertex normals, use nullptr if you don't want to read this
 *  property.
 *  @param texcoords Vertex texture coordinates, use nullptr if you don't want
 *  to read this property.
 *  @param indices Indices, use nullptr if you don't want to read this property.
 *  @param colors Vertex colors, use nullptr if you don't want to read this
 *  property.
 *
 *  **Note**
 *
 *  Proper overloading functions are implemented so that it's not necessary to
 *  provide template parameters even if nullptr is used.
 *
 *  @sa CommonPlyReader
 */
template<int VN,
         typename FloatType,
         typename IndexType,
         typename ColorType,
         typename Interface>
void read_ply(const std::string& filename,
              std::vector<FloatType>& positions,
              std::vector<FloatType>* normals,
              std::vector<FloatType>* texcoords,
              std::vector<IndexType>* indices,
              std::vector<ColorType>* colors);

/** Write ply file.
 *
 *  Write a custom ply file format by providing a PlyWriter. If you are writing
 *  a common ply file, consider using other overloadings specifically designed
 *  to write common ply properties like positions, normals, texcoords, indices
 *  and colors.
 */
void write_ply(const std::string& filename,
               PlyWriter& writer,
               PlyFormat format = PlyFormat::ascii);

/** Write ply file using CommonPlyWriter.
 *
 *  A convenient function to write the most common ply properties like below,
 *  ```
 *  element vertex
 *  property float/double x            // vertex position
 *  property float/double y
 *  property float/double z
 *  property float/double nx           // vertex normal, optional
 *  property float/double ny
 *  property float/double nz
 *  property float/double s/texture_u  // texture coordinate, optional
 *  property float/double y/texture_v
 *  property uchar red                 // vertex color, optional
 *  property uchar green
 *  property uchar blue
 *  property uchar alpha               // alpha is optional for color
 *  element face
 *  property list uchar int/uint vertex_index/vertex_indices // optional
 *  ```
 *
 *  @tparam VN Number of vertices per face.
 *  @tparam FloatType Type of floating point value.
 *  @tparam IndexType Type of index value.
 *  @tparam ColorType Type of color value.
 *  @tparam Interface User should never specify the type for it.
 *
 *  @param filename Output file name.
 *  @param positions Vertex positions.
 *  @param normals Vertex normals, use nullptr if you don't want to write this
 *  property.
 *  @param texcoords Vertex texture coordinates, use nullptr if you don't want
 *  to write this property.
 *  @param indices Indices, use nullptr if you don't want to write this
 *  property.
 *  @param colors Vertex colors, use nullptr if you don't want to write this
 *  property.
 *  @param format Ply encoding format.
 *
 *  **Note**
 *
 *  Proper overloading functions are implemented so that it's not necessary to
 *  provide template parameters even if nullptr is used.
 *
 *  @sa CommonPlyWriter
 */
template<int VN,
         typename FloatType,
         typename IndexType,
         typename ColorType,
         typename Interface>
void write_ply(const std::string& filename,
               const std::vector<FloatType>& positions,
               const std::vector<FloatType>* normals,
               const std::vector<FloatType>* texcoords,
               const std::vector<IndexType>* indices,
               const std::vector<ColorType>* colors,
               PlyFormat format = PlyFormat::ascii);

/** @}*/
} // namespace Euclid

#include "src/PlyIO.cpp"
