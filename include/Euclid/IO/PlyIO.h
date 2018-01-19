/** Ply I/O.
 *
 *  Ply file are assumed to consist of geomety elements, e.g. vertex and face.
 *  And element has one or more properties, e.g. a vertex has x, y, z positions.
 *  @defgroup PkgPlyIO Ply I/O
 *  @ingroup PkgIO
 */
#pragma once
#include <iterator>
#include <memory>
#include <string>
#include <vector>

namespace Euclid
{
/** @{*/

/** Ply file format.
 *
 *  Ply can be stored in 3 file formats, i.e. ascii text,
 *  little endian and big endian binary formats.
 */
enum class PlyFormat
{
    ascii,
    binary_little_endian,
    binary_big_endian
};

// Forward declaration
class PlyReader;
class PlyWriter;

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
    {
    }

    virtual ~PlyProperty() = default;

    PlyProperty(const PlyProperty&) = default;

    PlyProperty(PlyProperty&&) = default;

    PlyProperty& operator=(const PlyProperty&) = default;

    PlyProperty& operator=(PlyProperty&&) = default;

    /** Return the name of this property.
     *
     */
    const std::string& name() const { return _name; }

    /** Return if this property is a list property.
     *
     */
    bool is_list() const { return _is_list; }

    /** Return the string for this type in the header.
     *
     */
    virtual std::string type_str() const = 0;

    /** Apply a PlyReader visitor on this property.
     *
     */
    virtual void apply(PlyReader& reader,
                       std::ifstream& stream,
                       bool is_binary,
                       bool swap_byte) const = 0;

    /** Apply a PlyWriter visitor on this property.
     *
     */
    virtual void apply(PlyWriter& writer,
                       std::ofstream& stream,
                       bool is_binary,
                       bool swap_byte) const = 0;

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
    {
    }

    std::string type_str() const override { return "double"; }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream, bool swap_byte) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool swap_byte) const;
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
    {
    }

    std::string type_str() const override { return "float"; }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream, bool swap_byte) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool swap_byte) const;
};

/** A int ply property.
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
    {
    }

    std::string type_str() const override { return "int"; }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream, bool swap_byte) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool swap_byte) const;
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
    {
    }

    std::string type_str() const override { return "uint"; }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream, bool swap_byte) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool swap_byte) const;
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
    {
    }

    std::string type_str() const override { return "short"; }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream, bool swap_byte) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool swap_byte) const;
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
    {
    }

    std::string type_str() const override { return "ushort"; }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream, bool swap_byte) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool swap_byte) const;
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
    {
    }

    std::string type_str() const override { return "char"; }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream, bool swap_byte) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool swap_byte) const;
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
    {
    }

    std::string type_str() const override { return "uchar"; }

    void apply(PlyReader& reader,
               std::ifstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    void apply(PlyWriter& writer,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) const override;

    value_type get_ascii(std::ifstream& stream) const;

    value_type get_binary(std::ifstream& stream, bool swap_byte) const;

    void put_ascii(std::ofstream& stream, value_type value) const;

    void put_binary(std::ofstream& stream,
                    value_type value,
                    bool swap_byte) const;
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

        reference operator*() { return **_iter; }

        bool operator!=(const iterator& rhs) { return _iter != rhs._iter; }

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

        reference operator*() { return **_iter; }

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
    {
    }

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
    const std::string& name() const { return _name; }

    /** Return the instance count of this element in file.
     *
     */
    unsigned count() const { return _count; }

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
    PlyProperty* property(size_t i) { return _properties[i].get(); }

    /** Get the property by index.
     *
     */
    const PlyProperty* property(size_t i) const { return _properties[i].get(); }

    /** Return the size of the property list.
     *
     */
    size_t n_props() const { return _properties.size(); }

    /** Return the begin iterator of the property vector.
     *
     */
    iterator begin() { return iterator(_properties.begin()); }

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
    iterator end() { return iterator(_properties.end()); }

    /** Return the end const_iterator of the property vector.
     *
     */
    const_iterator end() const { return const_iterator(_properties.cend()); }

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
    PlyFormat format() const { return _format; }

    /** Set the format.
     *
     */
    void set_format(PlyFormat format) { _format = format; }

    /** Get element by index.
     *
     */
    PlyElement& element(size_t i) { return _elements[i]; }

    /** Get element by index.
     *
     */
    const PlyElement& element(size_t i) const { return _elements[i]; }

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
    size_t n_elems() const { return _elements.size(); }

    /** Return the begin iterator of the elements.
     *
     */
    decltype(auto) begin() { return _elements.cbegin(); }

    /** Return the begin iterator of the elements.
     *
     */
    decltype(auto) begin() const { return _elements.begin(); }

    /** Return the end iterator of the elements.
     *
     */
    decltype(auto) end() { return _elements.end(); }

    /** Return the end iterator of the elements.
     *
     */
    decltype(auto) end() const { return _elements.cend(); }

