/** Render mesh using GPU rasterization.
 *
 *  Many times geometry algorithms require analysis on the rendered views.
 *  This package utilizes Vulkan to do headless GPU rendering.
 *
 *  @defgroup PkgRasterizer Rasterizer
 *  @ingroup PkgRender
 */
#pragma once
#include <Euclid/Config.h>
#ifdef EUCLID_USE_VULKAN

#include <vector>
#include <boost/math/constants/constants.hpp>
#include <vulkan/vulkan.h>
#include <Euclid/Render/RenderCore.h>
#include <config.h>

namespace Euclid
{
/** @{*/

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
     *  @param tnear The near clipping plane.
     *  @param tfar The far clipping plane.
     */
    RasCamera(const Eigen::Vector3f& position,
              const Eigen::Vector3f& focus,
              const Eigen::Vector3f& up,
              float tnear,
              float tfar);

    virtual ~RasCamera();

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
                   unsigned height,
                   float tnear,
                   float tfar);

    ~PerspRasCamera();

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

    /** Return the projection matrix.
     *
     */
    virtual Eigen::Matrix4f projection() const override;

private:
    float _vfov = boost::math::float_constants::half_pi;
    float _aspect = 1.0f;
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
     *  @param tnear Near clip plane.
     *  @param tfar Far clip plane.
     */
    OrthoRasCamera(const Eigen::Vector3f& position,
                   const Eigen::Vector3f& focus,
                   const Eigen::Vector3f& up,
                   float xextent,
                   float yextent,
                   float tnear,
                   float tfar);

    ~OrthoRasCamera();

    /** Set the extent of the frustum.
     *
     */
    void set_extent(float xextent, float yextent);

    /** Return the projection matrix.
     *
     */
    virtual Eigen::Matrix4f projection() const override;

private:
    float _xextent;
    float _yextent;
};

/** A simple rasterizer.
 *
 *  This rasterizer could render several types of images of a triangle mesh.
 */
class Rasterizer
{
public:
    enum SampleCount
    {
        SAMPLE_COUNT_1 = 0x00000001,
        SAMPLE_COUNT_2 = 0x00000002,
        SAMPLE_COUNT_4 = 0x00000004,
        SAMPLE_COUNT_8 = 0x00000008,
    };

public:
    /** Create a rasterizer.
     *
     *  Essential vulkan setup goes here. There's no need for a window since all
     *  rendering happens offscreen. If your gpu is not suitable for rendering,
     *  an exception will be thrown.
     */
    Rasterizer(uint32_t width = 256,
               uint32_t height = 256,
               SampleCount samples = SAMPLE_COUNT_1);

    ~Rasterizer();

    /** Attach a position buffer to the rasterizer.
     *
     *  User should at least provide this buffer for the rasterizer to render
     *  anything. The size of the position buffer determines how many vertices
     *  are drawn.
     *
     *  @param positions An array storing [x,y,z,x,y,z...] coordinates for each
     *  point. Set position to nullptr to release the current buffer.
     *  @param size The size of the buffer.
     */
    void attach_position_buffer(const float* positions, size_t size);

    /** Attach a normal buffer to the rasterizer.
     *
     *  Normal buffer is requested in render_shaded.
     *
     *  @param normals An array storing [x,y,z,x,y,z...] coordinates for each
     *  point. Set normal to nullptr to release the current buffer.
     *  @param size The size of the buffer.
     */
    void attach_normal_buffer(const float* normals, size_t size);

    /** Attach a color buffer to the rasterizer.
     *
     *  If color buffer is provided, the rasterizer will use this buffer in
     *  the render_shaded and render_unlit functions. Otherwise, it will use the
     *  colors provided in the material.
     *
     *  @param colors An array storing [r,g,b,r,g,b...] colors for each point.
     *  The values range in [0 1]. Set colors to nullptr to release the current
     *  buffer and fall back to the material.
     *  @param size The size of the buffer.
     */
    void attach_color_buffer(const float* colors, size_t size);

