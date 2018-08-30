#include <algorithm>
#include <functional>
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

inline RayCamera::RayCamera(const Vec3& position,
                            const Vec3& focus,
                            const Vec3& up)
    : Camera(position, focus, up)
{}

inline void RayCamera::set_range(float tnear, float tfar)
{
    this->tnear = tnear;
    this->tfar = tfar;
}

inline PerspRayCamera::PerspRayCamera(const Vec3& position,
                                      const Vec3& focus,
                                      const Vec3& up,
                                      float vfov,
                                      float aspect)
    : RayCamera(position, focus, up)
{
    auto fov = vfov * boost::math::float_constants::degree;
    this->film.height = 2.0f * std::tan(fov * 0.5f);
    this->film.width = aspect * this->film.height;
}

inline PerspRayCamera::PerspRayCamera(const Vec3& position,
                                      const Vec3& focus,
                                      const Vec3& up,
                                      float vfov,
                                      unsigned width,
                                      unsigned height)
    : RayCamera(position, focus, up)
{
    auto fov = vfov * boost::math::float_constants::degree;
    auto aspect = static_cast<float>(width) / height;
    this->film.height = 2.0f * std::tan(fov * 0.5f);
    this->film.width = aspect * film.height;
}

inline void PerspRayCamera::set_aspect(float aspect)
{
    this->film.width = aspect * this->film.height;
}

inline void PerspRayCamera::set_aspect(unsigned width, unsigned height)
{
    auto aspect = static_cast<float>(width) / height;
    this->film.width = aspect * this->film.height;
}

inline void PerspRayCamera::set_fov(float vfov)
{
    auto fov = vfov * boost::math::float_constants::degree;
    auto aspect = this->film.width / this->film.height;
    this->film.height = 2.0f * std::tan(fov * 0.5f);
    this->film.width = aspect * this->film.height;
}

inline RTCRayHit PerspRayCamera::gen_ray(float s, float t) const
{
    RTCRayHit rayhit;
    Eigen::Vector3f view = -this->dir + (s - 0.5f) * this->film.width * u +
                           (t - 0.5f) * this->film.height * v;
    EASSERT(view.dot(this->dir) < 0.0f);

    rayhit.ray.org_x = this->pos(0);
    rayhit.ray.org_y = this->pos(1);
    rayhit.ray.org_z = this->pos(2);
    rayhit.ray.dir_x = view(0);
    rayhit.ray.dir_y = view(1);
    rayhit.ray.dir_z = view(2);
    rayhit.ray.tnear = this->tnear;
    rayhit.ray.tfar = this->tfar;
    rayhit.ray.flags = 0;
    rayhit.hit.geomID = RTC_INVALID_GEOMETRY_ID;
    return rayhit;
}

inline OrthoRayCamera::OrthoRayCamera(const Vec3& position,
                                      const Vec3& focus,
                                      const Vec3& up,
                                      float width,
                                      float height)
    : RayCamera(position, focus, up)
{
    this->film.width = width;
    this->film.height = height;
}

inline void OrthoRayCamera::set_extent(float width, float height)
{
    this->film.width = width;
    this->film.height = height;
}

