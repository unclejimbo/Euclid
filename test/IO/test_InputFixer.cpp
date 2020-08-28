#include <catch2/catch.hpp>
#include <Euclid/IO/InputFixer.h>

#include <algorithm>
#include <array>
#include <vector>
#include <Euclid/Util/Assert.h>
#include <Euclid/IO/OffIO.h>

#include <config.h>

// Test whether two position vectors are identical
template<typename T>
static bool _pos_eq(const std::vector<T>& test, const std::vector<T>& expect)
{
    EASSERT(test.size() % 3 == 0);
    EASSERT(expect.size() % 3 == 0);
    if (test.size() != expect.size()) {
        return false;
    }

    using Point = std::array<T, 3>;

    std::vector<Point> test_points;
    for (size_t i = 0; i < test.size(); i += 3) {
        test_points.push_back({ { test[i], test[i + 1], test[i + 2] } });
    }
    std::sort(test_points.begin(), test_points.end());

    std::vector<Point> expect_points;
    for (size_t i = 0; i < expect.size(); i += 3) {
        expect_points.push_back(
            { { expect[i], expect[i + 1], expect[i + 2] } });
    }
    std::sort(expect_points.begin(), expect_points.end());

    return test_points == expect_points;
}

// Test whether two position vectors are identical,
// meanwhile the associated indices are correct
template<int N, typename T1, typename T2>
static bool _pos_eq(const std::vector<T1>& test_pos,
                    const std::vector<T2>& test_idx,
                    const std::vector<T1>& expect_pos,
                    const std::vector<T2>& expect_idx)
{
    EASSERT(test_idx.size() % N == 0);
    EASSERT(expect_idx.size() % N == 0);
    if (!_pos_eq(test_pos, expect_pos)) {
        return false;
    }

    using Point = std::array<T1, 3>;
    using Face = std::array<Point, N>;

    std::vector<Face> test_faces;
    for (size_t i = 0; i < test_idx.size(); i += N) {
        Face face;
        for (size_t j = 0; j < N; ++j) {
            face[j] = { { test_pos[test_idx[i + j] * 3 + 0],
                          test_pos[test_idx[i + j] * 3 + 1],
                          test_pos[test_idx[i + j] * 3 + 2] } };
        }
        auto beg = std::min_element(face.begin(), face.end());
        std::rotate(face.begin(), beg, face.end());
        test_faces.push_back(std::move(face));
    }
    std::sort(test_faces.begin(), test_faces.end());

    std::vector<Face> expect_faces;
    for (size_t i = 0; i < expect_idx.size(); i += N) {
        Face face;
        for (size_t j = 0; j < N; ++j) {
            face[j] = { { expect_pos[expect_idx[i + j] * 3 + 0],
                          expect_pos[expect_idx[i + j] * 3 + 1],
                          expect_pos[expect_idx[i + j] * 3 + 2] } };
        }
        auto beg = std::min_element(face.begin(), face.end());
        std::rotate(face.begin(), beg, face.end());
        expect_faces.push_back(std::move(face));
    }
    std::sort(expect_faces.begin(), expect_faces.end());

    return test_faces == expect_faces;
}

// Test whether two index vectors are identical
template<int N, typename T>
static bool _idx_eq(const std::vector<T>& test, const std::vector<T>& expected)
{
    EASSERT(test.size() == expected.size());
    EASSERT(test.size() % N == 0);
    EASSERT(expected.size() % N == 0);

    using Face = std::array<T, N>;

    std::vector<Face> test_faces;
    for (size_t i = 0; i < test.size(); i += N) {
        Face face;
        for (size_t j = 0; j < N; ++j) {
            face[j] = test[i + j];
        }
        auto beg = std::min_element(face.begin(), face.end());
        std::rotate(face.begin(), beg, face.end());
        test_faces.push_back(std::move(face));
    }
    std::sort(test_faces.begin(), test_faces.end());

    std::vector<Face> expected_faces;
    for (size_t i = 0; i < expected.size(); i += N) {
        Face face;
        for (size_t j = 0; j < N; ++j) {
            face[j] = expected[i + j];
        }
        auto beg = std::min_element(face.begin(), face.end());
        std::rotate(face.begin(), beg, face.end());
        expected_faces.push_back(std::move(face));
    }
    std::sort(expected_faces.begin(), expected_faces.end());

    return test_faces == expected_faces;
}

