# Welcome to Euclid

- [GitHub repo](https://github.com/unclejimbo/Euclid)
- [Latest documentation](https://unclejimbo.github.io/Euclid/)

## Introduction

Euclid is a header only library for geometry processing and shape analysis.

It contains some utilities and algorithms which extend and cooperate with other popular libraries around there, like Eigen(libigl), CGAL, to name a few.

The purpose of Euclid is not to replace any of the above packages, but to glue things together as well as to provide more algorithms which do not appear in those libraries.

## Dependencies

Different packages in Euclid may require a different set of dependencies. Although some are dependency free (like the IO module), but most of them rely on the following packages, which are also marked as 'REQUIRED' if you configure using CMake, including

- [Boost](https://www.boost.org/)
- [CGAL](https://www.cgal.org/index.html)
- [Eigen](http://eigen.tuxfamily.org/index.php?title=Main_Page)
- [Libigl](http://libigl.github.io/libigl/)

Additional dependencies are required by some packages, including

- [Spectra](https://spectralib.org/) for solving eigenvalue problems.
- [Embree](http://embree.github.io) for fast cpu ray tracing.
- [Vulkan](https://www.vulkan.org/) for headless gpu rendering.
- [TTK](https://topology-tool-kit.github.io/) for topological shape analysis.
- [Cereal](http://uscilab.github.io/cereal/index.html) for serialization.

Make sure you properly compile and link to the above libraries when they are used. Also, Euclid uses features in the C++17 standard, so you'll need a C++17 enabled compiler.

## Installation

Since it's a header only library, it is not mandatory to configure this project with CMake. Although be sure to configure other dependencies properly, as some of them are not header only.

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

## Getting Started

Here's an example which reads a mesh file, converts it to a CGAL::Surface_mesh data structure, computes its discrete gaussian curvatures and ouput the values into mesh colors.

```cpp
#include <vector>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/MeshUtil/CGALMesh.h>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Util/Color.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

int main()
{
    // Read triangle mesh into buffers
    std::vector<float> positions;
    std::vector<unsigned> indices;
    Euclid::read_off<3>("Euclid_root/data/bumpy.off", positions, nullptr, &indices, nullptr);

    // Generate a CGAL::Surface_mesh
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    // Compute gaussian curvatures
    auto curvatures = Euclid::gaussian_curvatures(mesh);

    // Turn curvatures into colors and output to a file
    std::vector<unsigned char> colors;
    Euclid::colormap(igl::COLOR_MAP_TYPE_JET, curvatures, colors, true);
    Euclid::write_ply<3>(
        "outdir/bumpy.ply", positions, nullptr, nullptr, &indices, &colors);
}
```

## Examples

See the [examples](https://github.com/unclejimbo/Euclid/tree/dev/examples) folder for more tutorials. However, many modules are not covered yet. For a more complete example, you could check the [test cases](https://github.com/unclejimbo/Euclid/tree/dev/test) to see the usage of most functions and classes. More information on how to run the tests could be found [here](https://codedocs.xyz/unclejimbo/Euclid/md_docs_runtest.html).

## License

MIT for code not related to any third-party libraries.

Otherwise it should follow whatever license the third-party libraries require.
