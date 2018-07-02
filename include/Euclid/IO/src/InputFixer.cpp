#include <algorithm>
#include <array>
#include <exception>
#include <map>
#include <numeric>
#include <string>
#include <tuple>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <boost/functional/hash.hpp>
#include <CGAL/Simple_cartesian.h>
#include <CGAL/Kernel/global_functions.h>
#include <Euclid/Util/Assert.h>

namespace Euclid
{

namespace _impl
{

/** Convert face index order to a canonical form such that
 *  the first index is the smallest.*/
template<typename T, int N>
std::array<T, N> to_canonical(const std::array<T, N>& face)
{
    auto iter = std::min_element(face.begin(), face.end());
    std::array<T, N> canonical;
    for (size_t i = 0; i < N; ++i) {
        canonical[i] = *iter++;
        if (iter == face.end()) iter = face.begin();
    }
    return canonical;
}

} // namespace _impl

template<typename T>
size_t remove_duplicate_vertices(std::vector<T>& positions)
{
    if (positions.empty()) {
        EWARNING("positions is empty.");
        return 0;
    }
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Input position size is not divisible by 3");
    }
    using Point = std::array<T, 3>;

    // Find duplicate points
    std::vector<size_t> marks;
    std::unordered_set<Point, boost::hash<Point>> unique_points;
    for (size_t i = 0; i < positions.size(); i += 3) {
        Point p{ { positions[i], positions[i + 1], positions[i + 2] } };
        auto [dummy, is_unique] = unique_points.insert(std::move(p));
        if (!is_unique) { marks.push_back(i); }
    }

    // Move elements in the back into slots to be removed
    size_t idx = positions.size() - 3;
    for (auto iter = marks.rbegin(); iter != marks.rend(); ++iter) {
        positions[*iter] = positions[idx];
        positions[*iter + 1] = positions[idx + 1];
        positions[*iter + 2] = positions[idx + 2];
        idx -= 3;
    }

    // Now drop the end
    positions.erase(positions.begin() + idx + 3, positions.end());
    positions.shrink_to_fit();

    return marks.size();
}

template<int N, typename T1, typename T2>
size_t remove_duplicate_vertices(std::vector<T1>& positions,
                                 std::vector<T2>& indices)
{
    static_assert(N >= 3);
    if (positions.empty()) {
        EWARNING("positions is empty.");
        return 0;
    }
    if (indices.empty()) { remove_duplicate_vertices(positions); }
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Input position size is not divisible by 3");
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input index size is not divisible by ");
        err_str.append(std::to_string(N));
        throw(err_str);
    }
    if (*std::max_element(indices.begin(), indices.end()) >=
        static_cast<T2>(positions.size() / 3)) {
        throw std::runtime_error(
            "Input indices is out of range of the position vector");
    }
    using Point = std::array<T1, 3>;

    // marks[duplicate] = first
    // duplicate and first are indices into the position vector
    std::map<size_t, size_t> marks;

    // unique_points[point] = index
    // index refers to the position vector
    std::unordered_map<Point, size_t, boost::hash<Point>> unique_points;

    // Map all the duplicate positions to their first appearance
    for (size_t i = 0; i < positions.size(); i += 3) {
        Point p{ { positions[i], positions[i + 1], positions[i + 2] } };
        auto [iter, is_unique] = unique_points.try_emplace(std::move(p), i);
        if (!is_unique) {
            EASSERT(p == iter->first);
            marks[i] = iter->second;
        }
    }

    size_t idx = positions.size() - 3;

    // values in index_swap refers to the index of a Point
    std::vector<size_t> index_swap(positions.size() / 3);
    std::iota(index_swap.begin(), index_swap.end(), 0);

    for (auto iter = marks.rbegin(); iter != marks.rend(); ++iter, idx -= 3) {
        // Replace positions in marks with positions in the back of the vector
        auto pos = iter->first;
        positions[pos] = positions[idx];
        positions[pos + 1] = positions[idx + 1];
        positions[pos + 2] = positions[idx + 2];

        // Swap indices accordingly
        std::swap(index_swap[pos / 3], index_swap[idx / 3]);
    }

    // index_map[old] = new
    // old and new are indices of Point
    std::vector<size_t> index_map(positions.size() / 3);

    for (size_t i = 0; i < index_swap.size() - marks.size(); ++i) {
        EASSERT(marks.find(index_swap[i] * 3) == marks.end());
        index_map[index_swap[i]] = i;
    }
    for (size_t i = index_swap.size() - marks.size(); i < index_swap.size();
         ++i) {
        EASSERT(marks.find(index_swap[i] * 3) != marks.end());
        index_map[index_swap[i]] = index_map[marks[index_swap[i] * 3] / 3];
    }

    // Now fix indices
    for (auto& i : indices) {
        auto new_idx = index_map[i];
        EASSERT(index_map[i] < (positions.size() / 3 - marks.size()));
        i = static_cast<T2>(index_map[i]);
    }

    // Now erase garbage values at the tail of the vector
    positions.erase(positions.begin() + idx + 3, positions.end());
    positions.shrink_to_fit();

    return marks.size();
}

