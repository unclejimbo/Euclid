#include <Euclid/Render/RayTracer.h>
#include <catch.hpp>

#include <string>

#include <CGAL/Simple_cartesian.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/Analysis/AABB.h>
#include <Euclid/Math/Vector.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;

TEST_CASE("Render, RayTracer", "[render][raytracer]")
{
    SECTION("triangle mesh")
    {
        std::string filename(DATA_DIR);
        filename.append("bunny.off");
        std::vector<float> positions;
        std::vector<unsigned> indices;
        Euclid::read_off<3>(filename, positions, indices);

        Euclid::AABB<Kernel> aabb(positions);
        Eigen::Vector3f center = Euclid::cgal_to_eigen<float>(aabb.center());
        Eigen::Vector3f view =
            center +
            Eigen::Vector3f(0.0f, 0.5f * aabb.ylen(), 2.0f * aabb.zlen());
        Eigen::Vector3f up = Eigen::Vector3f(0.0f, 1.0f, 0.0f);

        Euclid::RayTracer raytracer;
        positions.push_back(0.0f);
        raytracer.attach_geometry_buffers(positions, indices);

        const int width = 800;
        const int height = 600;

        SECTION("perspective camera")
        {
            std::vector<uint8_t> pixels(3 * width * height);
            SECTION("camera constructor")
            {
                Euclid::PerspectiveCamera cam(view,
                                              center,
                                              up,
                                              60.0f,
                                              static_cast<float>(width) /
                                                  height);
                raytracer.render_shaded(pixels.data(), cam, width, height, 1);

                std::string outfile(TMP_DIR);
                outfile.append("bunny_shaded1.png");
                stbi_write_png(outfile.c_str(),
                               width,
                               height,
                               3,
                               pixels.data(),
                               width * 3);
            }

            SECTION("camera functions")
            {
                Euclid::PerspectiveCamera cam;
                cam.lookat(view, center, up);
                cam.set_fov(60.0f);
                cam.set_aspect(width, height);
                raytracer.render_shaded(pixels.data(), cam, width, height, 1);

                std::string outfile(TMP_DIR);
                outfile.append("bunny_shaded2.png");
                stbi_write_png(outfile.c_str(),
                               width,
                               height,
                               3,
                               pixels.data(),
                               width * 3);
            }
        }

        SECTION("orthogonal camera")
        {
            std::vector<uint8_t> pixels(3 * width * height);
            auto xextent = aabb.xlen() * 1.5f;
            auto yextent = xextent * height / static_cast<float>(width);
            SECTION("camera constructor")
            {
                Euclid::OrthogonalCamera cam(
                    view, center, up, xextent, yextent);
                raytracer.render_shaded(pixels.data(), cam, width, height, 1);

                std::string outfile(TMP_DIR);
                outfile.append("bunny_shaded3.png");
                stbi_write_png(outfile.c_str(),
                               width,
                               height,
                               3,
                               pixels.data(),
                               width * 3);
            }

            SECTION("camera functions")
            {
                Euclid::OrthogonalCamera cam;
                cam.lookat(view, center, up);
                cam.set_extent(xextent, yextent);
                raytracer.render_shaded(pixels.data(), cam, width, height, 1);

                std::string outfile(TMP_DIR);
                outfile.append("bunny_shaded4.png");
                stbi_write_png(outfile.c_str(),
                               width,
                               height,
                               3,
                               pixels.data(),
                               width * 3);
            }
        }

        SECTION("multisampling")
        {
            std::vector<uint8_t> pixels(3 * width * height);
            Euclid::PerspectiveCamera cam(
                view, center, up, 60.0f, static_cast<float>(width) / height);
            raytracer.render_shaded(pixels.data(), cam, width, height, 8);

            std::string outfile(TMP_DIR);
            outfile.append("bunny_shaded5.png");
            stbi_write_png(
                outfile.c_str(), width, height, 3, pixels.data(), width * 3);
        }

        SECTION("change material")
        {
            std::vector<uint8_t> pixels(3 * width * height);
            Euclid::PerspectiveCamera cam(
                view, center, up, 60.0f, static_cast<float>(width) / height);
            Euclid::Material material;
            material.ambient << 0.2f, 0.0f, 0.0f;
            material.diffuse << 0.7f, 0.0f, 0.0f;
            raytracer.set_material(material);
            raytracer.render_shaded(pixels.data(), cam, width, height, 8);

            std::string outfile(TMP_DIR);
            outfile.append("bunny_shaded6.png");
            stbi_write_png(
                outfile.c_str(), width, height, 3, pixels.data(), width * 3);
        }

        SECTION("change background")
        {
            std::vector<uint8_t> pixels(3 * width * height);
            Euclid::PerspectiveCamera cam(
                view, center, up, 60.0f, static_cast<float>(width) / height);
            raytracer.set_background(0.0f, 0.3f, 0.4f);
            raytracer.render_shaded(pixels.data(), cam, width, height, 8);

            std::string outfile(TMP_DIR);
            outfile.append("bunny_shaded7.png");
            stbi_write_png(
                outfile.c_str(), width, height, 3, pixels.data(), width * 3);
        }

        SECTION("depth")
        {
            std::vector<uint8_t> pixels(width * height);
            Euclid::PerspectiveCamera cam(
                view, center, up, 60.0f, static_cast<float>(width) / height);
            raytracer.render_depth(pixels.data(), cam, width, height);

            std::string outfile(TMP_DIR);
            outfile.append("bunny_depth.png");
            stbi_write_png(
                outfile.c_str(), width, height, 1, pixels.data(), width);
        }

        SECTION("silhouette")
        {
            std::vector<uint8_t> pixels(width * height);
            Euclid::PerspectiveCamera cam(
                view, center, up, 60.0f, static_cast<float>(width) / height);
            raytracer.render_silhouette(pixels.data(), cam, width, height);

            std::string outfile(TMP_DIR);
            outfile.append("bunny_sillouette.png");
            stbi_write_png(
                outfile.c_str(), width, height, 1, pixels.data(), width);
        }

        SECTION("face index")
        {
            std::vector<uint8_t> pixels(3 * width * height);
            Euclid::PerspectiveCamera cam(
                view, center, up, 60.0f, static_cast<float>(width) / height);
            raytracer.render_index(pixels.data(), cam, width, height);

            std::string outfile(TMP_DIR);
            outfile.append("bunny_index.png");
            stbi_write_png(
                outfile.c_str(), width, height, 3, pixels.data(), width * 3);
        }
    }
}
