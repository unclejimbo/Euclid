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
#include <Euclid/Parameterization/SeamMesh.h>  // include after SM
#include <Euclid/Parameterization/RicciFlow.h> // include after SM

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
using VertexRadiusMap = std::unordered_map<Vertex, double>;
using VertexRadiusPMap = boost::associative_property_map<VertexRadiusMap>;
using EdgeWeightMap = std::unordered_map<Edge, double>;
using EdgeWeightPMap = boost::associative_property_map<EdgeWeightMap>;
using VertexCurvatureMap = std::unordered_map<Vertex, double>;
using VertexCurvaturePMap = boost::associative_property_map<VertexCurvatureMap>;

TEST_CASE("Parameterization, Ricci flow", "[parameterization][ricciflow]")
{
    Euclid::RicciFlowSolverSettings settings;
    settings.verbose = true;

    SECTION("parameterizer")
    {
        // embed to plane
        {
            std::string filename(DATA_DIR);
            filename.append("rocker_arm.off");
            std::vector<float> positions;
            std::vector<int> indices;
            Euclid::read_off<3>(
                filename, positions, nullptr, &indices, nullptr);
            Mesh mesh;
            Euclid::make_mesh<3>(mesh, positions, indices);

            // compute cut graph
            Edge_map emap;
            Edge_pmap epmap(emap);
            Vertex_map vmap;
            Vertex_pmap vpmap(vmap);
            CGAL::Seam_mesh<Mesh, Edge_pmap, Vertex_pmap> seam_mesh(
                mesh, epmap, vpmap);
            Euclid::mark_seam_mesh_with_cut_graph(mesh, seam_mesh);

            auto bnd =
                CGAL::Polygon_mesh_processing::longest_border(seam_mesh).first;
            UV_Map uvm;
            UV_PMap uvpmap(uvm);
            Euclid::Ricci_flow_parameterizer3<Mesh, Edge_pmap, Vertex_pmap>
                param(mesh);
            param.set_solver_settings(settings);
            CGAL::Surface_mesh_parameterization::parameterize(
                seam_mesh, param, bnd, uvpmap);

            // extract the parameterized mesh and write to file
            std::vector<double> pos, uv;
            std::vector<int> ind;
            Euclid::extract_seam_mesh(seam_mesh, uvpmap, pos, uv, ind);
            std::string fmesh(TMP_DIR);
            fmesh += "ricci_flow_euclidean_mesh_1.ply";
            Euclid::write_ply<3>(fmesh, pos, nullptr, &uv, &ind, nullptr);

            // write the uv domain as well
            std::string fdisk(TMP_DIR);
            fdisk += "ricci_flow_euclidean_disk_1.off";
            std::ofstream ofs(fdisk);
            CGAL::Surface_mesh_parameterization::IO::output_uvmap_to_off(
                seam_mesh, bnd, uvpmap, ofs);
        }

        // add cone singularities
        {
            std::string filename(DATA_DIR);
            filename.append("face.off");
            std::vector<float> positions;
            std::vector<int> indices;
            Euclid::read_off<3>(
                filename, positions, nullptr, &indices, nullptr);
            Mesh mesh;
            Euclid::make_mesh<3>(mesh, positions, indices);

            Edge_map emap;
            Edge_pmap epmap(emap);
            Vertex_map vmap;
            Vertex_pmap vpmap(vmap);
            CGAL::Seam_mesh<Mesh, Edge_pmap, Vertex_pmap> seam_mesh(
                mesh, epmap, vpmap);

            auto bnd =
                CGAL::Polygon_mesh_processing::longest_border(seam_mesh).first;
            UV_Map uvm;
            UV_PMap uvpmap(uvm);
            Euclid::Ricci_flow_parameterizer3<Mesh, Edge_pmap, Vertex_pmap>
                param(mesh);
            param.add_cone(Mesh::Vertex_index(2785), 0.5);
            param.add_cone(Mesh::Vertex_index(1559), 0.5);
            param.add_cone(Mesh::Vertex_index(5214), 0.5);
            param.add_cone(Mesh::Vertex_index(5910), 0.5);
            param.set_solver_settings(settings);
            CGAL::Surface_mesh_parameterization::parameterize(
                seam_mesh, param, bnd, uvpmap);

            std::vector<double> pos, uv;
            std::vector<int> ind;
            Euclid::extract_seam_mesh(seam_mesh, uvpmap, pos, uv, ind);
            std::string fmesh(TMP_DIR);
            fmesh += "ricci_flow_euclidean_mesh_2.ply";
            Euclid::write_ply<3>(fmesh, pos, nullptr, &uv, &ind, nullptr);

            std::string fdisk(TMP_DIR);
            fdisk += "ricci_flow_euclidean_disk_2.off";
            std::ofstream ofs(fdisk);
            CGAL::Surface_mesh_parameterization::IO::output_uvmap_to_off(
                seam_mesh, bnd, uvpmap, ofs);
        }
    }

    SECTION("free functions")
    {
        std::string filename(DATA_DIR);
        filename.append("rocker_arm.off");
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
        CGAL::Seam_mesh<Mesh, Edge_pmap, Vertex_pmap> seam_mesh(
            mesh, epmap, vpmap);
        Euclid::mark_seam_mesh_with_cut_graph(mesh, seam_mesh);

        auto bnd =
            CGAL::Polygon_mesh_processing::longest_border(seam_mesh).first;
        UV_Map uvm;
        UV_PMap uvpmap(uvm);
        VertexRadiusMap vrm;
        VertexRadiusPMap vrpm(vrm);
        EdgeWeightMap ewm;
        EdgeWeightPMap ewpm(ewm);
        VertexCurvatureMap vcm;
        VertexCurvaturePMap vcpm(vcm);
        for (auto v : mesh.vertices()) {
            put(vcpm, v, 0.0);
        }
        Euclid::circle_packing_metric(mesh, vrpm, ewpm);
        Euclid::ricci_flow(mesh, vrpm, ewpm, vcpm, settings);
        Euclid::embed_circle_packing_metric(
            mesh, vrpm, ewpm, seam_mesh, uvpmap);

        std::vector<double> pos, uv;
        std::vector<int> ind;
        Euclid::extract_seam_mesh(seam_mesh, uvpmap, pos, uv, ind);
        std::string fmesh(TMP_DIR);
        fmesh += "riccif_flow_mesh_3.ply";
        Euclid::write_ply<3>(fmesh, pos, nullptr, &uv, &ind, nullptr);

        std::string fdisk(TMP_DIR);
        fdisk += "riccif_flow_disk_3.off";
        std::ofstream ofs(fdisk);
        CGAL::Surface_mesh_parameterization::IO::output_uvmap_to_off(
            seam_mesh, bnd, uvpmap, ofs);
    }
}
