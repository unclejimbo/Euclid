#include <algorithm>
#include <limits>
#include <string>
#include <random>

#include <boost/math/constants/constants.hpp>
#include <Euclid/Util/Assert.h>

namespace Euclid
{

namespace _impl
{

inline void mask_filter(const RTCFilterFunctionNArguments* args)
{
    EASSERT(args->N == 1);
    auto mask = reinterpret_cast<uint8_t*>(args->geometryUserPtr);
    EASSERT(mask != nullptr);

    auto primid = RTCHitN_primID(args->hit, 1, 0);
    if (mask[primid] != 1) { args->valid[0] = 0; }
}

} // namespace _impl

inline Camera::Camera(const Vec3& position, const Vec3& focus, const Vec3& up)
{
    pos = position;
    dir = (position - focus).normalized();
    u = up.cross(dir).normalized();
    v = dir.cross(u);
}

inline void Camera::lookat(const Vec3& position,
                           const Vec3& focus,
                           const Vec3& up)
{
    pos = position;
    dir = (position - focus).normalized();
    u = up.cross(dir).normalized();
    v = dir.cross(u);
}

inline PerspectiveCamera::PerspectiveCamera(const Vec3& position,
                                            const Vec3& focus,
                                            const Vec3& up,
                                            float vfov,
                                            float aspect)
    : Camera(position, focus, up)
{
    auto fov = vfov * boost::math::float_constants::degree;
    film.height = 2.0f * std::tan(fov * 0.5f);
    film.width = aspect * film.height;
}

inline PerspectiveCamera::PerspectiveCamera(const Vec3& position,
                                            const Vec3& focus,
                                            const Vec3& up,
                                            float vfov,
                                            unsigned width,
                                            unsigned height)
    : Camera(position, focus, up)
{
    auto fov = vfov * boost::math::float_constants::degree;
    auto aspect = static_cast<float>(width) / height;
    film.height = 2.0f * std::tan(fov * 0.5f);
    film.width = aspect * film.height;
}

inline void PerspectiveCamera::set_aspect(float aspect)
{
    film.width = aspect * film.height;
}

inline void PerspectiveCamera::set_aspect(unsigned width, unsigned height)
{
    auto aspect = static_cast<float>(width) / height;
    film.width = aspect * film.height;
}

inline void PerspectiveCamera::set_fov(float vfov)
{
    auto fov = vfov * boost::math::float_constants::degree;
    auto aspect = film.width / film.height;
    film.height = 2.0f * std::tan(fov * 0.5f);
    film.width = aspect * film.height;
}

inline RTCRayHit PerspectiveCamera::gen_ray(float s,
                                            float t,
                                            float near,
                                            float far) const
{
    RTCRayHit rayhit;
    Eigen::Vector3f view =
        -dir + (s - 0.5f) * film.width * u + (t - 0.5f) * film.height * v;
    EASSERT(view.dot(dir) < 0.0f);

    rayhit.ray.org_x = pos(0);
    rayhit.ray.org_y = pos(1);
    rayhit.ray.org_z = pos(2);
    rayhit.ray.dir_x = view(0);
    rayhit.ray.dir_y = view(1);
    rayhit.ray.dir_z = view(2);
    rayhit.ray.tnear = near;
    rayhit.ray.tfar = far;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    return rayhit;
}

inline OrthogonalCamera::OrthogonalCamera(const Vec3& position,
                                          const Vec3& focus,
                                          const Vec3& up,
                                          float width,
                                          float height)
    : Camera(position, focus, up)
{
    film.width = width;
    film.height = height;
}

inline void OrthogonalCamera::set_extent(float width, float height)
{
    film.width = width;
    film.height = height;
}

inline RTCRayHit OrthogonalCamera::gen_ray(float s,
                                           float t,
                                           float near,
                                           float far) const
{
    RTCRayHit rayhit;
    Eigen::Vector3f origin =
        pos + (s - 0.5f) * film.width * u + (t - 0.5f) * film.height * v;

    rayhit.ray.org_x = origin(0);
    rayhit.ray.org_y = origin(1);
    rayhit.ray.org_z = origin(2);
    rayhit.ray.dir_x = -dir(0);
    rayhit.ray.dir_y = -dir(1);
    rayhit.ray.dir_z = -dir(2);
    rayhit.ray.tnear = near;
    rayhit.ray.tfar = far;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    return rayhit;
}

inline RayTracer::RayTracer(int threads)
{
    std::string cfg("threads=");
    cfg.append(std::to_string(threads));
    _device = rtcNewDevice(cfg.c_str());
    if (!_device) {
        auto err = rtcGetDeviceError(_device);
        std::string err_str("Embree device creation error: ");
        err_str.append(std::to_string(err));
        throw std::runtime_error(err_str);
    }

    _scene = rtcNewScene(_device);
    if (!_scene) {
        auto err = rtcGetDeviceError(_device);
        std::string err_str("Embree scene creation error: ");
        err_str.append(std::to_string(err));
        throw std::runtime_error(err_str);
    }

    _material.ambient << 0.1f, 0.1f, 0.1f;
    _material.diffuse << 0.7f, 0.7f, 0.7f;
}

inline RayTracer::~RayTracer()
{
    release_buffers();
    rtcReleaseScene(_scene);
    rtcReleaseDevice(_device);
}

// TODO: add generic type support
inline void RayTracer::attach_geometry_buffers(
    const std::vector<float>& positions,
    const std::vector<unsigned>& indices,
    RTCGeometryType type)
{
    if (positions.empty() || indices.empty()) {
        EWARNING("Input geometry is empty.");
        return;
    }
    if (positions.size() % 3 != 1) {
        throw std::invalid_argument("The last element of the positions buffer "
                                    "is not padded to 16 bytes, add "
                                    "one more 0.0f to your positions buffer.");
    }
    if (!(type == RTC_GEOMETRY_TYPE_TRIANGLE ||
          type == RTC_GEOMETRY_TYPE_QUAD)) {
        throw std::invalid_argument(
            "Input type must be RTC_GEOMETRY_TYPE_TRIANGLE or "
            "RTC_GEOMETRY_TYPE_QUAD.");
    }
    if (type == RTC_GEOMETRY_TYPE_TRIANGLE && indices.size() % 3 != 0) {
        throw std::invalid_argument("Size of input indices is not divisible by "
                                    "3, thus not a valid triangle mesh.");
    }
    if (type == RTC_GEOMETRY_TYPE_QUAD && indices.size() % 4 != 0) {
        throw std::invalid_argument("Size of input indices is not divisible by "
                                    "4, thus not a valid quad mesh.");
    }

    release_buffers();

    _geometry = rtcNewGeometry(_device, type);

    rtcSetSharedGeometryBuffer(_geometry,
                               RTC_BUFFER_TYPE_VERTEX,
                               0,
                               RTC_FORMAT_FLOAT3,
                               positions.data(),
                               0,
                               3 * sizeof(float),
                               positions.size() / 3);

    if (type == RTC_GEOMETRY_TYPE_TRIANGLE) {
        rtcSetSharedGeometryBuffer(_geometry,
                                   RTC_BUFFER_TYPE_INDEX,
                                   0,
                                   RTC_FORMAT_UINT3,
                                   indices.data(),
                                   0,
                                   3 * sizeof(unsigned),
                                   indices.size() / 3);
    }
    else { // type == RTC_GEOMETRY_TYPE_QUAD
        rtcSetSharedGeometryBuffer(_geometry,
                                   RTC_BUFFER_TYPE_INDEX,
                                   0,
                                   RTC_FORMAT_UINT4,
                                   indices.data(),
                                   0,
                                   4 * sizeof(unsigned),
                                   indices.size() / 4);
    }

    rtcCommitGeometry(_geometry);
    _geom_id = rtcAttachGeometry(_scene, _geometry);
    rtcCommitScene(_scene);
}

inline void RayTracer::attach_face_color_buffer(const float* colors)
{
    _face_colors = colors;
}

inline void RayTracer::attach_face_mask_buffer(const uint8_t* mask)
{
    if (mask != nullptr && _face_mask == nullptr) {
        rtcSetGeometryIntersectFilterFunction(_geometry, _impl::mask_filter);
    }
    if (mask == nullptr && _face_mask != nullptr) {
        rtcSetGeometryIntersectFilterFunction(_geometry, nullptr);
    }
    rtcSetGeometryUserData(_geometry, const_cast<uint8_t*>(mask));
    rtcCommitGeometry(_geometry);
    _face_mask = mask;
}

inline void RayTracer::release_buffers()
{
    if (_geom_id != -1) {
        rtcDetachGeometry(_scene, _geom_id);
        _geom_id = -1;
        _face_colors = nullptr;
        _face_mask = nullptr;
        rtcReleaseGeometry(_geometry);
    }
}

inline void RayTracer::set_material(const Material& material)
{
    _material = material;
}

inline void RayTracer::set_background(
    const Eigen::Ref<const Eigen::Array3f>& color)
{
    _background = color;
}

inline void RayTracer::set_background(float r, float g, float b)
{
    _background << r, g, b;
}

inline void RayTracer::enable_light(bool on)
{
    _lighting = on;
}

inline void RayTracer::render_shaded(uint8_t* pixels,
                                     const Camera& camera,
                                     int width,
                                     int height,
                                     bool interleaved)
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);

