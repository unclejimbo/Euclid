#include <catch2/catch.hpp>
#include <Euclid/Util/Color.h>

#include <algorithm>
#include <string>
#include <vector>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Render/RayTracer.h>

#include <stb_image_write.h>

#include <config.h>

TEST_CASE("Util, Color", "[util][color]")
{
    std::vector<float> positions;
    std::vector<unsigned> indices;
    std::string filename(DATA_DIR);
    filename.append("bunny_vn.ply");
    Euclid::read_ply<3>(
        filename, positions, nullptr, nullptr, &indices, nullptr);

    SECTION("color map")
    {
        std::vector<float> x(positions.size() / 3);
        for (size_t i = 0; i < x.size(); ++i) {
            x[i] = positions[3 * i];
        }

        SECTION("range [0, 255]")
        {
            std::vector<uint8_t> colors;
            Euclid::colormap(
                igl::COLOR_MAP_TYPE_PLASMA, x, colors, true, false, false);

            REQUIRE(colors.size() == positions.size());
            auto [cmin, cmax] =
                std::minmax_element(colors.begin(), colors.end());
            REQUIRE(*cmin >= 0);
            REQUIRE(*cmax <= 255);

            std::string fout(TMP_DIR);
            fout.append("colormap.ply");
            Euclid::write_ply<3>(
                fout, positions, nullptr, nullptr, &indices, &colors);
        }

        SECTION("range [0, 255], alpha")
        {
            std::vector<uint8_t> colors;
            Euclid::colormap(
                igl::COLOR_MAP_TYPE_PLASMA, x, colors, true, true, false);

            REQUIRE(colors.size() == positions.size() * 4 / 3);
            auto [cmin, cmax] =
                std::minmax_element(colors.begin(), colors.end());
            REQUIRE(*cmin >= 0);
            REQUIRE(*cmax <= 255);

            std::string fout(TMP_DIR);
            fout.append("colormap.off");
            Euclid::write_off<3>(fout, positions, &colors, &indices, nullptr);
        }

        SECTION("range [0, 255], inverse color")
        {
            std::vector<uint8_t> colors;
            Euclid::colormap(
                igl::COLOR_MAP_TYPE_PLASMA, x, colors, true, false, true);

            REQUIRE(colors.size() == positions.size());
            auto [cmin, cmax] =
                std::minmax_element(colors.begin(), colors.end());
            REQUIRE(*cmin >= 0);
            REQUIRE(*cmax <= 255);

            std::string fout(TMP_DIR);
            fout.append("colormap_inverse.ply");
            Euclid::write_ply<3>(
                fout, positions, nullptr, nullptr, &indices, &colors);
        }

        SECTION("range [0, 1]")
        {
            constexpr const unsigned width = 800;
            constexpr const unsigned height = 600;
            std::vector<float> colors;
            Euclid::colormap(igl::COLOR_MAP_TYPE_PLASMA, x, colors);

            REQUIRE(colors.size() == positions.size());
            auto [cmin, cmax] =
                std::minmax_element(colors.begin(), colors.end());
            REQUIRE(*cmin >= 0.0);
            REQUIRE(*cmax <= 1.0);

            Euclid::RayTracer raytracer;
            positions.push_back(0.0f);
            raytracer.attach_geometry_buffers(positions, indices);
            colors.push_back(0.0f);
            raytracer.attach_color_buffer(&colors, true);
            Eigen::Vector3f view(1.5f, 1.5f, 1.5f);
            Eigen::Vector3f focus(0.0f, 0.0f, 0.0f);
            Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
            Euclid::PerspRayCamera cam;
            cam.lookat(view, focus, up);
            cam.set_fov(60.0f);
            cam.set_aspect(width, height);
            std::vector<uint8_t> pixels;
            raytracer.render_shaded(pixels, cam, width, height);

            std::string fout(TMP_DIR);
            fout.append("colormap.png");
            stbi_write_png(
                fout.c_str(), width, height, 3, pixels.data(), width * 3);
        }
    }

    SECTION("random colors")
    {
        SECTION("range [0, 255]")
        {
            std::vector<uint8_t> colors;
            Euclid::rnd_colors(positions.size() / 3, colors, true, false);

            REQUIRE(colors.size() == positions.size());
            auto [cmin, cmax] =
                std::minmax_element(colors.begin(), colors.end());
            REQUIRE(*cmin >= 0);
            REQUIRE(*cmax <= 255);

            std::string fout(TMP_DIR);
            fout.append("rnd_colors.ply");
            Euclid::write_ply<3>(
                fout, positions, nullptr, nullptr, &indices, &colors);
        }

        SECTION("range [0, 255], alpha")
        {
            std::vector<uint8_t> colors;
            Euclid::rnd_colors(positions.size() / 3, colors, true, true);

            REQUIRE(colors.size() == positions.size() * 4 / 3);
            auto [cmin, cmax] =
                std::minmax_element(colors.begin(), colors.end());
            REQUIRE(*cmin >= 0);
            REQUIRE(*cmax <= 255);

            std::string fout(TMP_DIR);
            fout.append("rnd_colors.off");
            Euclid::write_off<3>(fout, positions, &colors, &indices, nullptr);
        }

        SECTION("range [0, 1]")
        {
            constexpr const unsigned width = 800;
            constexpr const unsigned height = 600;
            std::vector<float> colors;
            Euclid::rnd_colors(indices.size() / 3, colors);

            REQUIRE(colors.size() == indices.size());
            auto [cmin, cmax] =
                std::minmax_element(colors.begin(), colors.end());
            REQUIRE(*cmin >= 0.0);
            REQUIRE(*cmax <= 1.0);

            Euclid::RayTracer raytracer;
            positions.push_back(0.0f);
            raytracer.attach_geometry_buffers(positions, indices);
            colors.push_back(0.0f);
            raytracer.attach_color_buffer(&colors);
            Eigen::Vector3f view(1.5f, 1.5f, 1.5f);
            Eigen::Vector3f focus(0.0f, 0.0f, 0.0f);
            Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
            Euclid::PerspRayCamera cam;
            cam.lookat(view, focus, up);
            cam.set_fov(60.0f);
            cam.set_aspect(width, height);
            std::vector<uint8_t> pixels;
            raytracer.render_shaded(pixels, cam, width, height);

            std::string fout(TMP_DIR);
            fout.append("rnd_colors.png");
            stbi_write_png(
                fout.c_str(), width, height, 3, pixels.data(), width * 3);
        }
    }
}
