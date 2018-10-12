#include <cmath>
#include <tuple>
#include <unordered_map>

#include <boost/math/constants/constants.hpp>
#include <Euclid/Geometry/TriMeshGeometry.h>
#include <Euclid/Math/Vector.h>

namespace Euclid
{

template<typename Mesh>
void SpinImage<Mesh>::build(const Mesh& mesh,
                            const std::vector<Vector_3>* vnormals,
                            FT resolution)
{
    this->mesh = &mesh;

    if (vnormals != nullptr) { this->vnormals.reset(vnormals, false); }
    else {
        std::vector<Vector_3> fnormals;
        fnormals.reserve(num_faces(mesh));
        for (auto f : faces(mesh)) {
            fnormals.push_back(  Euclid::face_normal(f, mesh) );
        }

        std::vector<Vector_3>* vertex_normals = new std::vector<Vector_3>;
        vertex_normals->reserve(num_vertices(mesh));
        for (auto v : vertices(mesh)) {
            vertex_normals->push_back(Euclid::vertex_normal(v, mesh, fnormals));
        }
        this->vnormals.reset(vertex_normals, true);
    }

    if (resolution != 0.0) { this->resolution = resolution; }
    else {
        this->resolution = 0.0;
        for (auto e : edges(mesh)) {
            this->resolution += edge_length(e, mesh);
        }
        this->resolution /= static_cast<FT>(num_edges(mesh));
    }
}

template<typename Mesh>
template<typename Derived>
void SpinImage<Mesh>::compute(Eigen::ArrayBase<Derived>& spin_img,
                              float bin_scale,
                              int image_width,
                              float support_angle)
{
    auto vpmap = get(boost::vertex_point, *this->mesh);
    auto vimap = get(boost::vertex_index, *this->mesh);
    auto cos_range =
        std::cos(support_angle * boost::math::float_constants::degree);
    auto bin_size = this->resolution * static_cast<FT>(bin_scale);
    auto support_distance = bin_size * image_width;
    auto beta_max = support_distance * 0.5;
    spin_img.derived().setZero(image_width * image_width,
                               num_vertices(*this->mesh));

    for (auto vi : vertices(*this->mesh)) {
        auto ii = get(vimap, vi);
        auto pi = get(vpmap, vi);
        auto ni = (*this->vnormals)[ii];

        // Find all vertices that lie in the support and compute the spin image
        for (auto vj : vertices(*this->mesh)) {
            auto ij = get(vimap, vj);
            auto pj = get(vpmap, vj);

            if (ni * (*this->vnormals)[ij] < cos_range) { continue; }

            auto beta = ni * (pj - pi);
            auto alpha = std::sqrt((pj - pi).squared_length() - beta * beta);

            auto col = static_cast<int>(std::floor(alpha / bin_size));
            if (col > image_width - 2) { continue; }
            auto row =
                static_cast<int>(std::floor((beta_max - beta) / bin_size));
            if (row > image_width - 2 || row < 0) { continue; }

            // Bilinear interpolation
            auto a = alpha / bin_size - col;
            auto b = beta_max / bin_size - beta / bin_size - row;
            EASSERT(a <= 1.0 && a >= 0.0);
            EASSERT(b <= 1.0 && b >= 0.0);
            spin_img(row * image_width + col, ii) += (1.0f - a) * (1.0f - b);
            spin_img(row * image_width + col + 1, ii) += a * (1.0f - b);
            spin_img((row + 1) * image_width + col, ii) += (1.0f - a) * b;
            spin_img((row + 1) * image_width + col + 1, ii) += a * b;
        }
    }
}

} // namespace Euclid