    /** Attach an index buffer to the Rasterizer.
     *
     *  If index buffer is provided, the rasterizer will use indexed drawing
     *  mode. Otherwise, it will use plain drawing mode.
     *
     *  @param indices An array storing [v1,v2,v3,v1,v2,v3...] indices for each
     *  triangle. Set indices to nullptr to release the current buffer and
     *  fall back to plain drawing mode.
     *  @param size The size of the buffer.
     */
    void attach_index_buffer(const unsigned* indices, size_t size);

    /** Release all associated buffers.
     *
     */
    void release_buffers();

    /** Change the material of the model.
     *
     */
    void set_material(const Material& material);

    /** Change the light settings.
     *
     */
    void set_light(const Light& light);

    /** Change the color of background.
     *
     */
    void set_background(const Eigen::Array3f& color);

    /** Change the color of background.
     *
     */
    void set_background(float r, float g, float b);

    /** Set the size and sampling of the resulting image.
     *
     */
    void set_image(uint32_t width, uint32_t height, SampleCount samples);

    /** Render the mesh into a shaded image.
     *
     *  This function renders the mesh with simple lambertian shading, using a
     *  point light located at the camera position.
     *
     *  @param model The model matrix.
     *  @param camera Camera.
     *  @param pixels Output pixels
     *  @param interleaved If true, pixels are stored like [RGBRGBRGB...],
     *  otherwise pixels are stored like [RRR...GGG...BBB...].
     */
    void render_shaded(const Eigen::Matrix4f& model,
                       const RasCamera& camera,
                       std::vector<uint8_t>& pixels,
                       bool interleaved = true);

    /** Render the mesh into a unlit image.
     *
     *  Only show raw diffuse color or vertex color if provided.
     *
     *  @param model The model matrix.
     *  @param camera Camera.
     *  @param pixels Output pixels.
     *  @param interleaved If true, pixels are stored like [RGBRGBRGB...],
     *  otherwise pixels are stored like [RRR...GGG...BBB...].
     */
    void render_unlit(const Eigen::Matrix4f& model,
                      const RasCamera& camera,
                      std::vector<uint8_t>& pixels,
                      bool interleaved = true);

    /** Render the mesh into a depth image.
     *
     *  The minimum depth value is mapped to 255 in image, and the maximum
     * depth value is mapped to 0.
     *
     *  @param model The model matrix.
     *  @param camera Camera.
     *  @param pixels Output pixels.
     *  @param interleaved If true, pixels are stored like [RGBRGBRGB...],
     *  otherwise pixels are stored like [RRR...GGG...BBB...].
     *  @param linear If true, return linearized depth value, otherwise
     * return z value as it is.
     */
    void render_depth(const Eigen::Matrix4f& model,
                      const RasCamera& camera,
                      std::vector<uint8_t>& pixels,
                      bool interleaved = true,
                      bool linear = true);

    /** Render the mesh into a depth image.
     *
     *  The depth values are returned as they are, while the depth of the
     *  background is one.
     *
     *  @param model The model matrix.
     *  @param camera Camera.
     *  @param values The depth values.
     *  @param linear If true, return linearized depth value, otherwise return z
     *  value as it is.
     */
    void render_depth(const Eigen::Matrix4f& model,
                      const RasCamera& camera,
                      std::vector<float>& values,
                      bool linear = true);

private:
    enum ShaderType : int
    {
        SHADER_TYPE_SHADING = 0,
        SHADER_TYPE_VCOLOR,
        SHADER_TYPE_UNLIT,
        SHADER_TYPE_UNLIT_VCOLOR,
        SHADER_TYPE_DEPTH,
        SHADER_TYPE_COUNT
    };

private:
    void _create_instance();

    void _pick_physical_device();

    uint32_t _create_logical_device();

    void _create_command_pool(uint32_t queue_family_index);

    void _create_descriptor_pool();

    void _create_descriptors();

    void _create_render_pass(VkSampleCountFlagBits samples);

    void _create_pipeline(uint32_t width,
                          uint32_t height,
                          VkSampleCountFlagBits samples,
                          ShaderType shader);

    void _render_scene(const Eigen::Matrix4f& model,
                       const RasCamera& camera,
                       const std::vector<VkBuffer>& active_buffers,
                       ShaderType shader);