inline RTCRayHit OrthoRayCamera::gen_ray(float s, float t) const
{
    RTCRayHit rayhit;
    Eigen::Vector3f origin = this->pos + (s - 0.5f) * this->film.width * u +
                             (t - 0.5f) * this->film.height * v;

    rayhit.ray.org_x = origin(0);
    rayhit.ray.org_y = origin(1);
    rayhit.ray.org_z = origin(2);
    rayhit.ray.dir_x = -this->dir(0);
    rayhit.ray.dir_y = -this->dir(1);
    rayhit.ray.dir_z = -this->dir(2);
    rayhit.ray.tnear = this->tnear;
    rayhit.ray.tfar = this->tfar;
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
    const std::vector<unsigned>& indices)
{
    if (positions.empty() || indices.empty()) {
        EWARNING("Input geometry is empty.");
        return;
    }
    if (positions.size() % 3 != 1) {
        throw std::invalid_argument("The last element of the positions buffer "
                                    "is not padded to 16 bytes. Add "
                                    "one more 0.0f to your positions buffer.");
    }
    if (indices.size() % 3 != 0) {
        throw std::invalid_argument("Size of input indices is not divisible by "
                                    "3, thus not a valid triangle mesh.");
    }

    release_buffers();

    _geometry = rtcNewGeometry(_device, RTC_GEOMETRY_TYPE_TRIANGLE);
    rtcSetSharedGeometryBuffer(_geometry,
                               RTC_BUFFER_TYPE_VERTEX,
                               0,
                               RTC_FORMAT_FLOAT3,
                               positions.data(),
                               0,
                               3 * sizeof(float),
                               positions.size() / 3);
    rtcSetSharedGeometryBuffer(_geometry,
                               RTC_BUFFER_TYPE_INDEX,
                               0,
                               RTC_FORMAT_UINT3,
                               indices.data(),
                               0,
                               3 * sizeof(unsigned),
                               indices.size() / 3);
    rtcCommitGeometry(_geometry);
    _geom_id = rtcAttachGeometry(_scene, _geometry);
    rtcCommitScene(_scene);
}

inline void RayTracer::attach_color_buffer(const std::vector<float>* colors,
                                           bool vertex_color)
{
    if (colors && colors->empty()) {
        EWARNING("Input geometry is empty.");
        return;
    }
    if (colors && vertex_color && colors->size() % 3 != 1) {
        throw std::invalid_argument("The last element of the colors buffer "
                                    "is not padded to 16 bytes. Add "
                                    "one more 0.0f to your colors buffer.");
    }

    // ! vertex color -> vertex color
    if (!(_colors && _vertex_color) && (colors && vertex_color)) {
        rtcSetGeometryVertexAttributeCount(_geometry, 1);
        rtcSetSharedGeometryBuffer(_geometry,
                                   RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                                   0,
                                   RTC_FORMAT_FLOAT3,
                                   colors->data(),
                                   0,
                                   3 * sizeof(float),
                                   colors->size() / 3);
        rtcCommitGeometry(_geometry);
    }
    // vertex color -> ! vertex color
    else if ((_colors && _vertex_color) && !(colors && vertex_color)) {
        rtcSetGeometryVertexAttributeCount(_geometry, 0);
        rtcCommitGeometry(_geometry);
    }
    // vertex color -> vertex color
    else if ((_colors && _vertex_color) && (colors && vertex_color)) {
        rtcSetSharedGeometryBuffer(_geometry,
                                   RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE,
                                   0,
                                   RTC_FORMAT_FLOAT3,
                                   colors->data(),
                                   0,
                                   3 * sizeof(float),
                                   colors->size() / 3);
        rtcCommitGeometry(_geometry);
    }

    _colors = colors;
    _vertex_color = vertex_color;
}

inline void RayTracer::attach_face_mask_buffer(const std::vector<uint8_t>* mask)
{
    // no mask -> mask
    if (!_face_mask && mask) {
        rtcSetGeometryIntersectFilterFunction(_geometry, _impl::mask_filter);
    }
    // mask -> no mask
    else if (_face_mask && !mask) {
        rtcSetGeometryIntersectFilterFunction(_geometry, nullptr);
    }

    uint8_t* buffer = nullptr;
    if (mask) { buffer = const_cast<uint8_t*>(mask->data()); }
    rtcSetGeometryUserData(_geometry, buffer);
    rtcCommitGeometry(_geometry);
    _face_mask = mask;
}

