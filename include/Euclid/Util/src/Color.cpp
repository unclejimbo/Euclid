#include <algorithm>
#include <random>

namespace Euclid
{

template<typename T1, typename T2>
void colormap(const igl::ColorMapType& cm,
              const std::vector<T1>& values,
              std::vector<T2>& colors,
              bool to255,
              bool alpha,
              bool inverse)
{
    colors.clear();
    if (!alpha) {
        colors.reserve(values.size() * 3);
    }
    else {
        colors.reserve(values.size() * 4);
    }
    auto [vmin, vmax] = std::minmax_element(values.begin(), values.end());
    auto vdenom = static_cast<T1>(1.0) / (*vmax - *vmin);
    for (auto v : values) {
        T1 f;
        if (!inverse) {
            f = (v - *vmin) * vdenom;
        }
        else {
            f = (*vmax - v) * vdenom;
        }
        T1 r, g, b;
        igl::colormap(cm, f, r, g, b);
        if (!to255) {
            colors.push_back(static_cast<T2>(r));
            colors.push_back(static_cast<T2>(g));
            colors.push_back(static_cast<T2>(b));
            if (alpha) {
                colors.push_back(static_cast<T2>(1));
            }
        }
        else {
            colors.push_back(static_cast<T2>(255 * r));
            colors.push_back(static_cast<T2>(255 * g));
            colors.push_back(static_cast<T2>(255 * b));
            if (alpha) {
                colors.push_back(static_cast<T2>(255));
            }
        }
    }
}

template<typename T>
void rnd_colors(unsigned n, std::vector<T>& colors, bool to255, bool alpha)
{
    colors.clear();
    if (!alpha) {
        colors.reserve(n * 3);
    }
    else {
        colors.reserve(n * 4);
    }

    std::random_device rd;
    std::minstd_rand rd_gen(rd());
    if (!to255) {
        std::uniform_real_distribution rd_number(0.0, 1.0);
        for (size_t i = 0; i < n; ++i) {
            auto r = rd_number(rd_gen);
            auto g = rd_number(rd_gen);
            auto b = rd_number(rd_gen);
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);
            if (alpha) {
                colors.push_back(static_cast<T>(1));
            }
        }
    }
    else {
        std::uniform_int_distribution rd_number(0, 255);
        for (size_t i = 0; i < n; ++i) {
            auto r = rd_number(rd_gen);
            auto g = rd_number(rd_gen);
            auto b = rd_number(rd_gen);
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);
            if (alpha) {
                colors.push_back(static_cast<T>(255));
            }
        }
    }
}

} // namespace Euclid