// Test whether the referred positions of two index arrays are identical
template<int N, typename T1, typename T2>
static bool _referred_eq(const std::vector<T1>& test_pos,
                         const std::vector<T2>& test_idx,
                         const std::vector<T1>& expected_pos,
                         const std::vector<T2>& expected_idx)
{
    EASSERT(test_pos.size() % 3 == 0);
    EASSERT(expected_pos.size() % 3 == 0);
    EASSERT(test_idx.size() == expected_idx.size());
    EASSERT(test_idx.size() % N == 0);
    EASSERT(expected_idx.size() % N == 0);

    using Point = std::array<T1, 3>;
    using Face = std::array<Point, N>;

    std::vector<Face> test_faces;
    for (size_t i = 0; i < test_idx.size(); i += N) {
        Face face;
        for (size_t j = 0; j < N; ++j) {
            face[j] = { { test_pos[test_idx[i + j] * 3 + 0],
                          test_pos[test_idx[i + j] * 3 + 1],
                          test_pos[test_idx[i + j] * 3 + 2] } };
        }
        auto beg = std::min_element(face.begin(), face.end());
        std::rotate(face.begin(), beg, face.end());
        test_faces.push_back(std::move(face));
    }
    std::sort(test_faces.begin(), test_faces.end());

    std::vector<Face> expected_faces;
    for (size_t i = 0; i < expected_idx.size(); i += N) {
        Face face;
        for (size_t j = 0; j < N; ++j) {
            face[j] = { { expected_pos[expected_idx[i + j] * 3 + 0],
                          expected_pos[expected_idx[i + j] * 3 + 1],
                          expected_pos[expected_idx[i + j] * 3 + 2] } };
        }
        auto beg = std::min_element(face.begin(), face.end());
        std::rotate(face.begin(), beg, face.end());
        expected_faces.push_back(std::move(face));
    }
    std::sort(expected_faces.begin(), expected_faces.end());

    return test_faces == expected_faces;
}

