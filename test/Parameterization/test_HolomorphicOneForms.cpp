#include <catch2/catch.hpp>
#include <Euclid/Parameterization/HolomorphicOneForms.h>

#include <string>
#include <unordered_map>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Surface_mesh_parameterization/parameterize.h>
#include <CGAL/Surface_mesh_parameterization/IO/File_off.h>
#include <CGAL/Polygon_mesh_processing/measure.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/MeshUtil/MeshHelpers.h>
#include <Euclid/Parameterization/SeamMesh.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<double>;
using Point_2 = Kernel::Point_2;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;
using Vertex = typename boost::graph_traits<Mesh>::vertex_descriptor;
using Halfedge = boost::graph_traits<Mesh>::halfedge_descriptor;
using Edge = typename boost::graph_traits<Mesh>::edge_descriptor;
using Vertex_map = std::unordered_map<Vertex, bool>;
using Edge_map = std::unordered_map<Edge, bool>;
using Vertex_pmap = boost::associative_property_map<Vertex_map>;
using Edge_pmap = boost::associative_property_map<Edge_map>;
using Seam_mesh = CGAL::Seam_mesh<Mesh, Edge_pmap, Vertex_pmap>;
using UV_Map = std::unordered_map<Halfedge, Point_2>;
using UV_PMap = boost::associative_property_map<UV_Map>;

TEST_CASE("Parameterization, Holomorphic one forms",
          "[parameterization][holomorphiconeforms]")
{
    std::string filename(DATA_DIR);
    filename.append("Kitten.off");
    std::vector<float> positions;
    std::vector<int> indices;
    Euclid::read_off<3>(filename, positions, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    // compute cut graph
    Edge_map emap;
    Edge_pmap epmap(emap);
    Vertex_map vmap;
    Vertex_pmap vpmap(vmap);
    CGAL::Seam_mesh<Mesh, Edge_pmap, Vertex_pmap> seam_mesh(mesh, epmap, vpmap);
    Euclid::mark_seam_mesh_with_cut_graph(mesh, seam_mesh);

    // construct the holomorphic 1-forms parameterizer
    Euclid::Holomorphic_one_forms_parameterizer3<Mesh, Edge_pmap, Vertex_pmap>
        param(mesh);
    auto bnd = CGAL::Polygon_mesh_processing::longest_border(seam_mesh).first;
    UV_Map uv_map;
    UV_PMap uv_pmap(uv_map);

    // default coeffs
    {
        CGAL::Surface_mesh_parameterization::parameterize(
            seam_mesh, param, bnd, uv_pmap);

        // extract the parameterized mesh and write to file
        std::vector<double> pos, uv;
        std::vector<int> ind;
        Euclid::extract_seam_mesh(seam_mesh, uv_pmap, pos, uv, ind);
        std::string fmesh(TMP_DIR);
        fmesh += "Kitten_holo_mesh_1.ply";
        Euclid::write_ply<3>(fmesh, pos, nullptr, &uv, &ind, nullptr);

        // write the uv domain as well
        std::string fdisk(TMP_DIR);
        fdisk += "Kitten_holo_disk_1.off";
        std::ofstream ofs(fdisk);
        CGAL::Surface_mesh_parameterization::IO::output_uvmap_to_off(
            seam_mesh, bnd, uv_pmap, ofs);
    }

    // change coeffs
    {
        Eigen::Vector2d coeffs = Eigen::Vector2d::Ones();
        param.set_coeffs(coeffs);
        CGAL::Surface_mesh_parameterization::parameterize(
            seam_mesh, param, bnd, uv_pmap);

        // extract the parameterized mesh and write to file
        std::vector<double> pos, uv;
        std::vector<int> ind;
        Euclid::extract_seam_mesh(seam_mesh, uv_pmap, pos, uv, ind);
        std::string fmesh(TMP_DIR);
        fmesh += "Kitten_holo_mesh_2.ply";
        Euclid::write_ply<3>(fmesh, pos, nullptr, &uv, &ind, nullptr);

        // write the uv domain as well
        std::string fdisk(TMP_DIR);
        fdisk += "Kitten_holo_disk_2.off";
        std::ofstream ofs(fdisk);
        CGAL::Surface_mesh_parameterization::IO::output_uvmap_to_off(
            seam_mesh, bnd, uv_pmap, ofs);
    }
}
