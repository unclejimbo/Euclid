#include <catch2/catch.hpp>
#include <Euclid/Render/RayTracer.h>

#include <string>
#include <CGAL/Simple_cartesian.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/Analysis/AABB.h>
#include <Euclid/Math/Vector.h>
#include <Euclid/Util/Color.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;

TEST_CASE("Render, RayTracer", "[render][raytracer]")
{
    std::string filename(DATA_DIR);
    filename.append("bunny.off");
    std::vector<float> positions;
    std::vector<unsigned> indices;
    Euclid::read_off<3>(filename, positions, nullptr, &indices, nullptr);

    Euclid::AABB<Kernel> aabb(positions);
    Eigen::Vector3f center;
    Euclid::cgal_to_eigen(aabb.center(), center);
    Eigen::Vector3f view =
        center + Eigen::Vector3f(0.0f, 0.5f * aabb.ylen(), 2.0f * aabb.zlen());
    Eigen::Vector3f up = Eigen::Vector3f(0.0f, 1.0f, 0.0f);

    Euclid::RayTracer raytracer;
    positions.push_back(0.0f);
    raytracer.attach_geometry_buffers(positions, indices);

    const int width = 800;
    const int height = 600;

    Euclid::PerspRayCamera cam(view, center, up, 60.0f, width, height);

    SECTION("perspective camera")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_persp.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("orthographic camera")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        auto xextent = aabb.xlen() * 1.5f;
        auto yextent = xextent * height / static_cast<float>(width);
        Euclid::OrthoRayCamera cam(view, center, up, xextent, yextent);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_ortho.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("multisampling")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        raytracer.render_shaded(pixels, cam, width, height, 8);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_multisample.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("change material")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        Euclid::Material material;
        material.ambient << 0.2f, 0.0f, 0.0f;
        material.diffuse << 0.7f, 0.0f, 0.0f;
        raytracer.set_material(material);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_material.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("change background")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        raytracer.set_background(0.0f, 0.3f, 0.4f);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_background.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("random face color")
    {
        std::vector<float> rd_colors;
        Euclid::rnd_colors(indices.size() / 3, rd_colors);
        raytracer.attach_color_buffer(&rd_colors);

        std::vector<uint8_t> pixels(3 * width * height);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_face_color.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("random vertex color")
    {
        std::vector<float> rd_colors;
        Euclid::rnd_colors(positions.size() / 3, rd_colors);
        rd_colors.push_back(0.0f);
        raytracer.attach_color_buffer(&rd_colors, true);

        std::vector<uint8_t> pixels(3 * width * height);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_vertex_color.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("light off")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        raytracer.enable_light(false);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_lightoff.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("depth")
    {
        std::vector<uint8_t> pixels(width * height);
        raytracer.render_depth(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_depth.png");
        stbi_write_png(outfile.c_str(), width, height, 1, pixels.data(), width);
    }

    SECTION("silhouette")
    {
        std::vector<uint8_t> pixels(width * height);
        raytracer.render_silhouette(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_silhouette.png");
        stbi_write_png(outfile.c_str(), width, height, 1, pixels.data(), width);
    }

    SECTION("face index color")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        raytracer.render_index(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_fidx1.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("face index")
    {
        std::vector<uint32_t> indices(width * height);
        raytracer.render_index(indices, cam, width, height);

        std::vector<uint8_t> pixels(3 * width * height);
        for (size_t i = 0; i < indices.size(); ++i) {
            pixels[3 * i + 0] = indices[i] & 0xFF;
            pixels[3 * i + 1] = (indices[i] >> 8) & 0xFF;
            pixels[3 * i + 2] = (indices[i] >> 16) & 0xFF;
        }
        std::string outfile(TMP_DIR);
        outfile.append("raytracer_fidx2.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("face index using face color")
    {
        std::vector<float> colors(indices.size());
        for (unsigned i = 0; i < indices.size() / 3; ++i) {
            uint8_t r = (i + 1) & 0x000000FF;
            uint8_t g = ((i + 1) >> 8) & 0x000000FF;
            uint8_t b = ((i + 1) >> 16) & 0x000000FF;
            colors[3 * i + 0] = r / 255.0f;
            colors[3 * i + 1] = g / 255.0f;
            colors[3 * i + 2] = b / 255.0f;
        }
        raytracer.attach_color_buffer(&colors);
        Euclid::Material material;
        material.ambient << 0.0f, 0.0f, 0.0f;
        material.diffuse << 0.0f, 0.0f, 0.0f;
        raytracer.set_material(material);
        raytracer.enable_light(false);

        std::vector<uint8_t> pixels(3 * width * height);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_fidx3.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("face mask")
    {
        std::vector<uint8_t> mask(indices.size() / 3);
        for (size_t i = 0; i < indices.size(); i += 3) {
            auto y0 = positions[3 * indices[i + 0] + 1];
            auto y1 = positions[3 * indices[i + 1] + 1];
            auto y2 = positions[3 * indices[i + 2] + 1];
            if (y0 < center[1] && y1 < center[1] && y2 < center[1]) {
                mask[i / 3] = 0;
            }
            else {
                mask[i / 3] = 1;
            }
        }
        raytracer.attach_face_mask_buffer(&mask);

        std::vector<uint8_t> pixels(3 * width * height);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_face_mask.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("change geometry")
    {
        std::string filename(DATA_DIR);
        filename.append("kitten.off");
        std::vector<float> positions;
        std::vector<unsigned> indices;
        Euclid::read_off<3>(filename, positions, nullptr, &indices, nullptr);

        Euclid::AABB<Kernel> aabb(positions);
        Eigen::Vector3f center;
        Euclid::cgal_to_eigen(aabb.center(), center);
        Eigen::Vector3f view =
            center +
            Eigen::Vector3f(0.0f, 0.5f * aabb.ylen(), 2.0f * aabb.zlen());
        Eigen::Vector3f up = Eigen::Vector3f(0.0f, 1.0f, 0.0f);

        positions.push_back(0.0f);
        raytracer.attach_geometry_buffers(positions, indices);

        std::vector<uint8_t> pixels(3 * width * height);
        Euclid::PerspRayCamera cam(view, center, up, 60.0f, width, height);
        raytracer.render_shaded(pixels, cam, width, height);

        std::string outfile(TMP_DIR);
        outfile.append("raytracer_geometry.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }
}
