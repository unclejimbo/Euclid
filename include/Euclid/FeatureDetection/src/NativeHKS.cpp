#include <Euclid/MeshUtil/MeshHelpers.h>

namespace Euclid
{

template<typename Mesh, typename Derived>
std::vector<vertex_t<Mesh>> native_hks_features(
    const Mesh& mesh,
    const Eigen::ArrayBase<Derived>& hks,
    int tscale)
{
    auto vimap = get(boost::vertex_index, mesh);
    std::vector<vertex_t<Mesh>> points;
    for (auto v : vertices(mesh)) {
        bool local_maximum = true;
        auto neighbors = nring_vertices(v, mesh, 2);
        for (auto vi : neighbors) {
            if (hks(tscale, get(vimap, vi)) > hks(tscale, get(vimap, v))) {
                local_maximum = false;
                break;
            }
        }
        if (local_maximum) {
            points.push_back(v);
        }
    }
    return points;
}

} // namespace Euclid
