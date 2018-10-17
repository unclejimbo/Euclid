/** Render mesh using ray tracing.
 *
 *  Many times geometry algorithms require analysis on the rendered views.
 *  This package utilizes Embree to do fast CPU ray tracing.
 *
 *  @defgroup PkgRayTracer Ray Tracer
 *  @ingroup PkgRender
 */
#pragma once

#include <cmath>
#include <limits>
#include <vector>

#include <embree3/rtcore.h>
#include <Euclid/Render/RenderCore.h>

namespace Euclid
{
/** @{*/

/** A camera model used for ray tracing.
 *
 */
class RayCamera : public Camera
{
public:
    /** The film plane of a camera.
     *
     *  The film plane is used to help generating rays for a RayCamera. The
     *  width and height are the actual size of the film in the world
     *  coordinates.
     */
    struct Film
    {
        float width = 0.0f;
        float height = 0.0f;
    };

public:
    /** Create a RayCamera.
     *
     */
    RayCamera() = default;

    /** Create a RayCamera.
     *
     *  @param position Position.
     *  @param focus Focus.
     *  @param up Rough up direction.
     */
    RayCamera(const Vec3& position,
              const Vec3& focus = Eigen::Vector3f::Zero(),
              const Vec3& up = Eigen::Vector3f(0.0f, 1.0f, 0.0f));

    virtual ~RayCamera() = default;

    /** Set the range of the ray.
     *
     *  @param tnear The near range of ray.
     *  @param tfar The far range of ray.
     */
    void set_range(float tnear, float tfar);

    /** Generate an embree rayhit structure.
     *
     *  The ray's origin be at the pixel (s, t) on the film plane and points
     *  to the camera viewing direction.
     */
    virtual RTCRayHit gen_ray(float s, float t) const = 0;

public:
    /** The film plane.
     *
     *  @param tnear The near range of ray.
     *  @param tfar The far range of ray.
     */
    Film film;

    /** The near plane.
     *
     */
    float tnear = 0.0f;

    /** The far plane.
     *
     */
    float tfar = std::numeric_limits<float>::max();
};

/** A RayCamera using perspective projection.
 *
 *  The range of visible frustum of a perspective camera is determined
 *  by the field of view and aspect ratio.
 */
class PerspRayCamera : public RayCamera
{
public:
    /** Create a PerspRayCamera using default parameters.
     *
     */
    PerspRayCamera() = default;

    /** Create a PerspRayCamera.
     *
     *  In addition to camera position and orientation, a perspective camera
     *  uses field of view and apsect ratio to determine the extent of the
     *  film plane.
     *
     *  @param position Position.
     *  @param focus Focus.
     *  @param up Rough up direction.
     *  @param vfov Vertical field of view in degrees.
     *  @param aspect Aspect ratio.
     */
    PerspRayCamera(const Vec3& position,
                   const Vec3& focus,
                   const Vec3& up,
                   float vfov,
                   float aspect);

    /** Create a PerspRayCamera.
     *
     *  In addition to camera position and orientation, a perspective camera
     *  uses field of view and apsect ratio to determine the extent of the
     *  film plane.
     *
     *  @param position Position.
     *  @param focus Focus.
     *  @param up Rough up direction.
     *  @param vfov Vertical field of view in degrees.
     *  @param width Width of the image.
     *  @param height Height of the image.
     */
    PerspRayCamera(const Vec3& position,
                   const Vec3& focus,
                   const Vec3& up,
                   float vfov,
                   unsigned width,
                   unsigned height);

    /** Set aspect ratio.
     *
     */
    void set_aspect(float aspect);

    /** Set aspect ratio.
     *
     */
    void set_aspect(unsigned width, unsigned height);

    /** Set vertical fov in degrees.
     *
     */
    void set_fov(float vfov);

