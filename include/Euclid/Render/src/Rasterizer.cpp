#include <fstream>
#include <iostream>

namespace Euclid
{

namespace _impl
{

static inline VKAPI_ATTR VkBool32 VKAPI_CALL
vkDebugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT,
                VkDebugUtilsMessageTypeFlagsEXT,
                const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                void*)
{
    std::cerr << "validation layer: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
}

static inline VkResult vkCreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger)
{
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
        return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

static inline void vkDestroyDebugUtilsMessengerEXT(
    VkInstance instance,
    VkDebugUtilsMessengerEXT debugMessenger,
    const VkAllocationCallbacks* pAllocator)
{
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(
        instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) { func(instance, debugMessenger, pAllocator); }
}

static inline VkResult vkCreateRenderPass2KHR(
    VkInstance instance,
    VkDevice device,
    const VkRenderPassCreateInfo2KHR* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkRenderPass* pRenderPass)
{
    auto func = (PFN_vkCreateRenderPass2KHR)vkGetInstanceProcAddr(
        instance, "vkCreateRenderPass2KHR");
    if (func != nullptr) {
        return func(device, pCreateInfo, pAllocator, pRenderPass);
    }
    else {
        return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
}

} // namespace _impl

/********************** RasCamera **********************/

inline RasCamera::RasCamera(const Eigen::Vector3f& position,
                            const Eigen::Vector3f& focus,
                            const Eigen::Vector3f& up,
                            float tnear,
                            float tfar)
    : Camera(position, focus, up, tnear, tfar)
{}

inline RasCamera::~RasCamera() = default;

inline Eigen::Matrix4f RasCamera::view() const
{
    Eigen::Matrix4f m1, m2;
    // clang-format off
    m1 << u(0), u(1), u(2), 0.0f,
          v(0), v(1), v(2), 0.0f,
          dir(0), dir(1), dir(2), 0.0f,
          0.0f, 0.0f, 0.0f, 1.0f;
    m2 << 1.0f, 0.0f, 0.0f, -pos(0),
          0.0f, 1.0f, 0.0f, -pos(1),
          0.0f, 0.0f, 1.0f, -pos(2),
          0.0f, 0.0f, 0.0f, 1.0f;
    // clang-format on
    return m1 * m2;
}

inline PerspRasCamera::PerspRasCamera(const Eigen::Vector3f& position,
                                      const Eigen::Vector3f& focus,
                                      const Eigen::Vector3f& up,
                                      float vfov,
                                      float aspect,
                                      float tnear,
                                      float tfar)
    : RasCamera(position, focus, up, tnear, tfar)
{
    _vfov = vfov * boost::math::float_constants::degree;
    _aspect = aspect;
}

inline PerspRasCamera::PerspRasCamera(const Eigen::Vector3f& position,
                                      const Eigen::Vector3f& focus,
                                      const Eigen::Vector3f& up,
                                      float vfov,
                                      unsigned width,
                                      unsigned height,
                                      float tnear,
                                      float tfar)
    : RasCamera(position, focus, up, tnear, tfar)
{
    _vfov = vfov * boost::math::float_constants::degree;
    _aspect = static_cast<float>(width) / height;
}

inline PerspRasCamera::~PerspRasCamera() = default;

inline void PerspRasCamera::set_aspect(float aspect)
{
    _aspect = aspect;
}

inline void PerspRasCamera::set_aspect(unsigned width, unsigned height)
{
    _aspect = static_cast<float>(width) / height;
}

inline void PerspRasCamera::set_fov(float vfov)
{
    _vfov = vfov * boost::math::float_constants::degree;
}

inline Eigen::Matrix4f PerspRasCamera::projection() const
{
    auto yscale = std::tan(0.5f * _vfov);
    auto xscale = yscale * _aspect;
    auto d = 1.0f / (this->tfar - this->tnear);

    Eigen::Matrix4f proj_mat;
    proj_mat.setZero();
    proj_mat(0, 0) = 1.0f / xscale;
    proj_mat(1, 1) = -1.0f / yscale;  // invert y
    proj_mat(2, 2) = -this->tfar * d; // map z to [0, 1]
    proj_mat(2, 3) = -this->tfar * this->tnear * d;
    proj_mat(3, 2) = -1.0f;

    return proj_mat;
}

inline OrthoRasCamera::OrthoRasCamera(const Eigen::Vector3f& position,
                                      const Eigen::Vector3f& focus,
                                      const Eigen::Vector3f& up,
                                      float xextent,
                                      float yextent,
                                      float tnear,
                                      float tfar)
    : RasCamera(position, focus, up, tnear, tfar), _xextent(xextent),
      _yextent(yextent)
{}

inline OrthoRasCamera::~OrthoRasCamera() = default;

inline void OrthoRasCamera::set_extent(float xextent, float yextent)
{
    _xextent = xextent;
    _yextent = yextent;
}

inline Eigen::Matrix4f OrthoRasCamera::projection() const
{
    Eigen::Matrix4f proj_mat;
    proj_mat.setZero();
    proj_mat(0, 0) = 2.0f / _xextent;
    proj_mat(1, 1) = -2.0f / _yextent;
    proj_mat(2, 2) = -1.0f / (this->tfar - this->tnear); // invert y
    proj_mat(2, 3) =
        -this->tnear / (this->tfar - this->tnear); // map z to [0, 1]
    proj_mat(3, 3) = 1.0f;

    return proj_mat;
}

/************************ Rsterizer *************************/

inline const Material Rasterizer::_default_material{ { 0.1f, 0.1f, 0.1f },
                                                     { 0.7f, 0.7f, 0.7f } };

inline const Light Rasterizer::_default_light{ { 1.0f, 1.0f, 1.0f },
                                               { 1.0f, 1.0f, 1.0f },
                                               1.0f };

inline Rasterizer::Rasterizer(uint32_t width,
                              uint32_t height,
                              SampleCount samples)
{
    _pipeline_layouts.resize(SHADER_TYPE_COUNT);
    _pipelines.resize(SHADER_TYPE_COUNT);

    // vulkan instance and devices
    _create_instance();
    _pick_physical_device();
    auto index = _create_logical_device();

    // a single command pool recording all commands
    _create_command_pool(index);

    // descriptors (shader uniforms)
    _create_descriptor_pool();
    _create_descriptors();

    // images, framebuffers and pipelines
    // need descriptor_set_layout in pipeline
    set_image(width, height, samples);

    // create and initialize uniforms
    _create_buffer(_transform_buffer,
                   _transform_buffer_memory,
                   sizeof(Transform),
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    _create_buffer(_material_buffer,
                   _material_buffer_memory,
                   sizeof(Material),
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    _create_buffer(_light_buffer,
                   _light_buffer_memory,
                   sizeof(Light),
                   VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    set_material(_default_material);
    set_light(_default_light);
}

inline Rasterizer::~Rasterizer()
{
    vkDestroyImageView(_device, _color_image_view, nullptr);
    vkDestroyImageView(_device, _depth_image_view, nullptr);
    vkDestroyImageView(_device, _color_resolve_image_view, nullptr);
    vkDestroyImageView(_device, _depth_resolve_image_view, nullptr);
    _release_image(_color_image, _color_image_memory);
    _release_image(_depth_image, _depth_image_memory);
    _release_image(_color_resolve_image, _color_resolve_image_memory);
    _release_image(_depth_resolve_image, _depth_resolve_image_memory);
    vkDestroyFramebuffer(_device, _framebuffer, nullptr);
    for (auto pipeline : _pipelines) {
        vkDestroyPipeline(_device, pipeline, nullptr);
    }
    for (auto layout : _pipeline_layouts) {
        vkDestroyPipelineLayout(_device, layout, nullptr);
    }
    vkDestroyRenderPass(_device, _render_pass, nullptr);
    release_buffers();
    _release_buffer(_output_color_buffer, _output_color_buffer_memory);
    _release_buffer(_output_depth_buffer, _output_depth_buffer_memory);
    _release_buffer(_transform_buffer, _transform_buffer_memory);
    _release_buffer(_material_buffer, _material_buffer_memory);
    _release_buffer(_light_buffer, _light_buffer_memory);
    vkDestroyDescriptorPool(_device, _descriptor_pool, nullptr);
    for (auto& layout : _descriptor_set_layouts) {
        vkDestroyDescriptorSetLayout(_device, layout, nullptr);
    }
    vkDestroyCommandPool(_device, _command_pool, nullptr);
    vkDestroyDevice(_device, nullptr);
    _impl::vkDestroyDebugUtilsMessengerEXT(
        _instance, _debug_messenger, nullptr);
    vkDestroyInstance(_instance, nullptr);
}

inline void Rasterizer::attach_position_buffer(const float* positions,
                                               size_t size)
{
    if (_position_buffer != VK_NULL_HANDLE) {
        _release_buffer(_position_buffer, _position_buffer_memory);
        _num_vertices = 0;
    }
    if (positions != nullptr) {
        _transfer_data(positions,
                       sizeof(float) * size,
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       _position_buffer,
                       _position_buffer_memory);
        _num_vertices = size / 3;
    }
}

inline void Rasterizer::attach_normal_buffer(const float* normals, size_t size)
{
    if (_normal_buffer != VK_NULL_HANDLE) {
        _release_buffer(_normal_buffer, _normal_buffer_memory);
    }
    if (normals != nullptr) {
        _transfer_data(normals,
                       sizeof(float) * size,
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       _normal_buffer,
                       _normal_buffer_memory);
    }
}

inline void Rasterizer::attach_color_buffer(const float* colors, size_t size)
{
    if (_color_buffer != VK_NULL_HANDLE) {
        _release_buffer(_color_buffer, _color_buffer_memory);
    }
    if (colors != nullptr) {
        _transfer_data(colors,
                       sizeof(float) * size,
                       VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                       _color_buffer,
                       _color_buffer_memory);
    }
}

inline void Rasterizer::attach_index_buffer(const unsigned* indices,
                                            size_t size)
{
    if (_index_buffer != VK_NULL_HANDLE) {
        _release_buffer(_index_buffer, _index_buffer_memory);
        _num_indices = 0;
    }
    if (indices != nullptr) {
        _transfer_data(indices,
                       sizeof(unsigned) * size,
                       VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                       _index_buffer,
                       _index_buffer_memory);
        _num_indices = size;
    }
}

inline void Rasterizer::release_buffers()
{
    if (_position_buffer != VK_NULL_HANDLE) {
        _release_buffer(_position_buffer, _position_buffer_memory);
    }
    if (_normal_buffer != VK_NULL_HANDLE) {
        _release_buffer(_normal_buffer, _normal_buffer_memory);
    }
    if (_color_buffer != VK_NULL_HANDLE) {
        _release_buffer(_color_buffer, _color_buffer_memory);
    }
    if (_index_buffer != VK_NULL_HANDLE) {
        _release_buffer(_index_buffer, _index_buffer_memory);
    }
}

inline void Rasterizer::set_material(const Material& material)
{
    void* data;
    vkMapMemory(
        _device, _material_buffer_memory, 0, sizeof(Material), 0, &data);
    memcpy(data, &material, sizeof(Material));
    vkUnmapMemory(_device, _material_buffer_memory);

    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = _material_buffer;
    buffer_info.offset = 0;
    buffer_info.range = sizeof(Material);

    _update_ubo(buffer_info, 1, SHADER_TYPE_SHADING);
    _update_ubo(buffer_info, 1, SHADER_TYPE_VCOLOR);
    _update_ubo(buffer_info, 1, SHADER_TYPE_UNLIT);
}

inline void Rasterizer::set_light(const Light& light)
{
    void* data;
    vkMapMemory(_device, _light_buffer_memory, 0, sizeof(Light), 0, &data);
    memcpy(data, &light, sizeof(Light));
    vkUnmapMemory(_device, _light_buffer_memory);

    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = _light_buffer;
    buffer_info.offset = 0;
    buffer_info.range = sizeof(Light);

    _update_ubo(buffer_info, 2, SHADER_TYPE_SHADING);
    _update_ubo(buffer_info, 2, SHADER_TYPE_VCOLOR);
}

inline void Rasterizer::set_background(const Eigen::Array3f& color)
{
    _background = color;
}

inline void Rasterizer::set_background(float r, float g, float b)
{
    _background << r, g, b;
}

inline void Rasterizer::set_image(uint32_t width,
                                  uint32_t height,
                                  SampleCount samples)
{
    _width = width;
    _height = height;
    _samples = samples;
    auto vksamples = static_cast<VkSampleCountFlagBits>(samples);

    // images holds the actual data
    _create_image(_color_image,
                  _color_image_memory,
                  VK_FORMAT_R8G8B8A8_UNORM,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                      VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                  VK_IMAGE_TILING_OPTIMAL,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  width,
                  height,
                  vksamples);
    _create_image(_depth_image,
                  _depth_image_memory,
                  VK_FORMAT_D32_SFLOAT,
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                      VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                  VK_IMAGE_TILING_OPTIMAL,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  width,
                  height,
                  vksamples);
    _create_image(_color_resolve_image,
                  _color_resolve_image_memory,
                  VK_FORMAT_R8G8B8A8_UNORM,
                  VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
                      VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                  VK_IMAGE_TILING_OPTIMAL,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  width,
                  height,
                  VK_SAMPLE_COUNT_1_BIT);
    _create_image(_depth_resolve_image,
                  _depth_resolve_image_memory,
                  VK_FORMAT_D32_SFLOAT,
                  VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT |
                      VK_IMAGE_USAGE_TRANSFER_SRC_BIT,
                  VK_IMAGE_TILING_OPTIMAL,
                  VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                  width,
                  height,
                  VK_SAMPLE_COUNT_1_BIT);

    // image views point to images and are referenced by framebuffers
    _create_image_view(_color_image_view,
                       _color_image,
                       VK_FORMAT_R8G8B8A8_UNORM,
                       VK_IMAGE_ASPECT_COLOR_BIT);
    _create_image_view(_depth_image_view,
                       _depth_image,
                       VK_FORMAT_D32_SFLOAT,
                       VK_IMAGE_ASPECT_DEPTH_BIT);
    _create_image_view(_color_resolve_image_view,
                       _color_resolve_image,
                       VK_FORMAT_R8G8B8A8_UNORM,
                       VK_IMAGE_ASPECT_COLOR_BIT);
    _create_image_view(_depth_resolve_image_view,
                       _depth_resolve_image,
                       VK_FORMAT_D32_SFLOAT,
                       VK_IMAGE_ASPECT_DEPTH_BIT);

    // render pass is referenced in framebuffer as well as pipelines
    _create_render_pass(vksamples);

    // a single framebuffer with color and depth attachment is enough
    _create_framebuffer(width, height);

    // create one pipeline for each shader
    _create_pipeline(width, height, vksamples, SHADER_TYPE_SHADING);
    _create_pipeline(width, height, vksamples, SHADER_TYPE_VCOLOR);
    _create_pipeline(width, height, vksamples, SHADER_TYPE_UNLIT);
    _create_pipeline(width, height, vksamples, SHADER_TYPE_UNLIT_VCOLOR);
    _create_pipeline(width, height, vksamples, SHADER_TYPE_DEPTH);

    // copy images into host visible buffers
    if (_output_color_buffer != VK_NULL_HANDLE) {
        _release_buffer(_output_color_buffer, _output_color_buffer_memory);
    }
    _create_buffer(_output_color_buffer,
                   _output_color_buffer_memory,
                   sizeof(char) * 4 * width * height,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    if (_output_depth_buffer != VK_NULL_HANDLE) {
        _release_buffer(_output_depth_buffer, _output_depth_buffer_memory);
    }
    _create_buffer(_output_depth_buffer,
                   _output_depth_buffer_memory,
                   sizeof(float) * width * height,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
}

inline void Rasterizer::render_shaded(const Eigen::Matrix4f& model,
                                      const RasCamera& camera,
                                      std::vector<uint8_t>& pixels,
                                      bool interleaved)
{
    if (_color_buffer != VK_NULL_HANDLE) {
        std::vector<VkBuffer> buffers{ _position_buffer,
                                       _normal_buffer,
                                       _color_buffer };
        _render_scene(model, camera, buffers, SHADER_TYPE_VCOLOR);
    }
    else {
        std::vector<VkBuffer> buffers{ _position_buffer, _normal_buffer };
        _render_scene(model, camera, buffers, SHADER_TYPE_SHADING);
    }

    if (_samples == SAMPLE_COUNT_1) {
        _copy_fb_to_host_buffer(
            _color_image, VK_IMAGE_ASPECT_COLOR_BIT, _output_color_buffer);
    }
    else {
        _copy_fb_to_host_buffer(_color_resolve_image,
                                VK_IMAGE_ASPECT_COLOR_BIT,
                                _output_color_buffer);
    }

    const uint8_t* imagedata;
    vkMapMemory(_device,
                _output_color_buffer_memory,
                0,
                VK_WHOLE_SIZE,
                0,
                (void**)&imagedata);

    for (uint32_t y = 0; y < _height; y++) {
        for (uint32_t x = 0; x < _width; x++) {
            auto rowoffset = y * _width * sizeof(uint32_t);
            uint8_t r = imagedata[rowoffset + x * sizeof(uint32_t)];
            uint8_t g =
                imagedata[rowoffset + x * sizeof(uint32_t) + sizeof(uint8_t)];
            uint8_t b = imagedata[rowoffset + x * sizeof(uint32_t) +
                                  2 * sizeof(uint8_t)];
            if (interleaved) {
                pixels[3 * (y * _width + x) + 0] = r;
                pixels[3 * (y * _width + x) + 1] = g;
                pixels[3 * (y * _width + x) + 2] = b;
            }
            else {
                pixels[y * _width + x] = r;
                pixels[_width * _height + y * _width + x] = g;
                pixels[2 * _width * _height + y * _width + x] = b;
            }
        }
    }

    vkUnmapMemory(_device, _output_color_buffer_memory);
}

inline void Rasterizer::render_unlit(const Eigen::Matrix4f& model,
                                     const RasCamera& camera,
                                     std::vector<uint8_t>& pixels,
                                     bool interleaved)
{
    if (_color_buffer != VK_NULL_HANDLE) {
        std::vector<VkBuffer> buffers{ _position_buffer, _color_buffer };
        _render_scene(model, camera, buffers, SHADER_TYPE_UNLIT_VCOLOR);
    }
    else {
        std::vector<VkBuffer> buffers{ _position_buffer };
        _render_scene(model, camera, buffers, SHADER_TYPE_UNLIT);
    }

    if (_samples == SAMPLE_COUNT_1) {
        _copy_fb_to_host_buffer(
            _color_image, VK_IMAGE_ASPECT_COLOR_BIT, _output_color_buffer);
    }
    else {
        _copy_fb_to_host_buffer(_color_resolve_image,
                                VK_IMAGE_ASPECT_COLOR_BIT,
                                _output_color_buffer);
    }

    const uint8_t* imagedata;
    vkMapMemory(_device,
                _output_color_buffer_memory,
                0,
                VK_WHOLE_SIZE,
                0,
                (void**)&imagedata);

    for (uint32_t y = 0; y < _height; y++) {
        for (uint32_t x = 0; x < _width; x++) {
            auto rowoffset = y * _width * sizeof(uint32_t);
            uint8_t r = imagedata[rowoffset + x * sizeof(uint32_t)];
            uint8_t g =
                imagedata[rowoffset + x * sizeof(uint32_t) + sizeof(uint8_t)];
            uint8_t b = imagedata[rowoffset + x * sizeof(uint32_t) +
                                  2 * sizeof(uint8_t)];
            if (interleaved) {
                pixels[3 * (y * _width + x) + 0] = r;
                pixels[3 * (y * _width + x) + 1] = g;
                pixels[3 * (y * _width + x) + 2] = b;
            }
            else {
                pixels[y * _width + x] = r;
                pixels[_width * _height + y * _width + x] = g;
                pixels[2 * _width * _height + y * _width + x] = b;
            }
        }
    }

    vkUnmapMemory(_device, _output_color_buffer_memory);
}

inline void Rasterizer::render_depth(const Eigen::Matrix4f& model,
                                     const RasCamera& camera,
                                     std::vector<uint8_t>& pixels,
                                     bool interleaved,
                                     bool linear)
{
    std::vector<VkBuffer> buffers{ _position_buffer };
    _render_scene(model, camera, buffers, SHADER_TYPE_DEPTH);

    if (_samples == SAMPLE_COUNT_1) {
        _copy_fb_to_host_buffer(
            _depth_image, VK_IMAGE_ASPECT_DEPTH_BIT, _output_depth_buffer);
    }
    else {
        _copy_fb_to_host_buffer(_depth_resolve_image,
                                VK_IMAGE_ASPECT_DEPTH_BIT,
                                _output_depth_buffer);
    }

    const float* imagedata;
    vkMapMemory(_device,
                _output_depth_buffer_memory,
                0,
                VK_WHOLE_SIZE,
                0,
                (void**)&imagedata);

    auto linearize_depth = [&camera](float d) {
        return camera.tnear * camera.tfar /
               (camera.tfar + d * (camera.tnear - camera.tfar));
    };

    for (uint32_t y = 0; y < _height; y++) {
        for (uint32_t x = 0; x < _width; x++) {
            auto d = *imagedata++;
            uint8_t color = 0;
            if (linear) { color = 255 * linearize_depth(d) / camera.tfar; }
            else {
                color = 255 * d;
            }
            if (interleaved) {
                pixels[3 * (y * _width + x) + 0] = color;
                pixels[3 * (y * _width + x) + 1] = color;
                pixels[3 * (y * _width + x) + 2] = color;
            }
            else {
                pixels[y * _width + x] = color;
                pixels[_width * _height + y * _width + x] = color;
                pixels[2 * _width * _height + y * _width + x] = color;
            }
        }
    }

    vkUnmapMemory(_device, _output_depth_buffer_memory);
}

inline void Rasterizer::render_depth(const Eigen::Matrix4f& model,
                                     const RasCamera& camera,
                                     std::vector<float>& values,
                                     bool linear)
{
    std::vector<VkBuffer> buffers{ _position_buffer };
    _render_scene(model, camera, buffers, SHADER_TYPE_DEPTH);

    if (_samples == SAMPLE_COUNT_1) {
        _copy_fb_to_host_buffer(
            _depth_image, VK_IMAGE_ASPECT_DEPTH_BIT, _output_depth_buffer);
    }
    else {
        _copy_fb_to_host_buffer(_depth_resolve_image,
                                VK_IMAGE_ASPECT_DEPTH_BIT,
                                _output_depth_buffer);
    }

    const float* imagedata;
    vkMapMemory(_device,
                _output_depth_buffer_memory,
                0,
                VK_WHOLE_SIZE,
                0,
                (void**)&imagedata);

    auto linearize_depth = [&camera](float d) {
        return camera.tnear * camera.tfar /
               (camera.tfar + d * (camera.tnear - camera.tfar));
    };

    if (linear) {
        std::transform(imagedata,
                       imagedata + _width * _height,
                       values.begin(),
                       linearize_depth);
    }
    else {
        std::copy(imagedata, imagedata + _width * _height, values.begin());
    }

    vkUnmapMemory(_device, _output_depth_buffer_memory);
}

inline void Rasterizer::_create_instance()
{
    // Vulkan application
    VkApplicationInfo app_info{};
    app_info.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app_info.pApplicationName = "Rasterizer";
    app_info.applicationVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.pEngineName = "Euclid";
    app_info.engineVersion = VK_MAKE_VERSION(0, 1, 0);
    app_info.apiVersion = VK_API_VERSION_1_1;

    // Vulkan instance creation (without surface extensions)
    VkInstanceCreateInfo instance_info{};
    instance_info.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instance_info.pApplicationInfo = &app_info;
    instance_info.enabledLayerCount = 0;
    instance_info.enabledExtensionCount = 0;

#ifndef NDEBUG
    // Require validation layers on debug build
    const char* required_layers[] = { "VK_LAYER_KHRONOS_validation" };
    uint32_t num_required_layers = 1;

    // Check if required layers are available
    uint32_t num_available_layers;
    vkEnumerateInstanceLayerProperties(&num_available_layers, nullptr);
    std::vector<VkLayerProperties> available_layers(num_available_layers);
    vkEnumerateInstanceLayerProperties(&num_available_layers,
                                       available_layers.data());

    bool layers_available = true;
    for (auto required_layer : required_layers) {
        bool available = false;
        for (auto available_layer : available_layers) {
            if (strcmp(available_layer.layerName, required_layer) == 0) {
                available = true;
                break;
            }
        }
        if (!available) {
            layers_available = false;
            break;
        }
    }

    if (layers_available) {
        const char* required_extensions[]{ VK_EXT_DEBUG_UTILS_EXTENSION_NAME };
        instance_info.ppEnabledLayerNames = required_layers;
        instance_info.enabledLayerCount = num_required_layers;
        instance_info.enabledExtensionCount = 1;
        instance_info.ppEnabledExtensionNames = required_extensions;
    }
#endif

    if (vkCreateInstance(&instance_info, nullptr, &_instance) != VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

#ifndef NDEBUG
    if (layers_available) {
        VkDebugUtilsMessengerCreateInfoEXT debug_messenger_info{};
        debug_messenger_info.sType =
            VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
        debug_messenger_info.messageSeverity =
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
        debug_messenger_info.messageType =
            VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
            VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
        debug_messenger_info.pfnUserCallback = _impl::vkDebugCallback;
        debug_messenger_info.pUserData = nullptr;
        if (_impl::vkCreateDebugUtilsMessengerEXT(
                _instance, &debug_messenger_info, nullptr, &_debug_messenger) !=
            VK_SUCCESS) {
            throw std::runtime_error(
                "failed to create debug messenger extension!");
        }
    }
#endif
}

inline void Rasterizer::_pick_physical_device()
{
    // TODO: choose device based on usage
    // Enumerate physical devices and use the first one
    uint32_t device_count = 0;
    if (vkEnumeratePhysicalDevices(_instance, &device_count, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to find physical device!");
    }
    std::vector<VkPhysicalDevice> physicalDevices(device_count);
    if (vkEnumeratePhysicalDevices(
            _instance, &device_count, physicalDevices.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to find physical device!");
    }
    _physical_device = physicalDevices[0];
    vkGetPhysicalDeviceMemoryProperties(_physical_device,
                                        &_physical_device_memory_properties);
}

inline uint32_t Rasterizer::_create_logical_device()
{
    // Request a single graphics queue
    const float default_priority = 1.0f;
    VkDeviceQueueCreateInfo queue_info{};
    uint32_t qf_count;
    vkGetPhysicalDeviceQueueFamilyProperties(
        _physical_device, &qf_count, nullptr);
    std::vector<VkQueueFamilyProperties> qf_properties(qf_count);
    vkGetPhysicalDeviceQueueFamilyProperties(
        _physical_device, &qf_count, qf_properties.data());
    for (uint32_t i = 0; i < static_cast<uint32_t>(qf_properties.size()); i++) {
        if (qf_properties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            queue_info.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queue_info.queueFamilyIndex = i;
            queue_info.queueCount = 1;
            queue_info.pQueuePriorities = &default_priority;
            break;
        }
    }

    // uint32_t cnt;
    // vkEnumerateDeviceExtensionProperties(
    //     _physical_device, nullptr, &cnt, nullptr);
    // std::vector<VkExtensionProperties> props(cnt);
    // vkEnumerateDeviceExtensionProperties(
    //     _physical_device, nullptr, &cnt, props.data());
    // for (const auto& prop : props) {
    //     std::cout << prop.extensionName << std::endl;
    // }

    // Requested extensions
    const char* extensions[4] = { "VK_KHR_create_renderpass2",
                                  "VK_KHR_multiview",
                                  "VK_KHR_maintenance2" };

    // Create logical device
    VkDeviceCreateInfo device_info{};
    device_info.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    device_info.queueCreateInfoCount = 1;
    device_info.pQueueCreateInfos = &queue_info;
    device_info.enabledExtensionCount = 3;
    device_info.ppEnabledExtensionNames = extensions;
    if (vkCreateDevice(_physical_device, &device_info, nullptr, &_device) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    // Get the queue handle
    vkGetDeviceQueue(_device, queue_info.queueFamilyIndex, 0, &_queue);

    return queue_info.queueFamilyIndex;
}

inline void Rasterizer::_create_command_pool(uint32_t queue_family_index)
{
    VkCommandPoolCreateInfo cmd_pool_info{};
    cmd_pool_info.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmd_pool_info.queueFamilyIndex = queue_family_index;
    if (vkCreateCommandPool(_device, &cmd_pool_info, nullptr, &_command_pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

inline void Rasterizer::_create_descriptor_pool()
{
    VkDescriptorPoolSize pool_size{};
    pool_size.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    pool_size.descriptorCount = 10;

    VkDescriptorPoolCreateInfo descriptor_pool_info{};
    descriptor_pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptor_pool_info.poolSizeCount = 1;
    descriptor_pool_info.pPoolSizes = &pool_size;
    descriptor_pool_info.maxSets = SHADER_TYPE_COUNT;
    if (vkCreateDescriptorPool(
            _device, &descriptor_pool_info, nullptr, &_descriptor_pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor pool!");
    }
}

inline void Rasterizer::_create_descriptors()
{
    auto create_binding = [](uint32_t binding_point, VkShaderStageFlags stage) {
        VkDescriptorSetLayoutBinding binding{};
        binding.binding = binding_point;
        binding.stageFlags = stage;
        binding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        binding.descriptorCount = 1;
        binding.pImmutableSamplers = nullptr;
        return binding;
    };

    // Specify bindings for each descriptor set layout
    std::vector<std::vector<VkDescriptorSetLayoutBinding>> bindings(
        SHADER_TYPE_COUNT);
    bindings[SHADER_TYPE_SHADING].push_back(
        create_binding(0, VK_SHADER_STAGE_VERTEX_BIT));
    bindings[SHADER_TYPE_SHADING].push_back(
        create_binding(1, VK_SHADER_STAGE_FRAGMENT_BIT));
    bindings[SHADER_TYPE_SHADING].push_back(
        create_binding(2, VK_SHADER_STAGE_FRAGMENT_BIT));
    bindings[SHADER_TYPE_VCOLOR].push_back(
        create_binding(0, VK_SHADER_STAGE_VERTEX_BIT));
    bindings[SHADER_TYPE_VCOLOR].push_back(
        create_binding(1, VK_SHADER_STAGE_FRAGMENT_BIT));
    bindings[SHADER_TYPE_VCOLOR].push_back(
        create_binding(2, VK_SHADER_STAGE_FRAGMENT_BIT));
    bindings[SHADER_TYPE_UNLIT].push_back(
        create_binding(0, VK_SHADER_STAGE_VERTEX_BIT));
    bindings[SHADER_TYPE_UNLIT].push_back(
        create_binding(1, VK_SHADER_STAGE_FRAGMENT_BIT));
    bindings[SHADER_TYPE_UNLIT_VCOLOR].push_back(
        create_binding(0, VK_SHADER_STAGE_VERTEX_BIT));
    bindings[SHADER_TYPE_DEPTH].push_back(
        create_binding(0, VK_SHADER_STAGE_VERTEX_BIT));

    // Create descriptor set layouts
    _descriptor_set_layouts.resize(SHADER_TYPE_COUNT);
    for (size_t i = 0; i < _descriptor_set_layouts.size(); ++i) {
        VkDescriptorSetLayoutCreateInfo layout_info{};
        layout_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        layout_info.bindingCount = static_cast<uint32_t>(bindings[i].size());
        layout_info.pBindings = bindings[i].data();
        if (vkCreateDescriptorSetLayout(
                _device, &layout_info, nullptr, &_descriptor_set_layouts[i]) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create descriptor set layout!");
        }
    }

    // Create descriptor sets
    _descriptor_sets.resize(SHADER_TYPE_COUNT);
    VkDescriptorSetAllocateInfo alloc_info{};
    alloc_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    alloc_info.descriptorPool = _descriptor_pool;
    alloc_info.descriptorSetCount = static_cast<uint32_t>(SHADER_TYPE_COUNT);
    alloc_info.pSetLayouts = _descriptor_set_layouts.data();
    if (vkAllocateDescriptorSets(
            _device, &alloc_info, _descriptor_sets.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate descriptor sets!");
    }
}

inline void Rasterizer::_create_render_pass(VkSampleCountFlagBits samples)
{
    if (_render_pass != VK_NULL_HANDLE) {
        vkDestroyRenderPass(_device, _render_pass, nullptr);
    }

    if (samples == VK_SAMPLE_COUNT_1_BIT) {
        std::array<VkAttachmentDescription, 2> attachment_descriptions{};
        attachment_descriptions[0].format = VK_FORMAT_R8G8B8A8_UNORM,
        attachment_descriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_descriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_descriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_descriptions[0].stencilLoadOp =
            VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_descriptions[0].stencilStoreOp =
            VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_descriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_descriptions[0].finalLayout =
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        attachment_descriptions[1].format = VK_FORMAT_D32_SFLOAT,
        attachment_descriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_descriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_descriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_descriptions[1].stencilLoadOp =
            VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_descriptions[1].stencilStoreOp =
            VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_descriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_descriptions[1].finalLayout =
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        VkAttachmentReference color_ref;
        color_ref.attachment = 0;
        color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        VkAttachmentReference depth_ref;
        depth_ref.attachment = 1;
        depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

        VkSubpassDescription subpass_description{};
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.pColorAttachments = &color_ref;
        subpass_description.pDepthStencilAttachment = &depth_ref;

        // Reading the color attachment in the previous pass should be completed
        // before writing to it again
        std::array<VkSubpassDependency, 2> dependencies{};
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask =
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask =
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo render_pass_info{};
        render_pass_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        render_pass_info.attachmentCount =
            static_cast<uint32_t>(attachment_descriptions.size());
        render_pass_info.pAttachments = attachment_descriptions.data();
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass_description;
        render_pass_info.dependencyCount =
            static_cast<uint32_t>(dependencies.size());
        render_pass_info.pDependencies = dependencies.data();
        if (vkCreateRenderPass(
                _device, &render_pass_info, nullptr, &_render_pass) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create renderpass!");
        }
    }
    else {
        // Need VK_KHR_create_renderpass2 extension for depth resolving
        std::array<VkAttachmentDescription2KHR, 4> attachment_descriptions{};
        attachment_descriptions[0].sType =
            VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
        attachment_descriptions[0].format = VK_FORMAT_R8G8B8A8_UNORM,
        attachment_descriptions[0].samples = samples;
        attachment_descriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_descriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_descriptions[0].stencilLoadOp =
            VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_descriptions[0].stencilStoreOp =
            VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_descriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_descriptions[0].finalLayout =
            VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        attachment_descriptions[1].sType =
            VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
        attachment_descriptions[1].format = VK_FORMAT_D32_SFLOAT,
        attachment_descriptions[1].samples = samples;
        attachment_descriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_descriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_descriptions[1].stencilLoadOp =
            VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_descriptions[1].stencilStoreOp =
            VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_descriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_descriptions[1].finalLayout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        attachment_descriptions[2].sType =
            VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
        attachment_descriptions[2].format = VK_FORMAT_R8G8B8A8_UNORM,
        attachment_descriptions[2].samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_descriptions[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_descriptions[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_descriptions[2].stencilLoadOp =
            VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_descriptions[2].stencilStoreOp =
            VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_descriptions[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_descriptions[2].finalLayout =
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        attachment_descriptions[3].sType =
            VK_STRUCTURE_TYPE_ATTACHMENT_DESCRIPTION_2_KHR;
        attachment_descriptions[3].format = VK_FORMAT_D32_SFLOAT,
        attachment_descriptions[3].samples = VK_SAMPLE_COUNT_1_BIT;
        attachment_descriptions[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        attachment_descriptions[3].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        attachment_descriptions[3].stencilLoadOp =
            VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        attachment_descriptions[3].stencilStoreOp =
            VK_ATTACHMENT_STORE_OP_DONT_CARE;
        attachment_descriptions[3].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        attachment_descriptions[3].finalLayout =
            VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

        VkAttachmentReference2KHR color_ref{};
        color_ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
        color_ref.attachment = 0;
        color_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_ref.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        VkAttachmentReference2KHR depth_ref{};
        depth_ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
        depth_ref.attachment = 1;
        depth_ref.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_ref.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        VkAttachmentReference2KHR color_resolve_ref{};
        color_resolve_ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
        color_resolve_ref.attachment = 2;
        color_resolve_ref.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        color_resolve_ref.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        VkAttachmentReference2KHR depth_resolve_ref{};
        depth_resolve_ref.sType = VK_STRUCTURE_TYPE_ATTACHMENT_REFERENCE_2_KHR;
        depth_resolve_ref.attachment = 3;
        depth_resolve_ref.layout =
            VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        depth_resolve_ref.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        VkSubpassDescriptionDepthStencilResolve depth_resolve{};
        depth_resolve.sType =
            VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_DEPTH_STENCIL_RESOLVE_KHR;
        depth_resolve.pDepthStencilResolveAttachment = &depth_resolve_ref;
        depth_resolve.depthResolveMode = VK_RESOLVE_MODE_AVERAGE_BIT_KHR;
        depth_resolve.stencilResolveMode = VK_RESOLVE_MODE_NONE_KHR;

        VkSubpassDescription2KHR subpass_description{};
        subpass_description.sType = VK_STRUCTURE_TYPE_SUBPASS_DESCRIPTION_2_KHR;
        subpass_description.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass_description.inputAttachmentCount = 0;
        subpass_description.pInputAttachments = nullptr;
        subpass_description.preserveAttachmentCount = 0;
        subpass_description.pPreserveAttachments = nullptr;
        subpass_description.colorAttachmentCount = 1;
        subpass_description.pColorAttachments = &color_ref;
        subpass_description.pDepthStencilAttachment = &depth_ref;
        subpass_description.pResolveAttachments = &color_resolve_ref;
        subpass_description.pNext = &depth_resolve;

        std::array<VkSubpassDependency2KHR, 2> dependencies{};
        dependencies[0].sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2_KHR;
        dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[0].dstSubpass = 0;
        dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[0].dstStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[0].dstAccessMask =
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
        dependencies[1].sType = VK_STRUCTURE_TYPE_SUBPASS_DEPENDENCY_2_KHR;
        dependencies[1].srcSubpass = 0;
        dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
        dependencies[1].srcStageMask =
            VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
        dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
        dependencies[1].srcAccessMask =
            VK_ACCESS_MEMORY_READ_BIT | VK_ACCESS_MEMORY_WRITE_BIT;
        dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
        dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

        VkRenderPassCreateInfo2KHR render_pass_info{};
        render_pass_info.sType =
            VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO_2_KHR;
        render_pass_info.attachmentCount =
            static_cast<uint32_t>(attachment_descriptions.size());
        render_pass_info.pAttachments = attachment_descriptions.data();
        render_pass_info.subpassCount = 1;
        render_pass_info.pSubpasses = &subpass_description;
        render_pass_info.dependencyCount =
            static_cast<uint32_t>(dependencies.size());
        render_pass_info.pDependencies = dependencies.data();
        if (_impl::vkCreateRenderPass2KHR(_instance,
                                          _device,
                                          &render_pass_info,
                                          nullptr,
                                          &_render_pass) != VK_SUCCESS) {
            throw std::runtime_error("failed to create renderpass!");
        }
    }
}

inline void Rasterizer::_create_pipeline(uint32_t width,
                                         uint32_t height,
                                         VkSampleCountFlagBits samples,
                                         ShaderType shader)
{
    if (_pipelines[shader] != VK_NULL_HANDLE) {
        vkDestroyPipeline(_device, _pipelines[shader], nullptr);
    }
    if (_pipeline_layouts[shader] != VK_NULL_HANDLE) {
        vkDestroyPipelineLayout(_device, _pipeline_layouts[shader], nullptr);
    }

    std::vector<VkFormat> attribute_formats;
    std::string vertex_shader_path;
    std::string fragment_shader_path;
    switch (shader) {
    case SHADER_TYPE_SHADING: {
        attribute_formats = { VK_FORMAT_R32G32B32_SFLOAT,
                              VK_FORMAT_R32G32B32_SFLOAT };
        vertex_shader_path =
            std::string(DATA_DIR).append("shader/shading.vert.spv");
        fragment_shader_path =
            std::string(DATA_DIR).append("shader/shading.frag.spv");
    } break;
    case SHADER_TYPE_VCOLOR: {
        attribute_formats = { VK_FORMAT_R32G32B32_SFLOAT,
                              VK_FORMAT_R32G32B32_SFLOAT,
                              VK_FORMAT_R32G32B32_SFLOAT };
        vertex_shader_path =
            std::string(DATA_DIR).append("shader/shading_vcolor.vert.spv");
        fragment_shader_path =
            std::string(DATA_DIR).append("shader/shading_vcolor.frag.spv");
    } break;
    case SHADER_TYPE_UNLIT: {
        attribute_formats = { VK_FORMAT_R32G32B32_SFLOAT };
        vertex_shader_path =
            std::string(DATA_DIR).append("shader/unlit.vert.spv");
        fragment_shader_path =
            std::string(DATA_DIR).append("shader/unlit.frag.spv");
    } break;
    case SHADER_TYPE_UNLIT_VCOLOR: {
        attribute_formats = { VK_FORMAT_R32G32B32_SFLOAT,
                              VK_FORMAT_R32G32B32_SFLOAT };
        vertex_shader_path =
            std::string(DATA_DIR).append("shader/unlit_vcolor.vert.spv");
        fragment_shader_path =
            std::string(DATA_DIR).append("shader/unlit_vcolor.frag.spv");
    } break;
    case SHADER_TYPE_DEPTH: {
        attribute_formats = { VK_FORMAT_R32G32B32_SFLOAT };
        vertex_shader_path =
            std::string(DATA_DIR).append("shader/depth.vert.spv");
        fragment_shader_path =
            std::string(DATA_DIR).append("shader/depth.frag.spv");
    } break;
    default: return;
    }

    VkPipelineLayout layout;
    VkPipelineLayoutCreateInfo pipeline_layout_info{};
    pipeline_layout_info.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipeline_layout_info.setLayoutCount = 1;
    pipeline_layout_info.pSetLayouts = &_descriptor_set_layouts[shader];
    if (vkCreatePipelineLayout(
            _device, &pipeline_layout_info, nullptr, &layout) != VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }
    _pipeline_layouts[shader] = layout;

    VkPipelineInputAssemblyStateCreateInfo input_assembly_info{};
    input_assembly_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    input_assembly_info.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    input_assembly_info.primitiveRestartEnable = VK_FALSE;

    VkViewport viewport;
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = width;
    viewport.height = height;
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    VkRect2D scissor;
    scissor.offset = { 0, 0 };
    scissor.extent = { width, height };
    VkPipelineViewportStateCreateInfo viewport_info{};
    viewport_info.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewport_info.viewportCount = 1;
    viewport_info.pViewports = &viewport;
    viewport_info.scissorCount = 1;
    viewport_info.pScissors = &scissor;

    VkPipelineRasterizationStateCreateInfo rasterization_info{};
    rasterization_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterization_info.polygonMode = VK_POLYGON_MODE_FILL;
    rasterization_info.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterization_info.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterization_info.depthClampEnable = VK_FALSE;
    rasterization_info.depthBiasEnable = VK_FALSE;
    rasterization_info.rasterizerDiscardEnable = VK_FALSE;
    rasterization_info.lineWidth = 1.0f;

    VkPipelineMultisampleStateCreateInfo multisample_info{};
    multisample_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisample_info.sampleShadingEnable = VK_FALSE;
    multisample_info.rasterizationSamples = samples;

    VkPipelineDepthStencilStateCreateInfo depth_stencil_info{};
    depth_stencil_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depth_stencil_info.depthTestEnable = VK_TRUE;
    depth_stencil_info.depthWriteEnable = VK_TRUE;
    depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
    depth_stencil_info.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState blend_attachment{};
    blend_attachment.blendEnable = VK_FALSE;
    blend_attachment.colorWriteMask =
        VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    VkPipelineColorBlendStateCreateInfo blend_info{};
    blend_info.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    blend_info.attachmentCount = 1;
    blend_info.pAttachments = &blend_attachment;

    VkPipelineDynamicStateCreateInfo dynamic_states_info{};
    dynamic_states_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;

    VkPipelineTessellationStateCreateInfo tess_state_info{};
    tess_state_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;

    std::vector<VkVertexInputBindingDescription> vertex_input_bindings(
        attribute_formats.size());
    for (size_t i = 0; i < vertex_input_bindings.size(); ++i) {
        vertex_input_bindings[i].binding = static_cast<uint32_t>(i);
        if (attribute_formats[i] == VK_FORMAT_R32G32B32_SFLOAT) {
            vertex_input_bindings[i].stride = sizeof(float) * 3;
        }
        else if (attribute_formats[i] == VK_FORMAT_R32_UINT) {
            vertex_input_bindings[i].stride = sizeof(uint32_t);
        }
        else {
            assert(false);
        }
        vertex_input_bindings[i].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    }
    std::vector<VkVertexInputAttributeDescription> vertex_input_attributes(
        attribute_formats.size());
    for (size_t i = 0; i < vertex_input_attributes.size(); ++i) {
        vertex_input_attributes[i].binding = static_cast<uint32_t>(i);
        vertex_input_attributes[i].location = static_cast<uint32_t>(i);
        vertex_input_attributes[i].format = attribute_formats[i];
        vertex_input_attributes[i].offset = 0;
    }
    VkPipelineVertexInputStateCreateInfo vertex_input_info{};
    vertex_input_info.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertex_input_info.vertexBindingDescriptionCount =
        static_cast<uint32_t>(vertex_input_bindings.size());
    vertex_input_info.pVertexBindingDescriptions = vertex_input_bindings.data();
    vertex_input_info.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(vertex_input_attributes.size());
    vertex_input_info.pVertexAttributeDescriptions =
        vertex_input_attributes.data();

    auto vertexShader = _load_shader(vertex_shader_path.c_str());
    auto fragmentShader = _load_shader(fragment_shader_path.c_str());
    std::array<VkPipelineShaderStageCreateInfo, 2> shader_info{};
    shader_info[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_info[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shader_info[0].pName = "main";
    shader_info[0].module = vertexShader;
    shader_info[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shader_info[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shader_info[1].pName = "main";
    shader_info[1].module = fragmentShader;

    VkPipeline pipeline;
    VkGraphicsPipelineCreateInfo pipeline_info{};
    pipeline_info.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipeline_info.pInputAssemblyState = &input_assembly_info;
    pipeline_info.pViewportState = &viewport_info;
    pipeline_info.pRasterizationState = &rasterization_info;
    pipeline_info.pMultisampleState = &multisample_info;
    pipeline_info.pDepthStencilState = &depth_stencil_info;
    pipeline_info.pColorBlendState = &blend_info;
    pipeline_info.pDynamicState = &dynamic_states_info;
    pipeline_info.pTessellationState = &tess_state_info;
    pipeline_info.pVertexInputState = &vertex_input_info;
    pipeline_info.stageCount = static_cast<uint32_t>(shader_info.size());
    pipeline_info.pStages = shader_info.data();
    pipeline_info.layout = _pipeline_layouts[shader];
    pipeline_info.renderPass = _render_pass;
    pipeline_info.subpass = 0;
    if (vkCreateGraphicsPipelines(
            _device, nullptr, 1, &pipeline_info, nullptr, &pipeline) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    _pipelines[shader] = pipeline;

    vkDestroyShaderModule(_device, fragmentShader, nullptr);
    vkDestroyShaderModule(_device, vertexShader, nullptr);
}

inline void Rasterizer::_render_scene(
    const Eigen::Matrix4f& model,
    const RasCamera& camera,
    const std::vector<VkBuffer>& active_buffers,
    ShaderType shader)
{
    // Update tranform
    Transform transform(model, camera.view(), camera.projection());
    void* data;
    vkMapMemory(
        _device, _transform_buffer_memory, 0, sizeof(Transform), 0, &data);
    memcpy(data, &transform, sizeof(Transform));
    vkUnmapMemory(_device, _transform_buffer_memory);
    VkDescriptorBufferInfo buffer_info{};
    buffer_info.buffer = _transform_buffer;
    buffer_info.offset = 0;
    buffer_info.range = sizeof(Transform);
    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = _descriptor_sets[shader];
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &buffer_info;
    write.pImageInfo = nullptr;
    write.pTexelBufferView = nullptr;
    vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);

    VkCommandBufferAllocateInfo cmd_buffer_info{};
    cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd_buffer_info.commandPool = _command_pool;
    cmd_buffer_info.commandBufferCount = 1;
    VkCommandBuffer cmd_buffer;
    if (vkAllocateCommandBuffers(_device, &cmd_buffer_info, &cmd_buffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo cmd_buffer_begin_info{};
    cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    cmd_buffer_begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    if (vkBeginCommandBuffer(cmd_buffer, &cmd_buffer_begin_info) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    VkClearValue clear_values[4];
    clear_values[0].color = {
        { _background.x(), _background.y(), _background.z(), 1.0f }
    };
    clear_values[1].depthStencil = { 1.0f, 0 };
    clear_values[2].color = {
        { _background.x(), _background.y(), _background.z(), 1.0f }
    };
    clear_values[3].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo render_pass_begin_info{};
    render_pass_begin_info.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    render_pass_begin_info.renderArea.extent.width = _width;
    render_pass_begin_info.renderArea.extent.height = _height;
    render_pass_begin_info.clearValueCount = 4;
    render_pass_begin_info.pClearValues = clear_values;
    render_pass_begin_info.renderPass = _render_pass;
    render_pass_begin_info.framebuffer = _framebuffer;
    vkCmdBeginRenderPass(
        cmd_buffer, &render_pass_begin_info, VK_SUBPASS_CONTENTS_INLINE);

    vkCmdBindPipeline(
        cmd_buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelines[shader]);

    std::vector<VkDeviceSize> offsets(active_buffers.size(), 0);
    vkCmdBindVertexBuffers(cmd_buffer,
                           0,
                           active_buffers.size(),
                           active_buffers.data(),
                           offsets.data());
    if (_num_indices != 0) {
        vkCmdBindIndexBuffer(
            cmd_buffer, _index_buffer, 0, VK_INDEX_TYPE_UINT32);
    }

    vkCmdBindDescriptorSets(cmd_buffer,
                            VK_PIPELINE_BIND_POINT_GRAPHICS,
                            _pipeline_layouts[shader],
                            0,
                            1,
                            &_descriptor_sets[shader],
                            0,
                            nullptr);

    if (_num_indices != 0) {
        vkCmdDrawIndexed(cmd_buffer, _num_indices, 1, 0, 0, 0);
    }
    else if (_num_vertices != 0) {
        vkCmdDraw(cmd_buffer, _num_vertices, 1, 0, 0);
    }

    vkCmdEndRenderPass(cmd_buffer);

    if (vkEndCommandBuffer(cmd_buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    if (vkCreateFence(_device, &fence_info, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buffer;
    if (vkQueueSubmit(_queue, 1, &submit_info, fence)) {
        throw std::runtime_error("failed to submit queue!");
    }

    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, 1000000) != VK_SUCCESS) {
        throw std::runtime_error("failed to complete rendering!");
    }
    vkDestroyFence(_device, fence, nullptr);
}

inline void Rasterizer::_copy_fb_to_host_buffer(VkImage src,
                                                VkImageAspectFlags aspect,
                                                VkBuffer dst)
{
    VkCommandBufferAllocateInfo cmd_buffer_allocate_info{};
    cmd_buffer_allocate_info.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buffer_allocate_info.commandPool = _command_pool;
    cmd_buffer_allocate_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd_buffer_allocate_info.commandBufferCount = 1;
    VkCommandBuffer cmd_buffer;
    vkAllocateCommandBuffers(_device, &cmd_buffer_allocate_info, &cmd_buffer);

    VkCommandBufferBeginInfo cmd_buffer_begin_info{};
    cmd_buffer_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    vkBeginCommandBuffer(cmd_buffer, &cmd_buffer_begin_info);

    VkBufferImageCopy region{};
    region.imageSubresource.aspectMask = aspect;
    region.imageSubresource.layerCount = 1;
    region.imageExtent.width = _width;
    region.imageExtent.height = _height;
    region.imageExtent.depth = 1;
    region.bufferRowLength = 0;
    region.bufferImageHeight = 0;
    vkCmdCopyImageToBuffer(
        cmd_buffer, src, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, dst, 1, &region);

    vkEndCommandBuffer(cmd_buffer);

    VkFenceCreateInfo fence_info{};
    fence_info.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    VkFence fence;
    vkCreateFence(_device, &fence_info, nullptr, &fence);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buffer;
    vkQueueSubmit(_queue, 1, &submit_info, fence);

    vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX);
    vkDestroyFence(_device, fence, nullptr);
}

inline void Rasterizer::_transfer_data(const void* data,
                                       size_t byte_size,
                                       VkBufferUsageFlagBits usage,
                                       VkBuffer& buffer,
                                       VkDeviceMemory& buffer_memory)
{
    VkDeviceSize buffer_size = byte_size;

    // Create staging buffer
    VkBuffer staging_buffer;
    VkDeviceMemory staging_memory;
    _create_buffer(staging_buffer,
                   staging_memory,
                   buffer_size,
                   VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                       VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    // Copy data to staging buffer
    void* mapped;
    vkMapMemory(_device, staging_memory, 0, buffer_size, 0, &mapped);
    memcpy(mapped, data, byte_size);
    vkUnmapMemory(_device, staging_memory);

    // Create vertex buffer
    _create_buffer(buffer,
                   buffer_memory,
                   buffer_size,
                   VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
                   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    // Issue copy command
    _copy_buffer(staging_buffer, buffer, buffer_size);

    _release_buffer(staging_buffer, staging_memory);
}

inline uint32_t Rasterizer::_find_proper_memory_type(
    uint32_t mem_type_bits,
    VkMemoryPropertyFlags prop_flags)
{
    uint32_t t = 0;
    for (uint32_t i = 0; i < _physical_device_memory_properties.memoryTypeCount;
         ++i) {
        if ((mem_type_bits & (1 << i)) &&
            (_physical_device_memory_properties.memoryTypes[i].propertyFlags &
             prop_flags) == prop_flags) {
            t = i;
            break;
        }
    }
    return t;
}

inline void Rasterizer::_create_buffer(VkBuffer& buffer,
                                       VkDeviceMemory& memory,
                                       VkDeviceSize size,
                                       VkBufferUsageFlags usage,
                                       VkMemoryPropertyFlags props)
{
    VkBufferCreateInfo buffer_info{};
    buffer_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    buffer_info.usage = usage;
    buffer_info.size = size;
    buffer_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(_device, &buffer_info, nullptr, &buffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }

    VkMemoryRequirements mem_reqs;
    vkGetBufferMemoryRequirements(_device, buffer, &mem_reqs);
    VkMemoryAllocateInfo mem_alloc_info{};
    mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc_info.allocationSize = mem_reqs.size;
    mem_alloc_info.memoryTypeIndex =
        _find_proper_memory_type(mem_reqs.memoryTypeBits, props);
    if (vkAllocateMemory(_device, &mem_alloc_info, nullptr, &memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }

    vkBindBufferMemory(_device, buffer, memory, 0);
}

inline void Rasterizer::_release_buffer(VkBuffer& buffer,
                                        VkDeviceMemory& buffer_memory)
{
    vkDestroyBuffer(_device, buffer, nullptr);
    vkFreeMemory(_device, buffer_memory, nullptr);
    buffer = VK_NULL_HANDLE;
}

inline void Rasterizer::_copy_buffer(VkBuffer src,
                                     VkBuffer dst,
                                     VkDeviceSize size)
{
    VkCommandBufferAllocateInfo cmd_buffer_info{};
    cmd_buffer_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    cmd_buffer_info.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    cmd_buffer_info.commandPool = _command_pool;
    cmd_buffer_info.commandBufferCount = 1;
    VkCommandBuffer cmd_buffer;
    if (vkAllocateCommandBuffers(_device, &cmd_buffer_info, &cmd_buffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo begin_info{};
    begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    begin_info.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
    vkBeginCommandBuffer(cmd_buffer, &begin_info);

    VkBufferCopy copy{};
    copy.srcOffset = 0;
    copy.dstOffset = 0;
    copy.size = size;
    vkCmdCopyBuffer(cmd_buffer, src, dst, 1, &copy);

    vkEndCommandBuffer(cmd_buffer);

    VkSubmitInfo submit_info{};
    submit_info.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submit_info.commandBufferCount = 1;
    submit_info.pCommandBuffers = &cmd_buffer;
    vkQueueSubmit(_queue, 1, &submit_info, VK_NULL_HANDLE);
    vkQueueWaitIdle(_queue);

    vkFreeCommandBuffers(_device, _command_pool, 1, &cmd_buffer);
}

inline void Rasterizer::_create_image(VkImage& image,
                                      VkDeviceMemory& memory,
                                      VkFormat format,
                                      VkImageUsageFlags usage,
                                      VkImageTiling tiling,
                                      VkMemoryPropertyFlags prop_flags,
                                      uint32_t width,
                                      uint32_t height,
                                      VkSampleCountFlagBits samples)
{
    if (image != VK_NULL_HANDLE) { _release_image(image, memory); }

    VkImageCreateInfo image_info{};
    image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    image_info.imageType = VK_IMAGE_TYPE_2D;
    image_info.format = format;
    image_info.usage = usage;
    image_info.extent.width = width;
    image_info.extent.height = height;
    image_info.extent.depth = 1;
    image_info.mipLevels = 1;
    image_info.arrayLayers = 1;
    image_info.samples = samples;
    image_info.tiling = tiling;
    image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateImage(_device, &image_info, nullptr, &image) != VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }

    VkMemoryRequirements mem_reqs;
    vkGetImageMemoryRequirements(_device, image, &mem_reqs);
    VkMemoryAllocateInfo mem_alloc_info{};
    mem_alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    mem_alloc_info.allocationSize = mem_reqs.size;
    mem_alloc_info.memoryTypeIndex =
        _find_proper_memory_type(mem_reqs.memoryTypeBits, prop_flags);
    if (vkAllocateMemory(_device, &mem_alloc_info, nullptr, &memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(_device, image, memory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }
}

inline void Rasterizer::_release_image(VkImage& image, VkDeviceMemory& memory)
{
    vkDestroyImage(_device, image, nullptr);
    vkFreeMemory(_device, memory, nullptr);
    image = VK_NULL_HANDLE;
}

inline void Rasterizer::_create_image_view(VkImageView& view,
                                           VkImage image,
                                           VkFormat format,
                                           VkImageAspectFlags aspect)
{
    if (view != VK_NULL_HANDLE) { vkDestroyImageView(_device, view, nullptr); }

    VkImageViewCreateInfo image_view_info{};
    image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    image_view_info.image = image;
    image_view_info.format = format;
    image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
    image_view_info.subresourceRange = {};
    image_view_info.subresourceRange.aspectMask = aspect;
    image_view_info.subresourceRange.baseMipLevel = 0;
    image_view_info.subresourceRange.levelCount = 1;
    image_view_info.subresourceRange.baseArrayLayer = 0;
    image_view_info.subresourceRange.layerCount = 1;
    if (vkCreateImageView(_device, &image_view_info, nullptr, &view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
}

inline void Rasterizer::_create_framebuffer(uint32_t width, uint32_t height)
{
    if (_framebuffer != VK_NULL_HANDLE) {
        vkDestroyFramebuffer(_device, _framebuffer, nullptr);
    }
    std::vector<VkImageView> attachments;
    if (_samples == SAMPLE_COUNT_1) {
        attachments = { _color_image_view, _depth_image_view };
    }
    else {
        attachments = { _color_image_view,
                        _depth_image_view,
                        _color_resolve_image_view,
                        _depth_resolve_image_view };
    }
    VkFramebufferCreateInfo fb_info{};
    fb_info.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    fb_info.renderPass = _render_pass;
    fb_info.attachmentCount = attachments.size();
    fb_info.pAttachments = attachments.data();
    fb_info.width = width;
    fb_info.height = height;
    fb_info.layers = 1;
    if (vkCreateFramebuffer(_device, &fb_info, nullptr, &_framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create frame buffer!");
    }
}

inline VkShaderModule Rasterizer::_load_shader(const char* path)
{
    std::ifstream is(path, std::ios::binary | std::ios::in | std::ios::ate);

    if (is.is_open()) {
        size_t size = is.tellg();
        is.seekg(0, std::ios::beg);
        char* shader_src = new char[size];
        is.read(shader_src, size);
        is.close();

        assert(size > 0);

        VkShaderModule shader_module;
        VkShaderModuleCreateInfo module_info{};
        module_info.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        module_info.codeSize = size;
        module_info.pCode = (uint32_t*)shader_src;

        if (vkCreateShaderModule(_device, &module_info, NULL, &shader_module) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        delete[] shader_src;

        return shader_module;
    }
    else {
        std::cerr << "Error: Could not open shader file \"" << path << "\""
                  << std::endl;
        return VK_NULL_HANDLE;
    }
}

inline void Rasterizer::_update_ubo(const VkDescriptorBufferInfo& info,
                                    uint32_t binding,
                                    ShaderType shader)
{
    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = _descriptor_sets[shader];
    write.dstBinding = binding;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    write.descriptorCount = 1;
    write.pBufferInfo = &info;
    write.pImageInfo = nullptr;
    write.pTexelBufferView = nullptr;
    vkUpdateDescriptorSets(_device, 1, &write, 0, nullptr);
}

} // namespace Euclid
