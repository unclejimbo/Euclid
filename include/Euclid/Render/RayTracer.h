/** Render mesh using ray tracing.
 *
 *  Many times geometry algorithms require analysis on the rendered views.
 *  This package utilizes Embree to do fast CPU ray tracing.
 *
 *  @defgroup PkgRayTracer Ray-Tracer
 *  @ingroup PkgRender
 */
#pragma once

#include <cmath>
#include <limits>
#include <vector>

#include <Eigen/Core>
#include <embree3/rtcore.h>

namespace Euclid
{
/** @{*/

/** The film plane.
 *
 */
struct Film
{
    float width;
    float height;
};

/** A simple positionable camera model.
 *
 *  This class uses the right-handed coordinate system.
 */
class Camera
{
public:
    /** Create a Camera with default paramters.
     *
     */
    Camera() = default;

    /** Create a Camera.
     *
     *  Position the camera using position, focus and up.
     *
     *  @param position Position.
     *  @param focus Focus.
     *  @param up Rough up direction.
     */
    Camera(const Eigen::Vector3f& position,
           const Eigen::Vector3f& focus = Eigen::Vector3f::Zero(),
           const Eigen::Vector3f& up = Eigen::Vector3f(0.0f, 1.0f, 0.0f));

    virtual ~Camera() = default;

    /** Position the camera according to the parameteres.
     *
     */
    void lookat(const Eigen::Vector3f& position,
                const Eigen::Vector3f& focus,
                const Eigen::Vector3f& up);

    /** Generate an embree rayhit structure.
     *
     *  Generate a ray for a pixel (s, t) on the film plane.
     *  The parameter of the ray ranges in [near, far). The returned RTCRayHit
     *  structure also has its hit.geomID filed set to RTC_INVALID_GEOMETRY_ID.
     *
     *  @param s The u coordinate on the film plane, ranges in [0, 1).
     *  @param t The v coordinate on the film plane, ranges in [0, 1).
     *  @param near The value of ray parameter for the near end point.
     *  @param far The value of ray parameter for the far end point.
     */
    virtual RTCRayHit gen_ray(
        float s,
        float t,
        float near = 0.0f,
        float far = std::numeric_limits<float>::max()) const = 0;

public:
    /** Camera position.
     *
     */
    Eigen::Vector3f pos{ 0.0f, 0.0f, 0.0f };

    /** U direction.
     *
     */
    Eigen::Vector3f u{ 1.0f, 0.0f, 0.0f };

    /** V direction.
     *
     */
    Eigen::Vector3f v{ 0.0f, 1.0f, 0.0f };

    /** Negative view direction.
     *
     */
    Eigen::Vector3f dir{ 0.0f, 0.0f, 1.0f };

    /** Film plane.
     *
     */
    Film film{ 256, 256 };
};

/** A persective camera.
 *
 *  The range of visible frustum of a perspective camera is determined
 *  by the field of view and aspect ratio.
 */
class PerspectiveCamera : public Camera
{
public:
    /** Create a PerspectiveCamera using default parameters.
     *
     */
    PerspectiveCamera() : Camera(){};

    /** Create a PerspectiveCamera.
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
    PerspectiveCamera(const Eigen::Vector3f& position,
                      const Eigen::Vector3f& focus = Eigen::Vector3f::Zero(),
                      const Eigen::Vector3f& up = Eigen::Vector3f(0.0f,
                                                                  1.0f,
                                                                  0.0f),
                      float vfov = 90.0f,
                      float aspect = 1.0f);

    /** Create a PerspectiveCamera.
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
    PerspectiveCamera(const Eigen::Vector3f& position,
                      const Eigen::Vector3f& focus = Eigen::Vector3f::Zero(),
                      const Eigen::Vector3f& up = Eigen::Vector3f(0.0f,
                                                                  1.0f,
                                                                  0.0f),
                      float vfov = 90.0f,
                      unsigned width = 256,
                      unsigned height = 256);

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
    RTCRayHit gen_ray(
        float s,
        float t,
        float near = 0.0f,
        float far = std::numeric_limits<float>::max()) const override;
};

/** An orthogonal camera.
 *
 *  The range of visible frustum of an orthogonal camera is determined
 *  by the extent of film plane in world space.
 */
class OrthogonalCamera : public Camera
{
public:
    /** Create an OrthogonalCamera using default parameters.
     *
     */
    OrthogonalCamera() : Camera() {}

