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
#include <Euclid/Util/Assert.h>

namespace Euclid
{

namespace _impl
{

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
int remove_duplicate_vertices(std::vector<T>& positions)
{
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Input position size is not divisible by 3");
    }
    using Point = std::array<T, 3>;

    // Find duplicate points
    std::vector<size_t> marks;
    std::unordered_set<Point, boost::hash<Point>> unique_points;
    for (size_t i = 0; i < positions.size(); i += 3) {
        Point p{ { positions[i], positions[i + 1], positions[i + 2] } };
        auto[dummy, is_unique] = unique_points.insert(std::move(p));
        if (!is_unique) {
            marks.push_back(i);
        }
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

template<typename T1, typename T2>
int remove_duplicate_vertices(std::vector<T1>& positions,
                              std::vector<T2>& indices)
{
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Input position size is not divisible by 3");
    }
    using Point = std::array<T1, 3>;

    // Find duplicate points and correct their indices
    std::map<size_t, size_t> marks; // marks[duplicate] = first
    std::unordered_map<Point, size_t, boost::hash<Point>>
        unique_points; // unique_points[point] = index
    for (size_t i = 0; i < positions.size(); i += 3) {
        Point p{ { positions[i], positions[i + 1], positions[i + 2] } };
        auto[iter, is_unique] = unique_points.try_emplace(std::move(p), i);
        if (!is_unique) {
            marks[i] = iter->second;
        }
    }

    size_t idx = positions.size() - 3;
    std::vector<size_t> index_swap(positions.size() / 3);
    std::iota(index_swap.begin(), index_swap.end(), 0);
    for (auto iter = marks.rbegin(); iter != marks.rend(); ++iter, idx -= 3) {
        // Swap to-be-removed positions to the back
        auto pos = iter->first;
        positions[pos] = positions[idx];
        positions[pos + 1] = positions[idx + 1];
        positions[pos + 2] = positions[idx + 2];

        // Swap index accordingly
        std::swap(index_swap[pos / 3], index_swap[idx / 3]);
    }

    // Fix index map
    std::unordered_map<size_t, size_t> index_map;
    for (size_t i = 0; i < index_swap.size() - marks.size(); ++i) {
        index_map[index_swap[i]] = i;
    }
    for (size_t i = index_swap.size() - marks.size(); i < index_swap.size();
         ++i) {
        index_map[index_swap[i]] = marks[index_swap[i] * 3] / 3;
    }

    // Now fix indices
    for (auto& i : indices) {
        EASSERT(index_map[i] < positions.size() / 3 - marks.size());
        i = static_cast<T2>(index_map[i]);
    }

    // Now drop the end
    positions.erase(positions.begin() + idx + 3, positions.end());
    positions.shrink_to_fit();

    return marks.size();
}

template<int N, typename T>
int remove_duplicate_faces(std::vector<T>& indices)
{
    if (indices.size() % N != 0) {
        std::string err_str("Input index size is not divisible by ");
        err_str.append(std::to_string(N));
        throw(err_str);
    }
    using Face = std::array<T, N>;

    std::unordered_set<Face, boost::hash<Face>> unique_faces;
    std::vector<T> marks;
    for (size_t i = 0; i < indices.size(); i += N) {
        Face f;
        for (size_t j = 0; j < N; ++j) {
            f[j] = indices[i + j];
        }
        auto cf = _impl::to_canonical<T, N>(f);
        auto[iter, is_unique] = unique_faces.insert(std::move(cf));
        if (!is_unique) {
            marks.push_back(i);
        }
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
int remove_unreferenced_vertices(std::vector<T1>& positions,
                                 std::vector<T2>& indices)
{
    if (positions.size() % 3 != 0) {
        throw std::runtime_error("Input position size is not divisible by 3");
    }
    if (indices.size() % N != 0) {
        std::string err_str("Input index size is not divisible by ");
        err_str.append(std::to_string(N));
        throw(err_str);
    }

    std::vector<int> ref_count(positions.size() / N, 0);
    for (auto i : indices) {
        ++ref_count[i];
    }

    size_t idx = ref_count.size() - 1;
    std::vector<T2> index_swap(ref_count.size());
    std::iota(index_swap.begin(), index_swap.end(), 0);
    for (int i = static_cast<int>(ref_count.size()) - 1; i >= 0; --i) {
        if (ref_count[i] == 0) {
            positions[i * N] = positions[idx * N];
            positions[i * N + 1] = positions[idx * N + 1];
            positions[i * N + 2] = positions[idx * N + 2];
            std::swap(index_swap[i], index_swap[idx--]);
        }
    }

    std::unordered_map<size_t, T2> index_map;
    for (size_t i = 0; i <= idx; ++i) {
        index_map[index_swap[i]] = i;
    }

    for (auto& i : indices) {
        i = index_map[i];
    }

    positions.erase(positions.begin() + idx * N + N, positions.end());
    positions.shrink_to_fit();

    return ref_count.size() - idx - 1;
}

} // namespace Euclid
