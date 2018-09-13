#include <algorithm>
#include <random>

namespace Euclid
{

template<typename T1, typename T2>
void colormap(const igl::ColorMapType& cm,
              const std::vector<T1>& values,
              std::vector<T2>& colors,
              bool to255,
              bool inverse)
{
    colors.clear();
    colors.reserve(values.size() * 3);
    auto [vmin, vmax] = std::minmax_element(values.begin(), values.end());
    auto vdenom = static_cast<T1>(1.0) / (*vmax - *vmin);
    for (auto v : values) {
        T1 f;
        if (!inverse) { f = (v - *vmin) * vdenom; }
        else {
            f = (*vmax - v) * vdenom;
        }
        T1 r, g, b;
        igl::colormap(cm, f, r, g, b);
        if (!to255) {
            colors.push_back(static_cast<T2>(r));
            colors.push_back(static_cast<T2>(g));
            colors.push_back(static_cast<T2>(b));
        }
        else {
            colors.push_back(static_cast<T2>(255 * r));
            colors.push_back(static_cast<T2>(255 * g));
            colors.push_back(static_cast<T2>(255 * b));
        }
    }
}

template<typename T>
void rnd_colors(unsigned n, std::vector<T>& colors, bool to255)
{
    colors.clear();
    colors.resize(n * 3);
    std::random_device rd;
    std::minstd_rand rd_gen(rd());
    if (!to255) {
        std::uniform_real_distribution rd_number(0.0, 1.0);
        for (auto& c : colors) {
            c = rd_number(rd_gen);
        }
    }
    else {
        std::uniform_int_distribution rd_number(0, 255);
        for (auto& c : colors) {
            c = rd_number(rd_gen);
        }
    }
}

} // namespace Euclid