    /** Create an OrthogonalCamera.
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
    OrthogonalCamera(const Eigen::Vector3f& position,
                     const Eigen::Vector3f& focus = Eigen::Vector3f::Zero(),
                     const Eigen::Vector3f& up = Eigen::Vector3f(0.0f,
                                                                 1.0f,
                                                                 0.0f),
                     float xextent = 256.0f,
                     float yextent = 256.0f);

    /** Set the extent of the film plane.
     *
     */
    void set_extent(float width, float height);

    /** Generate an embree rayhit structure.
     *
     *  The ray's origin be at the pixel (s, t) on the film plane and points
     *  to the camera viewing direction.
     */
    RTCRayHit gen_ray(
        float s,
        float t,
        float near = 0.0f,
        float far = std::numeric_limits<float>::max()) const override;
};

/** A simple Phong material model.
 *
 */
struct Material
{
    Eigen::Array3f ambient;
    Eigen::Array3f diffuse;
};

/** A simple ray tracer.
 *
 *  This ray tracer could render the shaded, depth or silhouette image of a
 *  single mesh model.
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
     *  rendering. The user is responsible of padding the positions buffer with
     *  one more float for Embree's SSE instructions to work correctly.
     *
     *  @param positions The geometry's positions buffer.
     *  @param indices The geometry's indices buffer.
     *  @param type The geometry's type, be either RTC_GEOMETRY_TYPE_TRIANGLE or
     *  RTC_GEOMETRY_TYPE_QUAD.
     *
     *  **Note**
     *
     *  This class can only render one mesh at a time. Attach geometry once
     *  again will automatically release the previously attached geometry.
     */
    void attach_geometry_buffers(
        const std::vector<float>& positions,
        const std::vector<unsigned>& indices,
        RTCGeometryType type = RTC_GEOMETRY_TYPE_TRIANGLE);

    /** Attach a shared face color buffer to the ray tracer.
     *
     *  @param colors The size of the array pointed by colors should be equal to
     *  3 times the number of faces of the attached geometry, storing
     *  [r,g,b,r,g,b...] values of each face. The values range in [0 1]. Set
     *  colors to nullptr to disable face coloring and fall back to the
     *  material.
     */
    void attach_face_color_buffer(const float* colors);

    /** Attach a face maks buffer to the ray tracer.
     *
     *  This mask is used to filter out specified faces.
     *
     *  @param mask The size of the array pointed by mask should be equal to the
     *  number of faces of the attached geometry. Set (*mask)[i] to 1 to enable
     *  face i in intersection, otherwise it will be ignored. Set mask to
     *  nullptr to disable masking.
     */
    void attach_face_mask_buffer(const uint8_t* mask);

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
    void set_background(const Eigen::Array3f& color);

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
     *  This function renders the mesh with simple lambertian shading and store
     *  the pixel values to an array, using a point light located at the camera
     *  position.
     *
     *  @param pixels Output pixels
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     *  @param interleaved If true, pixels are stored like [RGBRGBRGB...],
     *  otherwise pixels are stored like [RRR...GGG...BBB...].
     */
    void render_shaded(uint8_t* pixels,
                       const Camera& camera,
                       int width,
                       int height,
                       bool interleaved = true);

    /** Render the mesh into a shaded image.
     *
     *  This function renders the mesh with simple lambertian shading and store
     *  the pixel values to an array, using a point light located at the camera
     *  position. Random multisampling is enabled.
     *
     *  @param pixels Output pixels
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     *  @param samples Number of samples per pixel.
     *  @param interleaved If true, pixels are stored like [RGBRGBRGB...],
     *  otherwise pixels are stored like [RRR...GGG...BBB...].
     */
    void render_shaded(uint8_t* pixels,
                       const Camera& camera,
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
    void render_depth(uint8_t* pixels,
                      const Camera& camera,
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
    void render_depth(float* values,
                      const Camera& camera,
                      int width,
                      int height);

    /** Render the mesh into a silhouette image.
     *
     *  @param pixels Output pixels.
     *  @param camera Camera.
     *  @param width Image width.
     *  @param height Image height.
     */
    void render_silhouette(uint8_t* pixels,
                           const Camera& camera,
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
    void render_index(uint8_t* pixels,
                      const Camera& camera,
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
    void render_index(uint32_t* indices,
                      const Camera& camera,
                      int width,
                      int height);

private:
    RTCDevice _device;
    RTCScene _scene;
    RTCGeometry _geometry;
    int _geom_id = -1;
    const float* _face_colors = nullptr;
    const uint8_t* _face_mask = nullptr;
    Material _material;
    Eigen::Array3f _background = Eigen::Array3f::Zero();
    bool _lighting = true;
};

/** @}*/
} // namespace Euclid

#include "src/RayTracer.cpp"
