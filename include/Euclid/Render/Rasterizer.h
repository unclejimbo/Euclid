/** Render mesh using GPU rasterization.
 *
 *  Many times geometry algorithms require analysis on the rendered views.
 *  This package utilizes Vulkan to do headless GPU rendering.
 *
 *  @defgroup PkgRasterizer Rasterizer
 *  @ingroup PkgRender
 */
#pragma once

#include <boost/math/constants/constants.hpp>
#include <vulkan/vulkan.h>
#include <Euclid/Render/RenderCore.h>
#include <config.h>

namespace Euclid
{
/** A camera model used for rasterization.
 *
 */
class RasCamera : public Camera
{
public:
    /** Create a RasCamera.
     *
     */
    RasCamera() = default;

    /** Create a RasCamera.
     *
     *  @param position Position.
     *  @param focus Focus.
     *  @param up Rough up direction.
     */
    RasCamera(const Eigen::Vector3f& position,
              const Eigen::Vector3f& focus = Eigen::Vector3f::Zero(),
              const Eigen::Vector3f& up = Eigen::Vector3f(0.0f, 1.0f, 0.0f));

    virtual ~RasCamera() = default;

    /** Return the view/lookat matrix.
     *
     */
    Eigen::Matrix4f view() const;

    /** Return the projection matrix.
     *
     */
    virtual Eigen::Matrix4f projection() const = 0;
};

/** A RasCamera using perspective projection.
 *
 *  The range of visible frustum of a perspective camera is determined
 *  by the field of view and aspect ratio.
 */
class PerspRasCamera : public RasCamera
{
public:
    /** Create a PerspRasCamera using default parameters.
     *
     */
    PerspRasCamera() : RasCamera(){};

    /** Create a PerspRasCamera.
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
     *  @param tnear Near plane of the view frustum.
     *  @param tfar Far plane of the view frustum.
     */
    PerspRasCamera(const Eigen::Vector3f& position,
                   const Eigen::Vector3f& focus,
                   const Eigen::Vector3f& up,
                   float vfov,
                   float aspect,
                   float tnear,
                   float tfar);

    /** Create a PerspRasCamera.
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
     *  @param tnear Near plane of the view frustum.
     *  @param tfar Far plane of the view frustum.
     */
    PerspRasCamera(const Eigen::Vector3f& position,
                   const Eigen::Vector3f& focus,
                   const Eigen::Vector3f& up,
                   float vfov,
                   unsigned width,
                   unsigned heigh,
                   float tnear,
                   float tfar);

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

    /** Set the near and far plane.
     *
     */
    void set_range(float tnear, float tfar);

    /** Return the projection matrix.
     *
     */
    virtual Eigen::Matrix4f projection() const override;

private:
    float _vfov = boost::math::float_constants::half_pi;
    float _aspect = 1.0f;
    float _tnear = 0.001f;
    float _tfar = 1000.0f;
};

/** A RasCamera using orthographic projection.
 *
 *  The range of visible frustum of an orthographic camera is determined
 *  by the extent of film plane in world space.
 */
class OrthoRasCamera : public RasCamera
{
public:
    /** Create an OrthoRasCamera using default parameters.
     *
     */
    OrthoRasCamera() : RasCamera(){};

    /** Create an OrthoRasCamera.
     *
     *  In addition to camera position and orientation, an orthogonal camera
     *  specifies width and height of the film plane directly.
     *
     *  @param position Position.
     *  @param focus Focus.
     *  @param up Rough up direction.
     *  @param xextent Width of the frustum in world space.
     *  @param yextent Height of the frustum in world space.
     */
    OrthoRasCamera(const Eigen::Vector3f& position,
                   const Eigen::Vector3f& focus,
                   const Eigen::Vector3f& up,
                   float xextent,
                   float yextent);

    /** Set the extent of the frustum.
     *
     */
    void set_extent(float xextent, float yextent);

    /** Return the projection matrix.
     *
     */
    virtual Eigen::Matrix4f projection() const override;

private:
    float _x;
    float _y;
};

/** A simple rasterizer.
 *
 *  This rasterizer could render several types of images of a triangle mesh.
 */
class Rasterizer
{
public:
    /** Create a rasterizer.
     *
     */
    Rasterizer();

    ~Rasterizer();

    /** Attach shared geoemtry buffers to the rasterizer.
     *
     *  Attach both the positions and indices buffer. These buffers are mapped
     *  directly by the Rasterizer so their lifetime should outlive the end of
     *  rendering. This class can only render one mesh at a time, so attaching
     *  another geometry will automatically release the previously attached
     *  geometry and all associated buffers.
     *
     *  @param positions The geometry's positions buffer.
     *  @param indices The geometry's indices buffer.
     */
    void attach_geometry_buffers(const std::vector<float>& positions,
                                 const std::vector<unsigned>& indices);

    /** Attach a shared color buffer to the rasterizer.
     *
     *  Attach a color buffer storing either per-face colors or per-vertex
     *  colors. This buffer is mapped directly by the Rasterizer so their
     *  lifetime should outlive the end of rendering. Attach another buffer will
     *  automatically release the previously attached one.
     *
     *  @param colors A color array storing [r,g,b,r,g,b...] values of each
     *  elements. The values range in [0 1]. Set colors to nullptr to disable
     *  color buffering and fall back to the material.
     *  @param vertex_color True for vertex color and false for face color.
     *  Default to false.
     */
    void attach_color_buffer(const float* colors, bool vertex_color = false);

    /** Attach a face maks buffer to the rasterizer.
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
                       const RasCamera& camera,
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
                       const RasCamera& camera,
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
                      const RasCamera& camera,
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
                      const RasCamera& camera,
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
                           const RasCamera& camera,
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
                      const RasCamera& camera,
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
                      const RasCamera& camera,
                      int width,
                      int height);
    void enable_index();
    void disable_index();

private:
    Material __material;
    Eigen::Array3f _background = Eigen::Array3f::Zero();

    const float* _face_colors;
    bool render_with_color_buffer = false;
    bool render_with_index = false;
    bool _lighting = true;
    bool _vertex_color = false;
    const uint8_t* _face_mask = nullptr;
    const uint32_t* _index_color = nullptr;
    int index_size = 0;

    struct Vertex
    {
        float position[3];
        // float color[3];
        float normal[3];
    };

    struct Vertex_Color
    {
        float position[3];
        float normal[3];
        float color[3];
    };

    struct Vertex_Index // do not need the normal vector anymore when using the
                        // render_index method
    {
        float position[3];
        float color[3];
    };

    struct Vertex_Silhouette
    {
        float position[3];
    };

    struct
    {
        VkImage image;
        VkDeviceMemory mem;
        VkImageView view;
    } depthStencil;

    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    uint32_t queueFamilyIndex;
    VkPipelineCache pipelineCache;
    VkQueue queue;
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    VkDescriptorSetLayout descriptorSetLayout;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    std::vector<VkShaderModule> shaderModules;
    VkBuffer vertexBuffer, indexBuffer;
    VkDeviceMemory vertexMemory, indexMemory;

    struct FrameBufferAttachment
    {
        VkImage image;
        VkDeviceMemory memory;
        VkImageView view;
    };
    int32_t width, height;
    VkFramebuffer framebuffer;
    FrameBufferAttachment colorAttachment, depthAttachment;

    FrameBufferAttachment dstAttachment;
    VkRenderPass renderPass;

    VkDebugReportCallbackEXT debugReportCallback{};
};

/** @}*/
} // namespace Euclid

#include "src/Rasterizer.cpp"
