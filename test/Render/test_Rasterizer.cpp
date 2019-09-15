#include <catch2/catch.hpp>
#include <Euclid/Render/Rasterizer.h>

#include <random>
#include <string>

#include <CGAL/Simple_cartesian.h>
#include <Euclid/IO/OffIO.h>
#include <Euclid/IO/PlyIO.h>
#include <Euclid/Analysis/AABB.h>
#include <Euclid/Math/Vector.h>
#include <Eigen/Geometry>

#include <stb_image_write.h>

#include <config.h>

using Kernel = CGAL::Simple_cartesian<float>;

TEST_CASE("Render, Rasterizer", "[render][rasterizer]")
{
    const uint32_t width = 800;
    const uint32_t height = 600;

    std::string filename(DATA_DIR);
    filename.append("bunny_vn.ply");
    std::vector<float> positions;
    std::vector<float> normals;
    std::vector<unsigned> indices;
    Euclid::read_ply<3>(
        filename, positions, &normals, nullptr, &indices, nullptr);

    // fit model into unit box
    Euclid::AABB<Kernel> aabb(positions);
    Eigen::Vector3f translation;
    Euclid::cgal_to_eigen(aabb.center(), translation);
    float scale = std::max(aabb.xlen(), aabb.ylen());
    scale = std::max(scale, aabb.zlen());
    Eigen::Transform<float, 3, Eigen::Affine> transform;
    transform.setIdentity();
    transform.translate(-translation);
    transform.scale(1.0f / scale);

    Eigen::Vector3f view(0.0f, 1.0f, 2.0f);
    Eigen::Vector3f center(0.0f, 0.0f, 0.0f);
    Eigen::Vector3f up(0.0f, 1.0f, 0.0f);
    Euclid::PerspRasCamera persp(
        view, center, up, 60.0f, width, height, 0.5f, 5.0f);

    Euclid::Rasterizer rasterizer(
        width, height, Euclid::Rasterizer::SAMPLE_COUNT_1);
    rasterizer.attach_position_buffer(positions.data(), positions.size());
    rasterizer.attach_normal_buffer(normals.data(), normals.size());
    rasterizer.attach_index_buffer(indices.data(), indices.size());

    Euclid::Light l;
    l.position = view + Eigen::Vector3f(0.1f, 0.1f, 0.1f);
    l.color = { 1.0f, 1.0f, 1.0f };
    l.intensity = 0.5f;
    rasterizer.set_light(l);

    SECTION("perspective camera")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        rasterizer.render_shaded(transform.matrix(), persp, pixels);

        std::string outfile(TMP_DIR);
        outfile.append("rasterizer_persp.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("orthographic camera")
    {
        const float xextent = 2.0f;
        const float yextent = xextent * height / (width + 1e-6f);
        Euclid::OrthoRasCamera ortho(
            view, center, up, xextent, yextent, 0.1f, 5.0f);

        std::vector<uint8_t> pixels(3 * width * height);
        rasterizer.render_shaded(transform.matrix(), ortho, pixels);

        std::string outfile(TMP_DIR);
        outfile.append("rasterizer_ortho.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("multisampling")
    {
        rasterizer.set_image(width, height, Euclid::Rasterizer::SAMPLE_COUNT_8);

        std::vector<uint8_t> pixels(3 * width * height);
        rasterizer.render_shaded(transform.matrix(), persp, pixels);

        std::string outfile(TMP_DIR);
        outfile.append("rasterizer_multisample.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("change material")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        Euclid::Material material;
        material.ambient << 0.2f, 0.0f, 0.0f;
        material.diffuse << 0.7f, 0.0f, 0.0f;
        rasterizer.set_material(material);
        rasterizer.render_shaded(transform.matrix(), persp, pixels);

        std::string outfile(TMP_DIR);
        outfile.append("rasterizer_material.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("change background")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        rasterizer.set_background(0.0f, 0.3f, 0.4f);
        rasterizer.render_shaded(transform.matrix(), persp, pixels);

        std::string outfile(TMP_DIR);
        outfile.append("rasterizer_background.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("random vertex color")
    {
        std::random_device rd;
        std::minstd_rand rd_gen(rd());
        std::uniform_real_distribution rd_number(0.0, 1.0);
        std::vector<float> rd_colors(positions.size());
        for (auto& color : rd_colors) {
            color = rd_number(rd_gen);
        }
        rasterizer.attach_color_buffer(rd_colors.data(), rd_colors.size());
        std::vector<uint8_t> pixels(3 * width * height);
        rasterizer.render_shaded(transform.matrix(), persp, pixels);

        std::string outfile(TMP_DIR);
        outfile.append("rasterizer_vertex_color.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("unlit")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        rasterizer.render_unlit(transform.matrix(), persp, pixels);

        std::string outfile(TMP_DIR);
        outfile.append("rasterizer_unlit.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("unlit vcolor")
    {
        std::random_device rd;
        std::minstd_rand rd_gen(rd());
        std::uniform_real_distribution rd_number(0.0, 1.0);
        std::vector<float> rd_colors(positions.size());
        for (auto& color : rd_colors) {
            color = rd_number(rd_gen);
        }
        rasterizer.attach_color_buffer(rd_colors.data(), rd_colors.size());
        std::vector<uint8_t> pixels(3 * width * height);
        rasterizer.render_unlit(transform.matrix(), persp, pixels);

        std::string outfile(TMP_DIR);
        outfile.append("rasterizer_unlit_vcolor.png");
        stbi_write_png(
            outfile.c_str(), width, height, 3, pixels.data(), width * 3);
    }

    SECTION("depth")
    {
        std::vector<uint8_t> pixels(3 * width * height);
        rasterizer.render_depth(transform.matrix(), persp, pixels);

        std::string outfile1(TMP_DIR);
        outfile1.append("rasterizer_depth_linear.png");
        stbi_write_png(
            outfile1.c_str(), width, height, 3, pixels.data(), width * 3);

        rasterizer.render_depth(transform.matrix(), persp, pixels, true, false);
        std::string outfile2(TMP_DIR);
        outfile2.append("rasterizer_depth.png");
        stbi_write_png(
            outfile2.c_str(), width, height, 3, pixels.data(), width * 3);

        std::vector<float> values(3 * width * height);
        rasterizer.render_depth(transform.matrix(), persp, values);
    }
}
