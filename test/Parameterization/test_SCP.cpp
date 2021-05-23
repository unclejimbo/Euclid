#include <catch2/catch.hpp>

#include <string>
#include <unordered_map>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh_parameterization/parameterize.h>
#include <CGAL/Surface_mesh_parameterization/IO/File_off.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/MeshUtil/CGALMesh.h>
#include <Euclid/Parameterization/SCP.h> // include after SM

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_2 = Kernel::Point_2;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;
using Vertex = typename boost::graph_traits<Mesh>::vertex_descriptor;
using UV_Map = std::unordered_map<Vertex, Point_2>;
using UV_PMap = boost::associative_property_map<UV_Map>;

TEST_CASE("Parameterization, SCP", "[parameterization][scp]")
{
    std::string fin(DATA_DIR);
    fin.append("face.off");
    std::vector<float> positions;
    std::vector<int> indices;
    Euclid::read_off<3>(fin, positions, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    auto bnd = CGAL::Polygon_mesh_processing::longest_border(mesh).first;
    UV_Map uvm;
    UV_PMap uvpm(uvm);

    SECTION("parameterizer")
    {
        Euclid::SCP_parameterizer_3<Mesh> param;
        CGAL::Surface_mesh_parameterization::parameterize(
            mesh, param, bnd, uvpm);

        std::vector<float> texcoords(mesh.number_of_vertices() * 2);
        for (auto v : vertices(mesh)) {
            auto uv = uvm[v];
            texcoords[v.idx() * 2 + 0] = uv.x();
            texcoords[v.idx() * 2 + 1] = uv.y();
        }

        std::string fmesh(TMP_DIR);
        fmesh.append("scp_mesh_1.ply");
        Euclid::write_ply<3>(
            fmesh, positions, nullptr, &texcoords, &indices, nullptr);

        std::string fdisk(TMP_DIR);
        fdisk.append("scp_disk_1.off");
        std::ofstream ofs(fdisk);
        CGAL::Surface_mesh_parameterization::IO::output_uvmap_to_off(
            mesh, bnd, uvpm, ofs);
    }

    SECTION("free functions")
    {
        Euclid::spectral_conformal_parameterization(mesh, uvpm);

        std::vector<float> texcoords(mesh.number_of_vertices() * 2);
        for (auto v : vertices(mesh)) {
            auto uv = uvm[v];
            texcoords[v.idx() * 2 + 0] = uv.x();
            texcoords[v.idx() * 2 + 1] = uv.y();
        }

        std::string fmesh(TMP_DIR);
        fmesh.append("scp_mesh_2.ply");
        Euclid::write_ply<3>(
            fmesh, positions, nullptr, &texcoords, &indices, nullptr);

        std::string fdisk(TMP_DIR);
        fdisk.append("scp_disk_2.off");
        std::ofstream ofs(fdisk);
        CGAL::Surface_mesh_parameterization::IO::output_uvmap_to_off(
            mesh, bnd, uvpm, ofs);
    }
}
