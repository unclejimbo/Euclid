# Welcome to Euclid

- [GitHub repo](https://github.com/unclejimbo/Euclid)
- [Latest documentation](https://codedocs.xyz/unclejimbo/Euclid/index.html)

## Introduction

Euclid is a header only library for geometry processing and shape analysis.

It contains some utilities and algorithms which extend and cooperate with other popular libraries around there, like Eigen(libigl), CGAL, to name a few.

The purpose of Euclid is not to replace any of the above packages, but to glue things together as well as to provide more algorithms which do not appear in those libraries.

## Modules Overview

### Math

- Common operations in linear algebra, matrix analysis, etc.
- Robust floating point comparisons.
- Affine transformation.
- Distance computation between probability distributions.

### IO

- Mesh I/O, currently supporting .ply, .obj and .off file format.
- Mesh fixers, fixing degeneracies in input meshes.

### Geometry

- Convert raw mesh arrays read from the IO package into mesh data structures in CGAL and libigl and vice versa.
- Generate common mesh primitives.
- Discrete differential and geometric properties.
- Geodesic distance.

### Analysis

- Shape bounding boxes.
- Geometric shape segmentation.
- Shape descriptors.
- View selection.

### Render

- Fast cpu ray tracing.

### Util

- A versatile timer.
- Memory management.

## Dependencies

Some simple third-party libraries has already been shipped with Euclid.
However, you'll need the following libraries when you use headers in Euclid that work with them, including

- [Boost](https://www.boost.org/).
- [CGAL](https://www.cgal.org/index.html) for tons of data structures and algorithms.
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page) for matrix manipulation as well as solving linear systems.
- [Spectra](https://spectralib.org/) for solving large scale eigen value problems.
- [Libigl](http://libigl.github.io/libigl/) yet another simple but powerful geometry processing library in C++.
- [Embree](http://embree.github.io) for fast cpu ray tracing.
- [Doxygen](http://www.stack.nl/~dimitri/doxygen/) for generating documentations.
- BLAS, LAPACK, and OpenMP for better performance.

Different packages in Euclid may require a different set of dependencies, and some packages may not require any of the above libraries. Also, Euclid uses features in the C++17 standard, so you'll need a C++17 enabled compiler.

## Installation

Since it's a header only library, the simpliest way to use Euclid is to include the needed headers. Although be sure to configure other dependencies properly, as some of them are not header only.

If you are using CMake, there are two ways to go:

First you could use the [find script](https://github.com/unclejimbo/Euclid/blob/dev/cmake/Modules/FindEuclid.cmake) shipped with Euclid and configure other dependencies manually like below. This is preferable if you only need parts of the headers and do not wish to configure all the dependencies.

```cmake
find_package(Euclid)
target_include_directories(your-target Euclid_INCLUDE_DIR)
// other dependencies, e.g. Eigen
```

Otherwise, you could configure Euclid using CMake first. It will output an EuclidConfig.cmake file in the build tree for you to use. You can set the variable `Euclid_DIR` to the path containing this file and then in your own CMakeLists.txt you could do

```cmake
find_package(Euclid)
target_link_libraries(your-target Euclid::Euclid)
```

and all the dependencies and compile options will be handled transitively by CMake.

## Getting Started

Here's an example which reads a mesh file, converts it to a CGAL::Surface_mesh data structure, computes its discrete gaussian curvatures and ouput the values into mesh colors.

```cpp
#include <algorithm>
#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Geometry/MeshHelpers.h>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <igl/colormap.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

int main()
{
    // Read triangle mesh into buffers
    std::vector<float> positions;
    std::vector<unsigned> indices;
    Euclid::read_off<3>("Euclid_root/data/bumpy.off", positions, indices);

    // Generate a CGAL::Surface_mesh
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    // Compute gaussian cuvatrues
    std::vector<float> curvatures;
    for (const auto& v : vertices(mesh)) {
        curvatures.push_back(Euclid::gaussian_curvature(v, mesh));
    }

    // Turn cuvatures into colors and output to a file
    auto [cmin, cmax] =
        std::minmax_element(curvatures.begin(), curvatures.end());
    std::vector<unsigned char> colors;
    for (auto c : curvatures) {
        auto curvature = (c - *cmin) / (*cmax - *cmin);
        float r, g, b;
        igl::colormap(igl::COLOR_MAP_TYPE_JET, curvature, r, g, b);
        colors.push_back(static_cast<unsigned char>(r * 255));
        colors.push_back(static_cast<unsigned char>(g * 255));
        colors.push_back(static_cast<unsigned char>(b * 255));
    }
    Euclid::write_ply<3>(
        "outdir/bumpy.ply", positions, nullptr, nullptr, &indices, &colors);
}
```

And here's the result rendered using MeshLab.

![](https://raw.githubusercontent.com/unclejimbo/Euclid/dev/resource/bumpy_gaussian.png)

## Examples

Currently there's no tutorial-like examples. However, you could check the [test cases](https://github.com/unclejimbo/Euclid/tree/dev/test) to see the usage of most functions and classes.

## License

MIT for code not related to any third-party libraries.

Otherwise it should follow whatever license the third-party libraries require, I guess?