template<int N, typename T>
size_t remove_duplicate_faces(std::vector<T>& indices)
{
    static_assert(N >= 3);
    if (indices.empty()) {
        EWARNING("indices is empty.");
        return 0;
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input index size is not divisible by ");
        err_str.append(std::to_string(N));
        throw(err_str);
    }
    using Face = std::array<T, N>;

    std::unordered_set<Face, boost::hash<Face>> unique_faces;
    std::vector<size_t> marks;
    for (size_t i = 0; i < indices.size(); i += N) {
        Face f;
        for (size_t j = 0; j < N; ++j) {
            f[j] = indices[i + j];
        }
        auto cf = _impl::to_canonical<T, N>(f);
        auto [iter, is_unique] = unique_faces.insert(std::move(cf));
        if (!is_unique) { marks.push_back(i); }
    }

    size_t idx = indices.size() - N;
    for (auto iter = marks.rbegin(); iter != marks.rend(); ++iter, idx -= N) {
        for (size_t i = 0; i < N; ++i) {
            indices[*iter + i] = indices[idx + i];
        }
    }

    indices.erase(indices.begin() + idx + N, indices.end());
    indices.shrink_to_fit();

    return marks.size();
}

template<int N, typename T1, typename T2>
size_t remove_unreferenced_vertices(std::vector<T1>& positions,
                                    std::vector<T2>& indices)
{
    static_assert(N >= 3);
    if (positions.empty()) {
        EWARNING("positions is empty.");
        return 0;
    }
    if (indices.empty()) {
        EWARNING("indices is empty.");
        return 0;
    }
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Input position size is not divisible by 3");
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input index size is not divisible by ");
        err_str.append(std::to_string(N));
        throw(err_str);
    }
    if (*std::max_element(indices.begin(), indices.end()) >=
        static_cast<T2>(positions.size() / 3)) {
        throw std::runtime_error(
            "Input indices is out of range of the position vector");
    }

    std::vector<int> ref_count(positions.size() / 3, 0);
    for (auto i : indices) {
        ++ref_count[i];
    }

    size_t idx = ref_count.size() - 1;
    std::vector<T2> index_swap(ref_count.size());
    std::iota(index_swap.begin(), index_swap.end(), 0);
    for (int i = static_cast<int>(ref_count.size()) - 1; i >= 0; --i) {
        if (ref_count[i] == 0) {
            for (auto j = 0; j < 3; ++j) {
                positions[i * 3 + j] = positions[idx * 3 + j];
            }
            std::swap(index_swap[i], index_swap[idx--]);
        }
    }

    std::unordered_map<T2, size_t> index_map;
    for (size_t i = 0; i <= idx; ++i) {
        index_map[index_swap[i]] = i;
    }

    for (auto& i : indices) {
        i = static_cast<T2>(index_map[i]);
    }

    positions.erase(positions.begin() + (idx + 1) * 3, positions.end());
    positions.shrink_to_fit();

    return ref_count.size() - idx - 1;
}

template<int N, typename T1, typename T2>
size_t remove_degenerate_faces(const std::vector<T1>& positions,
                               std::vector<T2>& indices)
{
    static_assert(N >= 3);
    if (positions.empty()) {
        EWARNING("position is empty.");
        return 0;
    }
    if (indices.empty()) {
        EWARNING("indices is empty.");
        return 0;
    }
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Input position size is not divisible by 3");
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input index size is not divisible by ");
        err_str.append(std::to_string(N));
        throw(err_str);
    }
    if (*std::max_element(indices.begin(), indices.end()) >=
        static_cast<T2>(positions.size() / 3)) {
        throw std::runtime_error(
            "Input indices is out of range of the position vector");
    }
    using Kernel = CGAL::Simple_cartesian<T1>;
    using Point_3 = typename Kernel::Point_3;

    std::vector<size_t> marks;
    for (size_t i = 0; i < indices.size(); i += N) {
        for (size_t j = 0; j < N - 1; ++j) {
            auto p0 = indices[i + j] * 3;
            auto p1 = indices[i + j + 1] * 3;
            auto p2 = (j == N - 2 ? indices[i] : indices[i + j + 2]) * 3;
            auto x0 = positions[p0];
            auto y0 = positions[p0 + 1];
            auto z0 = positions[p0 + 2];
            auto x1 = positions[p1];
            auto y1 = positions[p1 + 1];
            auto z1 = positions[p1 + 2];
            auto x2 = positions[p2];
            auto y2 = positions[p2 + 1];
            auto z2 = positions[p2 + 2];
            if (CGAL::collinear<Kernel>(Point_3{ x0, y0, z0 },
                                        Point_3{ x1, y1, z1 },
                                        Point_3{ x2, y2, z2 })) {
                marks.push_back(i);
                break;
            }
        }
    }

    auto idx = indices.size() - N;
    for (auto iter = marks.rbegin(); iter != marks.rend(); ++iter, idx -= N) {
        for (auto i = 0; i < N; ++i) {
            indices[*iter + i] = indices[idx + i];
        }
    }

    indices.erase(indices.begin() + idx + N, indices.end());
    indices.shrink_to_fit();

    return marks.size();
}

} // namespace Euclid