private:
    PlyFormat _format;
    std::vector<PlyElement> _elements;
};

/** An abstrct class for ply reader.
 *
 *  This class serves as a base class for a specific reader.
 *  You should use the CommonPlyReader class to read the most common
 *  ply format. When you are reading from a custom ply file, derive
 *  from this class and override the functions of the specific PlyProperty.
 */
class PlyReader
{
public:
    virtual ~PlyReader() = default;

    /** On reading the header.
     *
     *  This function is called right after the header specification
     *  is retrieved from the file. Override this function if you
     *  wish to do some preprocessing before reader the ply body,
     *  e.g. allocate space for buffers.
     */
    virtual void on_read(const PlyHeader& header) {}

    /** Read a PlyDoubleProperty.
     *
     */
    virtual void read(const PlyDoubleProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte)
    {
    }

    /** Read a PlyFloatProperty.
     *
     */
    virtual void read(const PlyFloatProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte)
    {
    }

    /** Read a PlyIntProperty.
     *
     */
    virtual void read(const PlyIntProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte)
    {
    }

    /** Read a PlyUintProperty.
     *
     */
    virtual void read(const PlyUintProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte)
    {
    }

    /** Read a PlyShortProperty.
     *
     */
    virtual void read(const PlyShortProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte)
    {
    }

    /** Read a PlyUshortProperty.
     *
     */
    virtual void read(const PlyUshortProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte)
    {
    }

    /** Read a PlyCharProperty.
     *
     */
    virtual void read(const PlyCharProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte)
    {
    }

    /** Read a PlyUcharProperty.
     *
     */
    virtual void read(const PlyUcharProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte)
    {
    }
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
 *  So this class provides a native implementation to read
 *  this type of ply files. Note that it's assumed that all faces
 *  have the same number of vertices.
 */
template<typename FloatType, typename IndexType, typename ColorType, int VN>
class CommonPlyReader : public PlyReader
{
public:
    CommonPlyReader(std::vector<FloatType>& positions,
                    std::vector<IndexType>* indices = nullptr,
                    std::vector<FloatType>* normals = nullptr,
                    std::vector<FloatType>* texcoords = nullptr,
                    std::vector<ColorType>* colors = nullptr)
        : _positions(positions), _indices(indices), _normals(normals),
          _texcoords(texcoords), _colors(colors)
    {
    }

    void on_read(const PlyHeader& header) override;

    void read(const PlyDoubleProperty* property,
              std::ifstream& stream,
              bool is_binary,
              bool swap_byte) override;

    void read(const PlyFloatProperty* property,
              std::ifstream& stream,
              bool is_binary,
              bool swap_byte) override;

    void read(const PlyIntProperty* property,
              std::ifstream& stream,
              bool is_binary,
              bool swap_byte) override;

    void read(const PlyUintProperty* property,
              std::ifstream& stream,
              bool is_binary,
              bool swap_byte) override;

    void read(const PlyUcharProperty* property,
              std::ifstream& stream,
              bool is_binary,
              bool swap_byte) override;

private:
    template<typename TPlyProperty>
    void _store_float(const TPlyProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte);

    template<typename TPlyProperty>
    void _store_color(const TPlyProperty* property,
                      std::ifstream& stream,
                      bool is_binary,
                      bool swap_byte);

    template<typename TPlyProperty>
    void _store_indices(const TPlyProperty* property,
                        std::ifstream& stream,
                        bool is_binary,
                        bool swap_byte);

private:
    std::vector<FloatType>& _positions;
    std::vector<IndexType>* _indices = nullptr;
    std::vector<FloatType>* _normals = nullptr;
    std::vector<FloatType>* _texcoords = nullptr;
    std::vector<ColorType>* _colors = nullptr;
};

/** An abstract ply writer.
 *
 */
class PlyWriter
{
public:
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
    virtual void write(const PlyDoubleProperty* property,
                       std::ofstream& stream,
                       bool is_binary,
                       bool swap_byte)
    {
    }

    /** Write a PlyFloatProperty.
     *
     */
    virtual void write(const PlyFloatProperty* property,
                       std::ofstream& stream,
                       bool is_binary,
                       bool swap_byte)
    {
    }