inline void RayTracer::release_buffers()
{
    if (_geom_id != -1) {
        rtcDetachGeometry(_scene, _geom_id);
        _geom_id = -1;
        _colors = nullptr;
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

inline void RayTracer::render_shaded(std::vector<uint8_t>& pixels,
                                     const RayCamera& camera,
                                     int width,
                                     int height,
                                     bool interleaved)
{
    pixels.resize(3 * width * height);
    auto diffuse_color = _select_diffuse_color();

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
                Eigen::Array3f ambient = _material.ambient;
                Eigen::Array3f diffuse = diffuse_color(rayhit.hit);
                if (_lighting) {
                    Eigen::Vector3f normal = Eigen::Vector3f(rayhit.hit.Ng_x,
                                                             rayhit.hit.Ng_y,
                                                             rayhit.hit.Ng_z)
                                                 .normalized();
                    // // Point light at the view position
                    Eigen::Vector3f lightdir = Eigen::Vector3f(rayhit.ray.dir_x,
                                                               rayhit.ray.dir_y,
                                                               rayhit.ray.dir_z)
                                                   .normalized();
                    diffuse *= std::abs(normal.dot(-lightdir));
                }
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

inline void RayTracer::render_shaded(std::vector<uint8_t>& pixels,
                                     const RayCamera& camera,
                                     int width,
                                     int height,
                                     int samples,
                                     bool interleaved)
{
    pixels.resize(3 * width * height);
    auto diffuse_color = _select_diffuse_color();

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
                    Eigen::Array3f ambient = _material.ambient;
                    Eigen::Array3f diffuse = diffuse_color(rayhit.hit);
                    if (_lighting) {
                        Eigen::Vector3f normal =
                            Eigen::Vector3f(rayhit.hit.Ng_x,
                                            rayhit.hit.Ng_y,
                                            rayhit.hit.Ng_z)
                                .normalized();
                        // // Point light at the view position
                        Eigen::Vector3f lightdir =
                            Eigen::Vector3f(rayhit.ray.dir_x,
                                            rayhit.ray.dir_y,
                                            rayhit.ray.dir_z)
                                .normalized();
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

inline void RayTracer::render_depth(std::vector<uint8_t>& pixels,
                                    const RayCamera& camera,
                                    int width,
                                    int height)
{
    pixels.resize(width * height);
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

inline void RayTracer::render_depth(std::vector<float>& values,
                                    const RayCamera& camera,
                                    int width,
                                    int height)
{
    values.resize(width * height);
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

inline void RayTracer::render_silhouette(std::vector<uint8_t>& pixels,
                                         const RayCamera& camera,
                                         int width,
                                         int height)
{
    pixels.resize(width * height);
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

inline void RayTracer::render_index(std::vector<uint8_t>& pixels,
                                    const RayCamera& camera,
                                    int width,
                                    int height,
                                    bool interleaved)
{
    pixels.resize(3 * width * height);
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

inline void RayTracer::render_index(std::vector<uint32_t>& indices,
                                    const RayCamera& camera,
                                    int width,
                                    int height)
{
    indices.resize(width * height);
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

inline std::function<Eigen::Array3f(const RTCHit&)>
RayTracer::_select_diffuse_color()
{
    if (_colors && _vertex_color) {
        return std::bind(
            &RayTracer::_diffuse_vertex_color, this, std::placeholders::_1);
    }
    else if (_colors && !_vertex_color) {
        return std::bind(
            &RayTracer::_diffuse_face_color, this, std::placeholders::_1);
    }
    else {
        return std::bind(
            &RayTracer::_diffuse_material, this, std::placeholders::_1);
    }
}

inline Eigen::Array3f RayTracer::_diffuse_material(const RTCHit& hit)
{
    (void)hit;
    return _material.diffuse;
}

inline Eigen::Array3f RayTracer::_diffuse_face_color(const RTCHit& hit)
{
    Eigen::Array3f diffuse;
    diffuse << (*_colors)[3 * hit.primID], (*_colors)[3 * hit.primID + 1],
        (*_colors)[3 * hit.primID + 2];
    return diffuse;
}

inline Eigen::Array3f RayTracer::_diffuse_vertex_color(const RTCHit& hit)
{
    float buffer[4];
    RTCInterpolateArguments args;
    args.geometry = _geometry;
    args.primID = hit.primID;
    args.u = hit.u;
    args.v = hit.v;
    args.bufferType = RTC_BUFFER_TYPE_VERTEX_ATTRIBUTE;
    args.bufferSlot = 0;
    args.valueCount = 3;
    args.P = buffer;
    args.dPdu = nullptr;
    args.dPdv = nullptr;
    args.ddPdudu = nullptr;
    args.ddPdvdv = nullptr;
    args.ddPdudv = nullptr;
    rtcInterpolate(&args);

    Eigen::Array3f diffuse;
    diffuse << buffer[0], buffer[1], buffer[2];
    return diffuse;
}

} // namespace Euclid