TEST_CASE("IO, InputFixer", "[io][inputfixer]")
{
    // one duplicate point, one unreferenced vertex
    std::vector<float> sick_tri_pos{ 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
                                     1.0f, 2.0f, 3.0f, 4.0f, 6.0f, 8.0f };
    // one duplicate face, one degenerate face
    std::vector<unsigned> sick_tri_idx{ 3, 1, 2, 3, 3, 3, 2, 3, 1, 3, 2, 1 };
    // no duplicate point, one unreferenced vertex
    std::vector<double> sick_quad_pos{ 0.0, 0.0, 0.0, 1.0,  1.0,  1.0,
                                       2.0, 2.0, 2.0, 3.0,  4.0,  5.0,
                                       6.0, 8.0, 7.0, -1.0, -3.0, 2.0 };
    // one duplicate face, one degenerate face
    std::vector<int> sick_quad_idx{ 0, 1, 2, 3, 1, 2, 3, 4, 3, 4, 1, 2 };

    SECTION("vertex duplication")
    {
        const std::vector<float> fixed_tri_pos{ 0.0f, 0.0f, 0.0f, 1.0f, 2.0f,
                                                3.0f, 4.0f, 6.0f, 8.0f };
        REQUIRE(Euclid::remove_duplicate_vertices(sick_tri_pos) == 1);
        REQUIRE(_pos_eq(sick_tri_pos, fixed_tri_pos));
    }

    SECTION("vertex duplication with indices")
    {
        const std::vector<float> fixed_tri_pos{ 0.0f, 0.0f, 0.0f, 1.0f, 2.0f,
                                                3.0f, 4.0f, 6.0f, 8.0f };
        const std::vector<unsigned> fixed_tri_idx{ 2, 0, 1, 2, 2, 2,
                                                   1, 2, 0, 2, 1, 0 };
        REQUIRE(Euclid::remove_duplicate_vertices<3>(sick_tri_pos,
                                                     sick_tri_idx) == 1);
        REQUIRE(_pos_eq(sick_tri_pos, fixed_tri_pos));
        REQUIRE(_pos_eq<3>(
            sick_tri_pos, sick_tri_idx, fixed_tri_pos, fixed_tri_idx));
    }

    SECTION("face duplication")
    {
        const std::vector<unsigned> fixed_tri_idx{ 3, 1, 2, 3, 3, 3, 3, 2, 1 };
        REQUIRE(Euclid::remove_duplicate_faces<3>(sick_tri_idx) == 1);
        REQUIRE(_idx_eq<3>(sick_tri_idx, fixed_tri_idx));

        const std::vector<int> fixed_quad_idx{ 0, 1, 2, 3, 1, 2, 3, 4 };
        REQUIRE(Euclid::remove_duplicate_faces<4>(sick_quad_idx) == 1);
        REQUIRE(_idx_eq<4>(sick_quad_idx, fixed_quad_idx));
    }

    SECTION("unreferenced vertices")
    {
        const std::vector<float> fixed_tri_pos{ 0.0f, 0.0f, 0.0f, 1.0f, 2.0f,
                                                3.0f, 4.0f, 6.0f, 8.0f };
        const std::vector<unsigned> fixed_tri_idx{ 2, 0, 1, 2, 2, 2,
                                                   1, 2, 0, 2, 1, 0 };
        REQUIRE(Euclid::remove_unreferenced_vertices<3>(sick_tri_pos,
                                                        sick_tri_idx) == 1);
        REQUIRE(_referred_eq<3>(
            sick_tri_pos, sick_tri_idx, fixed_tri_pos, fixed_tri_idx));

        const std::vector<double> fixed_quad_pos{ 0.0, 0.0, 0.0, 1.0, 1.0,
                                                  1.0, 2.0, 2.0, 2.0, 3.0,
                                                  4.0, 5.0, 6.0, 8.0, 7.0 };
        const std::vector<int> fixed_quad_idx{ 0, 1, 2, 3, 1, 2,
                                               3, 4, 3, 4, 1, 2 };
        REQUIRE(Euclid::remove_unreferenced_vertices<4>(sick_quad_pos,
                                                        sick_quad_idx) == 1);
        REQUIRE(_referred_eq<4>(
            sick_quad_pos, sick_quad_idx, fixed_quad_pos, fixed_quad_idx));
    }

    SECTION("degenerate faces")
    {
        const std::vector<unsigned> fixed_tri_idx{ 3, 1, 2, 3, 2, 1, 2, 3, 1 };
        REQUIRE(Euclid::remove_degenerate_faces<3>(sick_tri_pos,
                                                   sick_tri_idx) == 1);
        REQUIRE(_idx_eq<3>(fixed_tri_idx, sick_tri_idx));

        std::vector<int> fixed_quad_idx{ 3, 4, 1, 2, 1, 2, 3, 4 };
        REQUIRE(Euclid::remove_degenerate_faces<4>(sick_quad_pos,
                                                   sick_quad_idx) == 1);
        REQUIRE(sick_quad_idx == fixed_quad_idx);
    }

    SECTION("all deficiencies")
    {
        const std::vector<float> fixed_tri_pos{ 0.0f, 0.0f, 0.0f, 4.0f, 6.0f,
                                                8.0f, 1.0f, 2.0f, 3.0f };
        const std::vector<unsigned> fixed_tri_idx{ 1, 0, 2, 1, 2, 0 };
        Euclid::remove_duplicate_vertices<3>(sick_tri_pos, sick_tri_idx);
        Euclid::remove_duplicate_faces<3>(sick_tri_idx);
        Euclid::remove_unreferenced_vertices<3>(sick_tri_pos, sick_tri_idx);
        Euclid::remove_degenerate_faces<3>(sick_tri_pos, sick_tri_idx);
        REQUIRE(_pos_eq<3>(
            sick_tri_pos, sick_tri_idx, fixed_tri_pos, fixed_tri_idx));
        REQUIRE(_idx_eq<3>(sick_tri_idx, fixed_tri_idx));

        const std::vector<double> fixed_quad_pos{
            1.0, 1.0, 1.0, 2.0, 2.0, 2.0, 3.0, 4.0, 5.0, 6.0, 8.0, 7.0
        };
        const std::vector<int> fixed_quad_idx{ 0, 1, 2, 3 };
        Euclid::remove_duplicate_vertices<4>(sick_quad_pos, sick_quad_idx);
        Euclid::remove_duplicate_faces<4>(sick_quad_idx);
        Euclid::remove_degenerate_faces<4>(sick_quad_pos, sick_quad_idx);
        Euclid::remove_unreferenced_vertices<4>(sick_quad_pos, sick_quad_idx);
        REQUIRE(_pos_eq<4>(
            sick_quad_pos, sick_quad_idx, fixed_quad_pos, fixed_quad_idx));
        REQUIRE(_idx_eq<4>(sick_quad_idx, fixed_quad_idx));
    }

    SECTION("real world examples")
    {
        std::string file_name(DATA_DIR);
        file_name.append("chair.off");
        std::vector<double> positions;
        std::vector<int> indices;
        Euclid::read_off<3>(file_name, positions, nullptr, &indices, nullptr);

        Euclid::remove_duplicate_vertices<3>(positions, indices);
        Euclid::remove_duplicate_faces<3>(indices);
        Euclid::remove_degenerate_faces<3>(positions, indices);
        Euclid::remove_unreferenced_vertices<3>(positions, indices);

        std::string out_name(TMP_DIR);
        out_name.append("chair_fixed.off");
        Euclid::write_off<3>(out_name, positions, nullptr, &indices, nullptr);
    }
}