#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Eigen::Array3f color(0.0f, 0.0f, 0.0f);
            auto u = static_cast<float>(x) / width;
            auto v = static_cast<float>(y) / height;
            auto rayhit = camera.gen_ray(u, v);
            rtcIntersect1(_scene, &context, &rayhit);

            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                Eigen::Vector3f normal = Eigen::Vector3f(rayhit.hit.Ng_x,
                                                         rayhit.hit.Ng_y,
                                                         rayhit.hit.Ng_z)
                                             .normalized();
                // Point light at the view position
                Eigen::Vector3f lightdir = Eigen::Vector3f(rayhit.ray.dir_x,
                                                           rayhit.ray.dir_y,
                                                           rayhit.ray.dir_z)
                                               .normalized();

                Eigen::Array3f ambient = _material.ambient;
                Eigen::Array3f diffuse;
                if (_face_colors != nullptr) {
                    diffuse << _face_colors[3 * rayhit.hit.primID],
                        _face_colors[3 * rayhit.hit.primID + 1],
                        _face_colors[3 * rayhit.hit.primID + 2];
                }
                else {
                    diffuse = _material.diffuse;
                }
                if (_lighting) { diffuse *= std::abs(normal.dot(-lightdir)); }
                color += ambient + diffuse;
            }
            else {
                color += _background;
            }
            color(0) = std::min(color(0), 1.0f);
            color(1) = std::min(color(1), 1.0f);
            color(2) = std::min(color(2), 1.0f);
            color *= 255;
            auto r = static_cast<uint8_t>(color(0));
            auto g = static_cast<uint8_t>(color(1));
            auto b = static_cast<uint8_t>(color(2));
            if (interleaved) {
                pixels[3 * ((height - y - 1) * width + x) + 0] = r;
                pixels[3 * ((height - y - 1) * width + x) + 1] = g;
                pixels[3 * ((height - y - 1) * width + x) + 2] = b;
            }
            else {
                pixels[(height - y - 1) * width + x] = r;
                pixels[width * height + (height - y - 1) * width + x] = g;
                pixels[2 * width * height + (height - y - 1) * width + x] = b;
            }
        }
    }
}