    /** Generate an embree rayhit structure.
     *
     *  The ray's origin be at the camera position and points to the pixel
     *  (s, t) on the film plane.
     */
    RTCRayHit gen_ray(float s, float t) const override;
};

/** A RayCamera using orthographic projection.
 *
 *  The range of visible frustum of an orthographic camera is determined
 *  by the extent of film plane in world space.
 */
class OrthoRayCamera : public RayCamera
{
public:
    /** Create an OrthoRayCamera using default parameters.
     *
     */
    OrthoRayCamera() = default;

    /** Create an OrthoRayCamera.
     *
     *  In addition to camera position and orientation, an orthogonal camera
     *  specifies width and height of the film plane directly.
     *
     *  @param position Position.
     *  @param focus Focus.
     *  @param up Rough up direction.
     *  @param xextent Width of the film plane in world space.
     *  @param yextent Height of the film plane in world space.
     */
    OrthoRayCamera(const Vec3& position,
                   const Vec3& focus,
                   const Vec3& up,
                   float xextent,
                   float yextent);

    /** Set the extent of the film plane.
     *
     */
    void set_extent(float xextent, float yextent);

    /** Generate an embree rayhit structure.
     *
     *  The ray's origin be at the pixel (s, t) on the film plane and points
     *  to the camera viewing direction.
     */
    RTCRayHit gen_ray(float s, float t) const override;
};

/** A simple ray tracer.
 *
 *  This ray tracer could render several types of images of a triangle mesh.
 */
class RayTracer
{
public:
    /** Create a ray tracer.
     *
     *  @param threads Set to 0 to use number of hardware threads.
     */
    explicit RayTracer(int threads = 0);

    ~RayTracer();

    /** Attach shared geoemtry buffers to the ray tracer.
     *
     *  Attach both the positions and indices buffer. These buffers are mapped
     *  directly by the RayTracer so their lifetime should outlive the end of
     *  rendering. This class can only render one mesh at a time, so attaching
     *  another geometry will automatically release the previously attached
     *  geometry and all associated buffers.
     *
     *  **Note**
     *
     *  The user is responsible of padding the positions buffer with one more
     *  float for Embree's SSE instructions to work correctly.
     *
     *  @param positions The geometry's positions buffer.
     *  @param indices The geometry's indices buffer.
     */
    void attach_geometry_buffers(const std::vector<float>& positions,
                                 const std::vector<unsigned>& indices);

    /** Attach a shared color buffer to the ray tracer.
     *
     *  Attach a color buffer storing either per-face colors or per-vertex
     *  colors. This buffer is mapped directly by the RayTracer so their
     *  lifetime should outlive the end of rendering. Attach another buffer will
     *  automatically release the previously attached one.
     *
     *  **Note**
     *
     *  The user is responsible of padding the positions buffer with one more
     *  float for Embree's SSE instructions to work correctly, if you are in
     *  vertex color mode. Otherwise the padding will not be necessary.
     *
     *  @param colors A color array storing [r,g,b,r,g,b...] values of each
     *  elements. The values range in [0 1]. Set colors to nullptr to disable
     *  color buffering and fall back to the material.
     *  @param vertex_color True for vertex color and false for face color.
     *  Default to false.
     */
    void attach_color_buffer(const std::vector<float>* colors,
                             bool vertex_color = false);

    /** Attach a face maks buffer to the ray tracer.
     *
     *  This mask is used to filter out specified faces.
     *
     *  @param mask The size of the array pointed by mask should be equal to the
     *  number of faces of the attached geometry. Set (*mask)[i] to 1 to enable
     *  face i in intersection, otherwise it will be ignored. Set mask to
     *  nullptr to disable masking.
     */
    void attach_face_mask_buffer(const std::vector<uint8_t>* mask);

    /** Release all associated buffers.
     *
     */
    void release_buffers();

    /** Change the material of the model.
     *
     */
    void set_material(const Material& material);

    /** Change the color of background.
     *
     */
    void set_background(const Eigen::Ref<const Eigen::Array3f>& color);

    /** Change the color of background.
     *
     */
    void set_background(float r, float g, float b);

    /** Enable or diable lighting.
     *
     */
    void enable_light(bool on);