    void _copy_fb_to_host_buffer(VkImage src,
                                 VkImageAspectFlags aspect,
                                 VkBuffer dst);

    void _transfer_data(const void* data,
                        size_t byte_size,
                        VkBufferUsageFlagBits usage,
                        VkBuffer& buffer,
                        VkDeviceMemory& buffer_memory);

    uint32_t _find_proper_memory_type(uint32_t mem_type_bits,
                                      VkMemoryPropertyFlags prop_flags);

    void _create_buffer(VkBuffer& bufer,
                        VkDeviceMemory& buffer_memory,
                        VkDeviceSize size,
                        VkBufferUsageFlags usage,
                        VkMemoryPropertyFlags props);

    void _release_buffer(VkBuffer& buffer, VkDeviceMemory& buffer_memory);

    void _copy_buffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

    void _create_image(VkImage& image,
                       VkDeviceMemory& memory,
                       VkFormat format,
                       VkImageUsageFlags usage,
                       VkImageTiling tiling,
                       VkMemoryPropertyFlags prop_flags,
                       uint32_t width,
                       uint32_t height,
                       VkSampleCountFlagBits samples);

    void _release_image(VkImage& buffer, VkDeviceMemory& memory);

    void _create_image_view(VkImageView& view,
                            VkImage image,
                            VkFormat format,
                            VkImageAspectFlags aspect);

    void _create_framebuffer(uint32_t width, uint32_t height);

    VkShaderModule _load_shader(const char* path);

    void _update_ubo(const VkDescriptorBufferInfo& info,
                     uint32_t binding,
                     ShaderType shader);

private:
    static const Material _default_material;

    static const Light _default_light;

private:
    Material _material;
    Light _light;
    Eigen::Array3f _background = Eigen::Array3f::Zero();
    size_t _num_vertices;
    size_t _num_indices;
    uint32_t _width;
    uint32_t _height;
    SampleCount _samples;

    VkDebugUtilsMessengerEXT _debug_messenger;
    VkInstance _instance;
    VkPhysicalDevice _physical_device;
    VkPhysicalDeviceMemoryProperties _physical_device_memory_properties;
    VkDevice _device;
    VkQueue _queue;
    VkCommandPool _command_pool;
    VkDescriptorPool _descriptor_pool;
    std::vector<VkDescriptorSetLayout> _descriptor_set_layouts;
    std::vector<VkDescriptorSet> _descriptor_sets;
    std::vector<VkPipelineLayout> _pipeline_layouts;
    std::vector<VkPipeline> _pipelines;
    VkFramebuffer _framebuffer = VK_NULL_HANDLE;
    VkRenderPass _render_pass = VK_NULL_HANDLE;
    VkBuffer _position_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _position_buffer_memory;
    VkBuffer _normal_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _normal_buffer_memory;
    VkBuffer _color_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _color_buffer_memory;
    VkBuffer _index_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _index_buffer_memory;
    VkBuffer _transform_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _transform_buffer_memory;
    VkBuffer _material_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _material_buffer_memory;
    VkBuffer _light_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _light_buffer_memory;
    VkBuffer _output_color_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _output_color_buffer_memory;
    VkBuffer _output_depth_buffer = VK_NULL_HANDLE;
    VkDeviceMemory _output_depth_buffer_memory;
    VkImage _color_image = VK_NULL_HANDLE;
    VkDeviceMemory _color_image_memory;
    VkImageView _color_image_view = VK_NULL_HANDLE;
    VkImage _depth_image = VK_NULL_HANDLE;
    VkDeviceMemory _depth_image_memory;
    VkImageView _depth_image_view = VK_NULL_HANDLE;
    VkImage _color_resolve_image = VK_NULL_HANDLE;
    VkDeviceMemory _color_resolve_image_memory;
    VkImageView _color_resolve_image_view = VK_NULL_HANDLE;
    VkImage _depth_resolve_image = VK_NULL_HANDLE;
    VkDeviceMemory _depth_resolve_image_memory;
    VkImageView _depth_resolve_image_view = VK_NULL_HANDLE;
    std::vector<VkShaderModule> _shader_modules;
};

/** @}*/
} // namespace Euclid

#include "src/Rasterizer.cpp"

#endif