inline void RayTracer::render_shaded(uint8_t* pixels,
                                     const Camera& camera,
                                     int width,
                                     int height,
                                     int samples,
                                     bool interleaved)
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    std::random_device rd;
    std::minstd_rand rd_gen(rd());
    std::uniform_real_distribution<> rd_number(0.0f, 1.0f);
    const float rcpr_samples = 1.0f / samples;

#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            Eigen::Array3f color(0.0f, 0.0f, 0.0f);
            for (int s = 0; s < samples; ++s) {
                auto u = static_cast<float>(x + rd_number(rd_gen)) / width;
                auto v = static_cast<float>(y + rd_number(rd_gen)) / height;
                auto rayhit = camera.gen_ray(u, v);
                rtcIntersect1(_scene, &context, &rayhit);

                if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                    Eigen::Vector3f normal = Eigen::Vector3f(rayhit.hit.Ng_x,
                                                             rayhit.hit.Ng_y,
                                                             rayhit.hit.Ng_z)
                                                 .normalized();
                    // Point light at the view position
                    Eigen::Vector3f lightdir = Eigen::Vector3f(rayhit.ray.dir_x,
                                                               rayhit.ray.dir_y,
                                                               rayhit.ray.dir_z)
                                                   .normalized();

                    Eigen::Array3f ambient = _material.ambient;
                    Eigen::Array3f diffuse;
                    if (_face_colors != nullptr) {
                        diffuse << _face_colors[3 * rayhit.hit.primID],
                            _face_colors[3 * rayhit.hit.primID + 1],
                            _face_colors[3 * rayhit.hit.primID + 2];
                    }
                    else {
                        diffuse = _material.diffuse;
                    }
                    if (_lighting) {
                        diffuse *= std::abs(normal.dot(-lightdir));
                    }
                    color += ambient + diffuse;
                }
                else {
                    color += _background;
                }
            }
            color *= rcpr_samples;
            color(0) = std::min(color(0), 1.0f);
            color(1) = std::min(color(1), 1.0f);
            color(2) = std::min(color(2), 1.0f);
            color *= 255;
            auto r = static_cast<uint8_t>(color(0));
            auto g = static_cast<uint8_t>(color(1));
            auto b = static_cast<uint8_t>(color(2));
            if (interleaved) {
                pixels[3 * ((height - y - 1) * width + x) + 0] = r;
                pixels[3 * ((height - y - 1) * width + x) + 1] = g;
                pixels[3 * ((height - y - 1) * width + x) + 2] = b;
            }
            else {
                pixels[(height - y - 1) * width + x] = r;
                pixels[width * height + (height - y - 1) * width + x] = g;
                pixels[2 * width * height + (height - y - 1) * width + x] = b;
            }
        }
    }
}

