#include <algorithm>
#include <string>
#include <random>

#include <Euclid/Util/Assert.h>

namespace Euclid
{

inline Camera::Camera(const Eigen::Vector3f& position,
                      const Eigen::Vector3f& focus,
                      const Eigen::Vector3f& up)
{
    pos = position;
    dir = (position - focus).normalized();
    u = up.cross(dir).normalized();
    v = dir.cross(u);
}

inline void Camera::lookat(const Eigen::Vector3f& position,
                           const Eigen::Vector3f& focus,
                           const Eigen::Vector3f& up)
{
    pos = position;
    dir = (position - focus).normalized();
    u = up.cross(dir).normalized();
    v = dir.cross(u);
}

inline PerspectiveCamera::PerspectiveCamera(const Eigen::Vector3f& position,
                                            const Eigen::Vector3f& focus,
                                            const Eigen::Vector3f& up,
                                            float vfov,
                                            float aspect)
    : Camera(position, focus, up)
{
    auto fov = vfov * M_PI / 180.0f;
    film.height = 2.0f * std::tan(fov * 0.5f);
    film.width = aspect * film.height;
}

inline void PerspectiveCamera::set_aspect(unsigned width, unsigned height)
{
    auto aspect = static_cast<float>(width) / height;
    film.width = aspect * film.height;
}

inline void PerspectiveCamera::set_fov(float vfov)
{
    auto fov = vfov * M_PI / 180.0f;
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
        -dir + (s - 0.5f) * film.width * u + (t - 0.5f) * film.height * v - pos;
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

inline OrthogonalCamera::OrthogonalCamera(const Eigen::Vector3f& position,
                                          const Eigen::Vector3f& focus,
                                          const Eigen::Vector3f& up,
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

RayTracer::RayTracer(int threads)
{
    // Create device
    std::string cfg("threads=");
    cfg.append(std::to_string(threads));
    _device = rtcNewDevice(cfg.c_str());
    if (!_device) {
        auto err = rtcGetDeviceError(_device);
        std::string err_str("Embree device creation error: ");
        err_str.append(std::to_string(err));
        throw std::runtime_error(err_str);
    }

    // Create scene
    _scene = rtcNewScene(_device);
    if (!_scene) {
        auto err = rtcGetDeviceError(_device);
        std::string err_str("Embree scene creation error: ");
        err_str.append(std::to_string(err));
        throw std::runtime_error(err_str);
    }
}

inline RayTracer::~RayTracer()
{
    rtcReleaseScene(_scene);
    rtcReleaseDevice(_device);
}

template<typename FT, typename IT>
void RayTracer::attach_geometry(const std::vector<FT>& positions,
                                const std::vector<IT>& indices,
                                RTCGeometryType type)
{
    if (positions.empty() || indices.empty()) {
        EWARNING("Input geometry is empty.");
        return;
    }
    if (positions.size() % 3 != 0) {
        throw std::invalid_argument(
            "Size of input positions is not divisible by 3.");
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

    // Release previously allocated geometry if presents
    if (_geom_id != -1) { rtcDetachGeometry(_scene, _geom_id); }

    _geometry = rtcNewGeometry(_device, type);

    auto vertices =
        reinterpret_cast<float*>(rtcSetNewGeometryBuffer(_geometry,
                                                         RTC_BUFFER_TYPE_VERTEX,
                                                         0,
                                                         RTC_FORMAT_FLOAT3,
                                                         3 * sizeof(float),
                                                         positions.size() / 3));
    std::transform(positions.begin(), positions.end(), vertices, [](FT value) {
        return static_cast<float>(value);
    });

    unsigned* faces = nullptr;
    if (type == RTC_GEOMETRY_TYPE_TRIANGLE) {
        faces = reinterpret_cast<unsigned*>(
            rtcSetNewGeometryBuffer(_geometry,
                                    RTC_BUFFER_TYPE_INDEX,
                                    0,
                                    RTC_FORMAT_UINT3,
                                    3 * sizeof(unsigned),
                                    indices.size() / 3));
    }
    else { // type == RTC_GEOMETRY_TYPE_QUAD
        faces = reinterpret_cast<unsigned*>(
            rtcSetNewGeometryBuffer(_geometry,
                                    RTC_BUFFER_TYPE_INDEX,
                                    0,
                                    RTC_FORMAT_UINT4,
                                    4 * sizeof(unsigned),
                                    indices.size() / 4));
    }
    std::transform(indices.begin(), indices.end(), faces, [](IT value) {
        return static_cast<unsigned>(value);
    });

    rtcCommitGeometry(_geometry);
    _geom_id = rtcAttachGeometry(_scene, _geometry);
    rtcReleaseGeometry(_geometry);
    rtcCommitScene(_scene);
}

// TODO: add generic type support
inline void RayTracer::attach_geometry_shared(
    const std::vector<float>& positions,
    const std::vector<unsigned>& indices,
    RTCGeometryType type)
{
    if (positions.empty() || indices.empty()) {
        EWARNING("Input geometry is empty.");
        return;
    }
    if (positions.size() % 3 != 0) {
        throw std::invalid_argument(
            "Size of input positions is not divisible by 3.");
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

    // Release previously allocated geometry if presents
    if (_geom_id != -1) { rtcDetachGeometry(_scene, _geom_id); }

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
    rtcReleaseGeometry(_geometry);
    rtcCommitScene(_scene);
}

inline void RayTracer::release_geometry()
{
    if (_geom_id != -1) {
        rtcDetachGeometry(_scene, _geom_id);
        _geom_id = -1;
    }
}

template<typename T>
void RayTracer::render_shaded(T* pixels,
                              const Camera& camera,
                              const unsigned width,
                              const unsigned height,
                              const unsigned samples,
                              bool interleaved)
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    Eigen::Vector3f base_color = Eigen::Vector3f(1.0f, 1.0f, 1.0f);
    std::random_device rd;
    std::minstd_rand rd_gen(rd());
    std::uniform_real_distribution<> rd_number(0.0f, 1.0f);

    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            Eigen::Vector3f color(0.0f, 0.0f, 0.0f);
            for (unsigned s = 0; s < samples; ++s) {
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

                    Eigen::Vector3f ambient = base_color * 0.3f;
                    Eigen::Vector3f diffuse =
                        base_color *
                        std::clamp(normal.dot(-lightdir), 0.0f, 1.0f) * 0.7f;
                    color += ambient + diffuse;
                }
            }
            color /= samples;
            color *= 255;
            if (interleaved) {
                pixels[3 * ((height - y - 1) * width + x) + 0] =
                    static_cast<T>(color(0));
                pixels[3 * ((height - y - 1) * width + x) + 1] =
                    static_cast<T>(color(1));
                pixels[3 * ((height - y - 1) * width + x) + 2] =
                    static_cast<T>(color(2));
            }
            else {
                pixels[(height - y - 1) * width + x] = static_cast<T>(color(0));
                pixels[width * height + (height - y - 1) * width + x] =
                    static_cast<T>(color(1));
                pixels[2 * width * height + (height - y - 1) * width + x] =
                    static_cast<T>(color(2));
            }
        }
    }
}

template<typename T>
void RayTracer::render_depth(T* pixels,
                             const Camera& camera,
                             const unsigned width,
                             const unsigned height,
                             bool tone_mapped)
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    auto positions = reinterpret_cast<float*>(
        rtcGetGeometryBufferData(_geometry, RTC_BUFFER_TYPE_VERTEX, 0));
    auto indices = reinterpret_cast<unsigned*>(
        rtcGetGeometryBufferData(_geometry, RTC_BUFFER_TYPE_INDEX, 0));
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            auto u = static_cast<float>(x) / width;
            auto v = static_cast<float>(y) / height;
            auto rayhit = camera.gen_ray(u, v);
            rtcIntersect1(_scene, &context, &rayhit);
            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                auto v0 = indices[3 * rayhit.hit.primID];
                auto v1 = indices[3 * rayhit.hit.primID + 1];
                auto v2 = indices[3 * rayhit.hit.primID + 2];
                Eigen::Vector3f p0(positions[3 * v0 + 0],
                                   positions[3 * v0 + 1],
                                   positions[3 * v0 + 2]);
                Eigen::Vector3f p1(positions[3 * v1 + 0],
                                   positions[3 * v1 + 1],
                                   positions[3 * v1 + 2]);
                Eigen::Vector3f p2(positions[3 * v2 + 0],
                                   positions[3 * v2 + 1],
                                   positions[3 * v2 + 2]);
                Eigen::Vector3f p = p0 * rayhit.hit.u + p1 * rayhit.hit.v +
                                    p2 * (1.0f - rayhit.hit.u - rayhit.hit.v);
                auto depth = (p - camera.pos).norm();
                if (tone_mapped) {
                    auto value = depth / (depth + 1.0);
                    pixels[(height - y - 1) * width + x] =
                        static_cast<T>(value * 255);
                }
                else {
                    pixels[(height - y - 1) * width + x] =
                        static_cast<T>(depth);
                }
            }
        }
    }
}

template<typename T>
void RayTracer::render_silhouette(T* pixels,
                                  const Camera& camera,
                                  const unsigned width,
                                  const unsigned height)
{
    RTCIntersectContext context;
    rtcInitIntersectContext(&context);
    for (unsigned y = 0; y < height; ++y) {
        for (unsigned x = 0; x < width; ++x) {
            auto u = static_cast<float>(x) / width;
            auto v = static_cast<float>(y) / height;
            auto rayhit = camera.gen_ray(u, v);
            rtcIntersect1(_scene, &context, &rayhit);
            if (rayhit.hit.geomID != RTC_INVALID_GEOMETRY_ID) {
                pixels[(height - y - 1) * width + x] = static_cast<T>(255);
            }
        }
    }
}

} // namespace Euclid