    /** Render the mesh into a shaded image.
     *
     *  This function renders the mesh with simple lambertian shading, using a
     *  point light located at the camera position.
     *
     *  @param pixels Output pixels
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     *  @param interleaved If true, pixels are stored like [RGBRGBRGB...],
     *  otherwise pixels are stored like [RRR...GGG...BBB...].
     */
    void render_shaded(std::vector<uint8_t>& pixels,
                       const RayCamera& camera,
                       int width,
                       int height,
                       bool interleaved = true);

    /** Render the mesh into a shaded image.
     *
     *  This function renders the mesh with simple lambertian shading, using a
     *  point light located at the camera position. Random multisampling is
     *  enabled.
     *
     *  @param pixels Output pixels
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     *  @param samples Number of samples per pixel.
     *  @param interleaved If true, pixels are stored like [RGBRGBRGB...],
     *  otherwise pixels are stored like [RRR...GGG...BBB...].
     */
    void render_shaded(std::vector<uint8_t>& pixels,
                       const RayCamera& camera,
                       int width,
                       int height,
                       int samples,
                       bool interleaved = true);

    /** Render the mesh into a depth image.
     *
     *  The minimum depth value is mapped to 255 in image, and the maximum depth
     *  value is mapped to 0.
     *
     *  @param pixels Output pixels.
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     */
    void render_depth(std::vector<uint8_t>& pixels,
                      const RayCamera& camera,
                      int width,
                      int height);

    /** Render the mesh into a depth image.
     *
     *  The depth values are returned as they are, while the depth of the
     *  background is set to negative.
     *
     *  @param values Output depth values.
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     */
    void render_depth(std::vector<float>& values,
                      const RayCamera& camera,
                      int width,
                      int height);

    /** Render the mesh into a silhouette image.
     *
     *  @param pixels Output pixels.
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     */
    void render_silhouette(std::vector<uint8_t>& pixels,
                           const RayCamera& camera,
                           int width,
                           int height);

    /** Render the mesh based on face index.
     *
     *  Color each face base on its index, following
     *
     *  face_index = r + g << 8 + b << 16 - 1;
     *
     *  The background is set to 0, and the primitive index starts from 1. Due
     *  to bit width limitations, only 2^24 number of faces could be uniquely
     *  colored.
     *
     *  @param pixels Output pixels.
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     *  @param interleaved If true, pixels are stored like [RGBRGBRGB...],
     *  otherwise pixels are stored like [RRR...GGG...BBB...].
     */
    void render_index(std::vector<uint8_t>& pixels,
                      const RayCamera& camera,
                      int width,
                      int height,
                      bool interleaved = true);

    /** Render the mesh based on face index.
     *
     *  Each index is stored as a uint32_t value. The background is set to 0,
     *  and the primitive index starts from 1. Due to bit width limitations, the
     *  maximum number of indices supported is 2^32.
     *
     *  @param indices The face indices of each pixel.
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     */
    void render_index(std::vector<uint32_t>& indices,
                      const RayCamera& camera,
                      int width,
                      int height);

private:
    /** Select the correct diffuse color function.
     *
     */
    std::function<Eigen::Array3f(const RTCHit&)> _select_diffuse_color();

    /** Return diffuse color based on material.
     *
     */
    Eigen::Array3f _diffuse_material(const RTCHit& hit);

    /** Retrun diffuse color based on face color.
     *
     */
    Eigen::Array3f _diffuse_face_color(const RTCHit& hit);

    /** Retrun diffuse color based on vertex color.
     *
     */
    Eigen::Array3f _diffuse_vertex_color(const RTCHit& hit);

private:
    Material _material;
    Eigen::Array3f _background = Eigen::Array3f::Zero();
    RTCDevice _device;
    RTCScene _scene;
    RTCGeometry _geometry;
    const std::vector<float>* _colors = nullptr;
    const std::vector<uint8_t>* _face_mask = nullptr;
    int _geom_id = -1;
    bool _vertex_color = false;
    bool _lighting = true;
};

/** @}*/
} // namespace Euclid

#include "src/RayTracer.cpp"