    /** Write a PlyIntProperty.
     *
     */
    virtual void write(const PlyIntProperty* property,
                       std::ofstream& stream,
                       bool is_binary,
                       bool swap_byte)
    {
    }

    /** Write a PlyUintProperty.
     *
     */
    virtual void write(const PlyUintProperty* property,
                       std::ofstream& stream,
                       bool is_binary,
                       bool swap_byte)
    {
    }

    /** Write a PlyShortProperty.
     *
     */
    virtual void write(const PlyShortProperty* property,
                       std::ofstream& stream,
                       bool is_binary,
                       bool swap_byte)
    {
    }

    /** Write a PlyUshortProperty.
     *
     */
    virtual void write(const PlyUshortProperty* property,
                       std::ofstream& stream,
                       bool is_binary,
                       bool swap_byte)
    {
    }

    /** Write a PlyCharProperty.
     *
     */
    virtual void write(const PlyCharProperty* property,
                       std::ofstream& stream,
                       bool is_binary,
                       bool swap_byte)
    {
    }

    /** Write a PlyUcharProperty.
     *
     */
    virtual void write(const PlyUcharProperty* property,
                       std::ofstream& stream,
                       bool is_binary,
                       bool swap_byte)
    {
    }
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
 *  So this class provides a native implementation to write
 *  this type of ply files. Note that it's assumed that all faces
 *  have the same number of vertices.
 */
template<typename FloatType, typename IndexType, typename ColorType, int VN>
class CommonPlyWriter : public PlyWriter
{
public:
    CommonPlyWriter(std::vector<FloatType>& positions,
                    std::vector<IndexType>* indices = nullptr,
                    std::vector<FloatType>* normals = nullptr,
                    std::vector<FloatType>* texcoords = nullptr,
                    std::vector<ColorType>* colors = nullptr)
        : _positions(positions), _indices(indices), _normals(normals),
          _texcoords(texcoords), _colors(colors)
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
               bool is_binary,
               bool swap_byte) override;

    void write(const PlyFloatProperty* property,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) override;

    void write(const PlyIntProperty* property,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) override;

    void write(const PlyUintProperty* property,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) override;

    void write(const PlyUcharProperty* property,
               std::ofstream& stream,
               bool is_binary,
               bool swap_byte) override;

private:
    template<typename TPlyProperty>
    void _write_float(const TPlyProperty* property,
                      std::ofstream& stream,
                      bool is_binary,
                      bool swap_byte);

    template<typename TPlyProperty>
    void _write_color(const TPlyProperty* property,
                      std::ofstream& stream,
                      bool is_binary,
                      bool swap_byte);

    template<typename TPlyProperty>
    void _write_indices(const TPlyProperty* property,
                        std::ofstream& stream,
                        bool is_binary,
                        bool swap_byte);

private:
    std::vector<FloatType>& _positions;
    std::vector<IndexType>* _indices = nullptr;
    std::vector<FloatType>* _normals = nullptr;
    std::vector<FloatType>* _texcoords = nullptr;
    std::vector<ColorType>* _colors = nullptr;
    bool _has_alpha = false;
    size_t _piter = 0;
    size_t _iiter = 0;
    size_t _niter = 0;
    size_t _titer = 0;
    size_t _citer = 0;
};

/** Read ply header.
 *
 */
PlyHeader read_ply_header(const std::string& file_name);

/** Read ply file.
 *
 */
void read_ply(const std::string& file_name, PlyReader& reader);

/** Read ply file using CommonPlyReader.
 *
 */
template<typename FloatType, typename IndexType, typename ColorType, int VN>
void read_ply(const std::string& file_name,
              std::vector<FloatType>& vertices,
              std::vector<IndexType>* indices = nullptr,
              std::vector<FloatType>* normals = nullptr,
              std::vector<FloatType>* texcoords = nullptr,
              std::vector<ColorType>* colors = nullptr);

/** Write ply file.
 *
 */
void write_ply(const std::string& file_name,
               PlyWriter& writer,
               PlyFormat format = PlyFormat::ascii);

/** Write ply file using CommonPlyWriter.
 *
 */
template<typename FloatType, typename IndexType, typename ColorType, int VN>
void write_ply(const std::string& file_name,
               std::vector<FloatType>& vertices,
               std::vector<IndexType>* indices = nullptr,
               std::vector<FloatType>* normals = nullptr,
               std::vector<FloatType>* texcoords = nullptr,
               std::vector<ColorType>* colors = nullptr,
               PlyFormat format = PlyFormat::ascii);

/** @}*/
} // namespace Euclid

#include "src/PlyIO.cpp"
