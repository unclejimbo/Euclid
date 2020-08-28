#include <cassert>
#include <stdexcept>

namespace Euclid
{

template<typename Kernel>
AABB<Kernel>::AABB(const std::vector<FT>& positions)
{
    if (positions.empty()) {
        throw std::invalid_argument("Input is empty");
    }
    if (positions.size() % 3 != 0) {
        throw std::invalid_argument("Size of input is not divisible by 3");
    }

    auto xmin = positions[0];
    auto xmax = xmin;
    auto ymin = positions[1];
    auto ymax = ymin;
    auto zmin = positions[2];
    auto zmax = zmin;
    for (size_t i = 3; i < positions.size(); i += 3) {
        auto x = positions[i];
        auto y = positions[i + 1];
        auto z = positions[i + 2];
        if (x < xmin)
            xmin = x;
        else if (x > xmax)
            xmax = x;
        if (y < ymin)
            ymin = y;
        else if (y > ymax)
            ymax = y;
        if (z < zmin)
            zmin = z;
        else if (z > zmax)
            zmax = z;
    }
    _build_aabb(xmin, xmax, ymin, ymax, zmin, zmax);
}

template<typename Kernel>
AABB<Kernel>::AABB(const std::vector<Point_3>& points)
{
    if (points.empty()) {
        throw std::invalid_argument("Input is empty");
    }

    auto xmin = points[0].x();
    auto xmax = xmin;
    auto ymin = points[0].y();
    auto ymax = ymin;
    auto zmin = points[0].z();
    auto zmax = zmin;
    for (size_t i = 1; i < points.size(); ++i) {
        auto x = points[i].x();
        auto y = points[i].y();
        auto z = points[i].z();
        if (x < xmin)
            xmin = x;
        else if (x > xmax)
            xmax = x;
        if (y < ymin)
            ymin = y;
        else if (y > ymax)
            ymax = y;
        if (z < zmin)
            zmin = z;
        else if (z > zmax)
            zmax = z;
    }
    _build_aabb(xmin, xmax, ymin, ymax, zmin, zmax);
}

template<typename Kernel>
template<typename ForwardIterator, typename VPMap>
AABB<Kernel>::AABB(ForwardIterator first, ForwardIterator beyond, VPMap vpmap)
{
    if (first == beyond) {
        throw std::invalid_argument("Input is empty");
    }

    auto p0 = get(vpmap, *first);
    auto xmin = p0.x();
    auto xmax = xmin;
    auto ymin = p0.y();
    auto ymax = ymin;
    auto zmin = p0.z();
    auto zmax = zmin;
    while (++first != beyond) {
        auto p = get(vpmap, *first);
        auto x = p.x();
        auto y = p.y();
        auto z = p.z();
        if (x < xmin)
            xmin = x;
        else if (x > xmax)
            xmax = x;
        if (y < ymin)
            ymin = y;
        else if (y > ymax)
            ymax = y;
        if (z < zmin)
            zmin = z;
        else if (z > zmax)
            zmax = z;
    }
    _build_aabb(xmin, xmax, ymin, ymax, zmin, zmax);
}

template<typename Kernel>
typename AABB<Kernel>::Point_3 AABB<Kernel>::center() const
{
    return _center;
}

template<typename Kernel>
typename AABB<Kernel>::FT AABB<Kernel>::xmin() const
{
    return _center.x() - _xlen;
}

template<typename Kernel>
typename AABB<Kernel>::FT AABB<Kernel>::xmax() const
{
    return _center.x() + _xlen;
}

template<typename Kernel>
typename AABB<Kernel>::FT AABB<Kernel>::xlen() const
{
    return _xlen * static_cast<FT>(2);
}

template<typename Kernel>
typename AABB<Kernel>::FT AABB<Kernel>::ymin() const
{
    return _center.y() - _ylen;
}

template<typename Kernel>
typename AABB<Kernel>::FT AABB<Kernel>::ymax() const
{
    return _center.y() + _ylen;
}

template<typename Kernel>
typename AABB<Kernel>::FT AABB<Kernel>::ylen() const
{
    return _ylen * static_cast<FT>(2);
}

template<typename Kernel>
typename AABB<Kernel>::FT AABB<Kernel>::zmin() const
{
    return _center.z() - _zlen;
}

template<typename Kernel>
typename AABB<Kernel>::FT AABB<Kernel>::zmax() const
{
    return _center.z() + _zlen;
}

template<typename Kernel>
typename AABB<Kernel>::FT AABB<Kernel>::zlen() const
{
    return _zlen * static_cast<FT>(2);
}

template<typename Kernel>
typename AABB<Kernel>::Point_3 AABB<Kernel>::point(bool xlarge,
                                                   bool ylarge,
                                                   bool zlarge) const
{
    auto x = _xlen;
    auto y = _ylen;
    auto z = _zlen;
    if (!xlarge)
        x = -_xlen;
    if (!ylarge)
        y = -_ylen;
    if (!zlarge)
        z = -_zlen;
    return Point_3(_center.x() + x, _center.y() + y, _center.z() + z);
}

template<typename Kernel>
void AABB<Kernel>::_build_aabb(FT xmin,
                               FT xmax,
                               FT ymin,
                               FT ymax,
                               FT zmin,
                               FT zmax)
{
    _center =
        Point_3((xmin + xmax) * 0.5, (ymin + ymax) * 0.5, (zmin + zmax) * 0.5);
    _xlen = (xmax - xmin) * static_cast<FT>(0.5); // Half length
    _ylen = (ymax - ymin) * static_cast<FT>(0.5);
    _zlen = (zmax - zmin) * static_cast<FT>(0.5);
}

} // namespace Euclid