inline void RayTracer::render_depth(uint8_t* pixels,
                                    const Camera& camera,
                                    int width,
                                    int height)
{
    std::vector<float> depths(width * height, -1.0f);
    float min_depth = std::numeric_limits<float>::max();
    float max_depth = -1.0f;
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto u = static_cast<float>(x) / width;
            auto v = static_cast<float>(y) / height;
            auto rayhit = camera.gen_ray(u, v);
            rtcIntersect1(_scene, &context, &rayhit);
            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                auto depth = rayhit.ray.tfar * camera.dir.norm();
                depths[(height - y - 1) * width + x] = depth;
                if (depth < min_depth) min_depth = depth;
                if (depth > max_depth) max_depth = depth;
            }
        }
    }
    float denom = 1.0f / (max_depth - min_depth);
    for (size_t i = 0; i < depths.size(); ++i) {
        if (depths[i] == -1.0f) { pixels[i] = 0; }
        else {
            pixels[i] =
                static_cast<uint8_t>((max_depth - depths[i]) * denom * 255);
        }
    }
}

inline void RayTracer::render_depth(float* values,
                                    const Camera& camera,
                                    int width,
                                    int height)
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto u = static_cast<float>(x) / width;
            auto v = static_cast<float>(y) / height;
            auto rayhit = camera.gen_ray(u, v);
            rtcIntersect1(_scene, &context, &rayhit);
            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                auto depth = rayhit.ray.tfar * camera.dir.norm();
                values[(height - y - 1) * width + x] = depth;
            }
            else {
                values[(height - y - 1) * width + x] = -1.0f;
            }
        }
    }
}

inline void RayTracer::render_silhouette(uint8_t* pixels,
                                         const Camera& camera,
                                         int width,
                                         int height)
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto u = static_cast<float>(x) / width;
            auto v = static_cast<float>(y) / height;
            auto rayhit = camera.gen_ray(u, v);
            rtcOccluded1(_scene, &context, &(rayhit.ray));
            if (rayhit.ray.tfar <= 0.0f) {
                pixels[(height - y - 1) * width + x] = 255;
            }
            else {
                pixels[(height - y - 1) * width + x] = 0;
            }
        }
    }
}

inline void RayTracer::render_index(uint8_t* pixels,
                                    const Camera& camera,
                                    int width,
                                    int height,
                                    bool interleaved)
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto u = static_cast<float>(x) / width;
            auto v = static_cast<float>(y) / height;
            auto rayhit = camera.gen_ray(u, v);
            rtcIntersect1(_scene, &context, &rayhit);
            uint32_t index;
            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                index = rayhit.hit.primID + 1;
            }
            else {
                index = 0;
            }
            uint8_t r = index & 0x000000FF;
            uint8_t g = (index >> 8) & 0x000000FF;
            uint8_t b = (index >> 16) & 0x000000FF;
            if (interleaved) {
                pixels[3 * ((height - y - 1) * width + x) + 0] = r;
                pixels[3 * ((height - y - 1) * width + x) + 1] = g;
                pixels[3 * ((height - y - 1) * width + x) + 2] = b;
            }
            else {
                pixels[(height - y - 1) * width + x] = r;
                pixels[width * height + (height - y - 1) * width + x] = g;
                pixels[2 * width * height + (height - y - 1) * width + x] = b;
            }
        }
    }
}

inline void RayTracer::render_index(uint32_t* indices,
                                    const Camera& camera,
                                    int width,
                                    int height)
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
#pragma omp parallel for schedule(dynamic)
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto u = static_cast<float>(x) / width;
            auto v = static_cast<float>(y) / height;
            auto rayhit = camera.gen_ray(u, v);
            rtcIntersect1(_scene, &context, &rayhit);
            uint32_t index;
            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                index = rayhit.hit.primID + 1;
            }
            else {
                index = 0;
            }
            indices[(height - y - 1) * width + x] = index;
        }
    }
}

} // namespace Euclid
