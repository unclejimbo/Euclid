#include <catch2/catch.hpp>
#include <Euclid/SurfaceDelaunay/DelaunayMesh.h>

#include <string>
#include <vector>

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/MeshUtil/CGALMesh.h>
#include <Euclid/Util/Timer.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;
using Point_3 = typename Kernel::Point_3;
using Vector_3 = typename Kernel::Vector_3;
using Mesh = CGAL::Surface_mesh<Point_3>;

template<typename Mesh>
class RemeshVisitor
{
public:
    void on_started(Mesh&)
    {
        _nflipped = 0;
        _nsplit = 0;
        _timer.tick();
    }

    void on_finished(Mesh&)
    {
        auto t = _timer.tock();
        std::cout << "Time used: " << t << " #flip: " << _nflipped
                  << " #split: " << _nsplit << std::endl;
    }

    void on_flipping(Mesh&, const Euclid::edge_t<Mesh>&) {}

    void on_flipped(Mesh&, const Euclid::edge_t<Mesh>&)
    {
        ++_nflipped;
    }

    void on_nonflippable(Mesh&, const Euclid::edge_t<Mesh>&) {}

    void on_splitting(Mesh&, const Euclid::edge_t<Mesh>&) {}

    void on_split(Mesh&, const Euclid::SplitSite<Mesh>&)
    {
        ++_nsplit;
    }

private:
    Euclid::Timer _timer;
    int _nflipped;
    int _nsplit;
};

TEST_CASE("SurfaceDelaunay, DelaunayMesh", "[surfacedelaunay][delaunaymesh]")
{
    RemeshVisitor<Mesh> visitor;

    SECTION("a relatively good mesh")
    {
        std::string fmesh(DATA_DIR);
        fmesh.append("bunny.off");
        std::vector<float> positions;
        std::vector<int> indices;
        Euclid::read_off<3>(fmesh, positions, nullptr, &indices, nullptr);
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);

        SECTION("simple flip")
        {
            Euclid::remesh_delaunay(
                mesh, visitor, Euclid::RemeshDelaunayScheme::SimpleFlip);
            REQUIRE(Euclid::is_delaunay(mesh));

            Euclid::extract_mesh<3>(mesh, positions, indices);
            std::string fout(TMP_DIR);
            fout.append("bunny_delaunay_sf.off");
            Euclid::write_off<3>(fout, positions, nullptr, &indices, nullptr);
        }

        SECTION("geometry preserving")
        {
            Euclid::remesh_delaunay(
                mesh,
                visitor,
                Euclid::RemeshDelaunayScheme::GeometryPreserving);
            REQUIRE(Euclid::is_delaunay(mesh));

            Euclid::extract_mesh<3>(mesh, positions, indices);
            std::string fout(TMP_DIR);
            fout.append("bunny_delaunay_gp.off");
            Euclid::write_off<3>(fout, positions, nullptr, &indices, nullptr);
        }

        SECTION("feature preserving")
        {
            Euclid::remesh_delaunay(
                mesh, visitor, Euclid::RemeshDelaunayScheme::FeaturePreserving);
            REQUIRE(Euclid::is_delaunay(mesh));

            Euclid::extract_mesh<3>(mesh, positions, indices);
            std::string fout(TMP_DIR);
            fout.append("bunny_delaunay_fp.off");
            Euclid::write_off<3>(fout, positions, nullptr, &indices, nullptr);
        }
    }

    SECTION("a mesh containing non-flippable edges")
    {
        std::string fmesh(DATA_DIR);
        fmesh.append("sharp_sphere.off");
        std::vector<float> positions;
        std::vector<int> indices;
        Euclid::read_off<3>(fmesh, positions, nullptr, &indices, nullptr);
        Mesh mesh;
        Euclid::make_mesh<3>(mesh, positions, indices);

        SECTION("simple flip")
        {
            Euclid::remesh_delaunay(
                mesh, visitor, Euclid::RemeshDelaunayScheme::SimpleFlip);
            REQUIRE(!Euclid::is_delaunay(mesh));

            Euclid::extract_mesh<3>(mesh, positions, indices);
            std::string fout(TMP_DIR);
            fout.append("sharp_sphere_delaunay_sf.off");
            Euclid::write_off<3>(fout, positions, nullptr, &indices, nullptr);
        }

        SECTION("geometry preserving")
        {
            Euclid::remesh_delaunay(
                mesh,
                visitor,
                Euclid::RemeshDelaunayScheme::GeometryPreserving);
            REQUIRE(Euclid::is_delaunay(mesh));

            Euclid::extract_mesh<3>(mesh, positions, indices);
            std::string fout(TMP_DIR);
            fout.append("sharp_sphere_delaunay_gp.off");
            Euclid::write_off<3>(fout, positions, nullptr, &indices, nullptr);
        }

        SECTION("feature preserving")
        {
            Euclid::remesh_delaunay(
                mesh,
                visitor,
                Euclid::RemeshDelaunayScheme::FeaturePreserving,
                20.0);
            REQUIRE(Euclid::is_delaunay(mesh));

            Euclid::extract_mesh<3>(mesh, positions, indices);
            std::string fout(TMP_DIR);
            fout.append("sharp_sphere_delaunay_fp.off");
            Euclid::write_off<3>(fout, positions, nullptr, &indices, nullptr);
        }
    }
}
