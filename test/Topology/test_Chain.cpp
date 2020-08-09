#include <catch2/catch.hpp>
#include <Euclid/Topology/Chain.h>

#include <string>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/boost/graph/Dual.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/MeshUtil/MeshHelpers.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = Kernel::Point_3;
using Mesh = CGAL::Surface_mesh<Point_3>;
using Vert = Mesh::Vertex_index;
using DualMesh = CGAL::Dual<Mesh>;
using DualVert = Mesh::Face_index;

TEST_CASE("Topology, Chain", "[topology][chain]")
{
    std::string filename(DATA_DIR);
    filename.append("bunny.off");
    std::vector<float> positions;
    std::vector<int> indices;
    Euclid::read_off<3>(filename, positions, nullptr, &indices, nullptr);
    Mesh mesh;
    Euclid::make_mesh<3>(mesh, positions, indices);

    SECTION("Primal")
    {
        SECTION("Invalid")
        {
            const Euclid::VertexChain<Mesh> chain{ Vert(0), Vert(1), Vert(2) };
            REQUIRE(!Euclid::is_chain(mesh, chain));
            REQUIRE(!Euclid::is_loop(mesh, chain));
        }

        SECTION("Valid chain")
        {
            Euclid::VertexChain<Mesh> chain;
            auto h = *(halfedges(mesh).first);
            for (auto v : vertices_around_target(h, mesh)) {
                chain.push_back(v);
            }
            chain.pop_back();
            REQUIRE(Euclid::is_chain(mesh, chain));
            REQUIRE(!Euclid::is_loop(mesh, chain));
        }

        SECTION("Valid loop")
        {
            Euclid::VertexChain<Mesh> chain;
            auto h = *(halfedges(mesh).first);
            for (auto v : vertices_around_target(h, mesh)) {
                chain.push_back(v);
            }
            REQUIRE(Euclid::is_chain(mesh, chain));
            REQUIRE(Euclid::is_loop(mesh, chain));
        }
    }

    SECTION("Dual")
    {
        DualMesh dual(mesh);

        SECTION("Invalid")
        {
            const Euclid::VertexChain<DualMesh> chain{ DualVert(0),
                                                       DualVert(1),
                                                       DualVert(2) };
            REQUIRE(!Euclid::is_chain(dual, chain));
            REQUIRE(!Euclid::is_loop(dual, chain));
        }

        SECTION("Valid chain")
        {
            Euclid::VertexChain<DualMesh> chain;
            auto h = *(halfedges(mesh).first);
            for (auto f : faces_around_target(h, mesh)) {
                chain.push_back(f);
            }
            chain.pop_back();
            REQUIRE(Euclid::is_chain(dual, chain));
            REQUIRE(!Euclid::is_loop(dual, chain));
        }

        SECTION("Valid loop")
        {
            Euclid::VertexChain<DualMesh> chain;
            auto h = *(halfedges(mesh).first);
            for (auto f : faces_around_target(h, mesh)) {
                chain.push_back(f);
            }
            REQUIRE(Euclid::is_chain(dual, chain));
            REQUIRE(Euclid::is_loop(dual, chain));
        }
    }
}
