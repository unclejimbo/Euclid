#include <fstream>
#include <iostream>

namespace Euclid
{

namespace _impl
{
/*some functions*/
VkBufferCreateInfo bufferCreateInfo()
{
    VkBufferCreateInfo bufCreateInfo{};
    bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    return bufCreateInfo;
}

VkBufferCreateInfo bufferCreateInfo(VkBufferUsageFlags usage, VkDeviceSize size)
{
    VkBufferCreateInfo bufCreateInfo{};
    bufCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufCreateInfo.usage = usage;
    bufCreateInfo.size = size;
    return bufCreateInfo;
}

VkMemoryAllocateInfo memoryAllocateInfo()
{
    VkMemoryAllocateInfo memAllocInfo{};
    memAllocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    return memAllocInfo;
}

VkSubmitInfo submitInfo()
{
    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    return submitInfo;
}

VkFenceCreateInfo fenceCreateInfo(VkFenceCreateFlags flags = 0)
{
    VkFenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceCreateInfo.flags = flags;
    return fenceCreateInfo;
}

VkCommandBufferAllocateInfo commandBufferAllocateInfo(
    VkCommandPool commandPool,
    VkCommandBufferLevel level,
    uint32_t bufferCount)
{
    VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
    commandBufferAllocateInfo.sType =
        VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.level = level;
    commandBufferAllocateInfo.commandBufferCount = bufferCount;
    return commandBufferAllocateInfo;
}

VkCommandBufferBeginInfo commandBufferBeginInfo()
{
    VkCommandBufferBeginInfo cmdBufferBeginInfo{};
    cmdBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    return cmdBufferBeginInfo;
}

VkBool32 getSupportedDepthFormat(VkPhysicalDevice _physical_device,
                                 VkFormat* depthFormat)
{
    // Since all depth formats may be optional, we need to find a suitable depth
    // format to use Start with the highest precision packed format
    std::vector<VkFormat> depthFormats = { VK_FORMAT_D32_SFLOAT_S8_UINT,
                                           VK_FORMAT_D32_SFLOAT,
                                           VK_FORMAT_D24_UNORM_S8_UINT,
                                           VK_FORMAT_D16_UNORM_S8_UINT,
                                           VK_FORMAT_D16_UNORM };

    for (auto& format : depthFormats) {
        VkFormatProperties formatProps;
        vkGetPhysicalDeviceFormatProperties(
            _physical_device, format, &formatProps);
        // Format must support depth stencil attachment for optimal tiling
        if (formatProps.optimalTilingFeatures &
            VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT) {
            *depthFormat = format;
            return true;
        }
    }

    return false;
}

VkImageCreateInfo imageCreateInfo()
{
    VkImageCreateInfo imageCreateInfo{};
    imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    return imageCreateInfo;
}

VkImageViewCreateInfo imageViewCreateInfo()
{
    VkImageViewCreateInfo imageViewCreateInfo{};
    imageViewCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    return imageViewCreateInfo;
}

VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo(
    const std::vector<VkDescriptorSetLayoutBinding>& bindings)
{
    VkDescriptorSetLayoutCreateInfo descriptorSetLayoutCreateInfo{};
    descriptorSetLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
    descriptorSetLayoutCreateInfo.pBindings = bindings.data();
    descriptorSetLayoutCreateInfo.bindingCount =
        static_cast<uint32_t>(bindings.size());
    return descriptorSetLayoutCreateInfo;
}

VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo(
    const VkDescriptorSetLayout* pSetLayouts,
    uint32_t setLayoutCount = 1)
{
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo{};
    pipelineLayoutCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutCreateInfo.setLayoutCount = setLayoutCount;
    pipelineLayoutCreateInfo.pSetLayouts = pSetLayouts;
    return pipelineLayoutCreateInfo;
}

VkPushConstantRange pushConstantRange(VkShaderStageFlags stageFlags,
                                      uint32_t size,
                                      uint32_t offset)
{
    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = stageFlags;
    pushConstantRange.offset = offset;
    pushConstantRange.size = size;
    return pushConstantRange;
}

VkPipelineInputAssemblyStateCreateInfo pipelineInputAssemblyStateCreateInfo(
    VkPrimitiveTopology topology,
    VkPipelineInputAssemblyStateCreateFlags flags,
    VkBool32 primitiveRestartEnable)
{
    VkPipelineInputAssemblyStateCreateInfo
        pipelineInputAssemblyStateCreateInfo{};
    pipelineInputAssemblyStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    pipelineInputAssemblyStateCreateInfo.topology = topology;
    pipelineInputAssemblyStateCreateInfo.flags = flags;
    pipelineInputAssemblyStateCreateInfo.primitiveRestartEnable =
        primitiveRestartEnable;
    return pipelineInputAssemblyStateCreateInfo;
}

VkPipelineRasterizationStateCreateInfo pipelineRasterizationStateCreateInfo(
    VkPolygonMode polygonMode,
    VkCullModeFlags cullMode,
    VkFrontFace frontFace,
    VkPipelineRasterizationStateCreateFlags flags = 0)
{
    VkPipelineRasterizationStateCreateInfo
        pipelineRasterizationStateCreateInfo{};
    pipelineRasterizationStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    pipelineRasterizationStateCreateInfo.polygonMode = polygonMode;
    pipelineRasterizationStateCreateInfo.cullMode = cullMode;
    pipelineRasterizationStateCreateInfo.frontFace = frontFace;
    pipelineRasterizationStateCreateInfo.flags = flags;
    pipelineRasterizationStateCreateInfo.depthClampEnable = VK_FALSE;
    pipelineRasterizationStateCreateInfo.lineWidth = 1.0f;
    return pipelineRasterizationStateCreateInfo;
}

VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState(
    VkColorComponentFlags colorWriteMask,
    VkBool32 blendEnable)
{
    VkPipelineColorBlendAttachmentState pipelineColorBlendAttachmentState{};
    pipelineColorBlendAttachmentState.colorWriteMask = colorWriteMask;
    pipelineColorBlendAttachmentState.blendEnable = blendEnable;
    return pipelineColorBlendAttachmentState;
}

VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo(
    uint32_t attachmentCount,
    const VkPipelineColorBlendAttachmentState* pAttachments)
{
    VkPipelineColorBlendStateCreateInfo pipelineColorBlendStateCreateInfo{};
    pipelineColorBlendStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    pipelineColorBlendStateCreateInfo.attachmentCount = attachmentCount;
    pipelineColorBlendStateCreateInfo.pAttachments = pAttachments;
    return pipelineColorBlendStateCreateInfo;
}

VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo(
    VkBool32 depthTestEnable,
    VkBool32 depthWriteEnable,
    VkCompareOp depthCompareOp)
{
    VkPipelineDepthStencilStateCreateInfo pipelineDepthStencilStateCreateInfo{};
    pipelineDepthStencilStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    pipelineDepthStencilStateCreateInfo.depthTestEnable = depthTestEnable;
    pipelineDepthStencilStateCreateInfo.depthWriteEnable = depthWriteEnable;
    pipelineDepthStencilStateCreateInfo.depthCompareOp = depthCompareOp;
    pipelineDepthStencilStateCreateInfo.front =
        pipelineDepthStencilStateCreateInfo.back;
    pipelineDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
    return pipelineDepthStencilStateCreateInfo;
}

VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo(
    uint32_t viewportCount,
    uint32_t scissorCount,
    VkPipelineViewportStateCreateFlags flags = 0)
{
    VkPipelineViewportStateCreateInfo pipelineViewportStateCreateInfo{};
    pipelineViewportStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    pipelineViewportStateCreateInfo.viewportCount = viewportCount;
    pipelineViewportStateCreateInfo.scissorCount = scissorCount;
    pipelineViewportStateCreateInfo.flags = flags;
    return pipelineViewportStateCreateInfo;
}

VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo(
    VkSampleCountFlagBits rasterizationSamples,
    VkPipelineMultisampleStateCreateFlags flags = 0)
{
    VkPipelineMultisampleStateCreateInfo pipelineMultisampleStateCreateInfo{};
    pipelineMultisampleStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    pipelineMultisampleStateCreateInfo.rasterizationSamples =
        rasterizationSamples;
    pipelineMultisampleStateCreateInfo.flags = flags;
    return pipelineMultisampleStateCreateInfo;
}

VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo(
    const std::vector<VkDynamicState>& pDynamicStates,
    VkPipelineDynamicStateCreateFlags flags = 0)
{
    VkPipelineDynamicStateCreateInfo pipelineDynamicStateCreateInfo{};
    pipelineDynamicStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    pipelineDynamicStateCreateInfo.pDynamicStates = pDynamicStates.data();
    pipelineDynamicStateCreateInfo.dynamicStateCount =
        static_cast<uint32_t>(pDynamicStates.size());
    pipelineDynamicStateCreateInfo.flags = flags;
    return pipelineDynamicStateCreateInfo;
}

VkGraphicsPipelineCreateInfo pipelineCreateInfo(VkPipelineLayout layout,
                                                VkRenderPass _render_pass,
                                                VkPipelineCreateFlags flags = 0)
{
    VkGraphicsPipelineCreateInfo pipelineCreateInfo{};
    pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineCreateInfo.layout = layout;
    pipelineCreateInfo.renderPass = _render_pass;
    pipelineCreateInfo.flags = flags;
    pipelineCreateInfo.basePipelineIndex = -1;
    pipelineCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
    return pipelineCreateInfo;
}

VkVertexInputBindingDescription vertexInputBindingDescription(
    uint32_t binding,
    uint32_t stride,
    VkVertexInputRate inputRate)
{
    VkVertexInputBindingDescription vInputBindDescription{};
    vInputBindDescription.binding = binding;
    vInputBindDescription.stride = stride;
    vInputBindDescription.inputRate = inputRate;
    return vInputBindDescription;
}

VkVertexInputAttributeDescription vertexInputAttributeDescription(
    uint32_t binding,
    uint32_t location,
    VkFormat format,
    uint32_t offset)
{
    VkVertexInputAttributeDescription vInputAttribDescription{};
    vInputAttribDescription.location = location;
    vInputAttribDescription.binding = binding;
    vInputAttribDescription.format = format;
    vInputAttribDescription.offset = offset;
    return vInputAttribDescription;
}

VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo()
{
    VkPipelineVertexInputStateCreateInfo pipelineVertexInputStateCreateInfo{};
    pipelineVertexInputStateCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    return pipelineVertexInputStateCreateInfo;
}

VkShaderModule loadShader(const char* fileName, VkDevice device)
{
    std::ifstream is(fileName, std::ios::binary | std::ios::in | std::ios::ate);

    if (is.is_open()) {
        size_t size = is.tellg();
        is.seekg(0, std::ios::beg);
        char* shaderCode = new char[size];
        is.read(shaderCode, size);
        is.close();

        assert(size > 0);

        VkShaderModule shaderModule;
        VkShaderModuleCreateInfo moduleCreateInfo{};
        moduleCreateInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        moduleCreateInfo.codeSize = size;
        moduleCreateInfo.pCode = (uint32_t*)shaderCode;

        if (vkCreateShaderModule(
                device, &moduleCreateInfo, NULL, &shaderModule) != VK_SUCCESS) {
            throw std::runtime_error("failed to create shader module!");
        }

        delete[] shaderCode;

        return shaderModule;
    }
    else {
        std::cerr << "Error: Could not open shader file \"" << fileName << "\""
                  << std::endl;
        return VK_NULL_HANDLE;
    }
}

VkImageMemoryBarrier imageMemoryBarrier()
{
    VkImageMemoryBarrier imageMemoryBarrier{};
    imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    return imageMemoryBarrier;
}

void insertImageMemoryBarrier(VkCommandBuffer cmdbuffer,
                              VkImage image,
                              VkAccessFlags srcAccessMask,
                              VkAccessFlags dstAccessMask,
                              VkImageLayout oldImageLayout,
                              VkImageLayout newImageLayout,
                              VkPipelineStageFlags srcStageMask,
                              VkPipelineStageFlags dstStageMask,
                              VkImageSubresourceRange subresourceRange)
{
    VkImageMemoryBarrier imageMemoryBarrier = _impl::imageMemoryBarrier();
    imageMemoryBarrier.srcAccessMask = srcAccessMask;
    imageMemoryBarrier.dstAccessMask = dstAccessMask;
    imageMemoryBarrier.oldLayout = oldImageLayout;
    imageMemoryBarrier.newLayout = newImageLayout;
    imageMemoryBarrier.image = image;
    imageMemoryBarrier.subresourceRange = subresourceRange;

    vkCmdPipelineBarrier(cmdbuffer,
                         srcStageMask,
                         dstStageMask,
                         0,
                         0,
                         nullptr,
                         0,
                         nullptr,
                         1,
                         &imageMemoryBarrier);
}

VkFramebufferCreateInfo framebufferCreateInfo()
{
    VkFramebufferCreateInfo framebufferCreateInfo{};
    framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    return framebufferCreateInfo;
}

static VKAPI_ATTR VkBool32 VKAPI_CALL
debugMessageCallback(VkDebugReportFlagsEXT flags,
                     VkDebugReportObjectTypeEXT objectType,
                     uint64_t object,
                     size_t location,
                     int32_t messageCode,
                     const char* pLayerPrefix,
                     const char* pMessage,
                     void* pUserData)
{

    return VK_FALSE;
}

} // namespace _impl

/********************** RasCamera **********************/

inline Euclid::RasCamera::RasCamera(const Eigen::Vector3f& position,
                                    const Eigen::Vector3f& focus,
                                    const Eigen::Vector3f& up)
    : Camera(position, focus, up)
{}

inline Eigen::Matrix4f Euclid::RasCamera::view() const
{
    Eigen::Matrix3f v;
    v.col(0) = this->u;
    v.col(1) = this->v;
    v.col(2) = this->dir;

    Eigen::Matrix4f view_mat;
    view_mat.setZero();
    view_mat.topLeftCorner<3, 3>() = v.transpose();
    view_mat.topRightCorner<3, 1>() = -v.transpose() * this->pos;
    view_mat(3, 3) = 1.0f;

    return view_mat;
}

inline Euclid::PerspRasCamera::PerspRasCamera(const Eigen::Vector3f& position,
                                              const Eigen::Vector3f& focus,
                                              const Eigen::Vector3f& up,
                                              float vfov,
                                              float aspect,
                                              float tnear,
                                              float tfar)
    : RasCamera(position, focus, up)
{
    _vfov = vfov * boost::math::float_constants::degree;
    _aspect = aspect;
    _tnear = tnear;
    _tfar = tfar;
}

inline Euclid::PerspRasCamera::PerspRasCamera(const Eigen::Vector3f& position,
                                              const Eigen::Vector3f& focus,
                                              const Eigen::Vector3f& up,
                                              float vfov,
                                              unsigned width,
                                              unsigned height,
                                              float tnear,
                                              float tfar)
    : RasCamera(position, focus, up)
{
    _vfov = vfov * boost::math::float_constants::degree;
    _aspect = static_cast<float>(width) / height;
    _tnear = tnear;
    _tfar = tfar;
}

inline void Euclid::PerspRasCamera::set_aspect(float aspect)
{
    _aspect = aspect;
}

inline void Euclid::PerspRasCamera::set_aspect(unsigned width, unsigned height)
{
    _aspect = static_cast<float>(width) / height;
}

inline void Euclid::PerspRasCamera::set_fov(float vfov)
{
    _vfov = vfov * boost::math::float_constants::degree;
}

inline void Euclid::PerspRasCamera::set_range(float tnear, float tfar)
{
    _tnear = tnear;
    _tfar = tfar;
}

inline Eigen::Matrix4f Euclid::PerspRasCamera::projection() const
{
    auto yscale = std::tan(0.5f * _vfov);
    auto xscale = yscale * _aspect;
    auto d = 1.0f / (_tfar - _tnear);

    Eigen::Matrix4f proj_mat;
    proj_mat.setZero();
    proj_mat(0, 0) = 1.0f / xscale;
    proj_mat(1, 1) = 1.0f / yscale;
    proj_mat(2, 2) = -(_tfar + _tnear) * d;
    proj_mat(2, 3) = -2.0f * _tfar * _tnear * d;
    proj_mat(3, 2) = -1.0f;

    return proj_mat;
}

inline Euclid::OrthoRasCamera::OrthoRasCamera(const Eigen::Vector3f& position,
                                              const Eigen::Vector3f& focus,
                                              const Eigen::Vector3f& up,
                                              float xextent,
                                              float yextent)
    : RasCamera(position, focus, up)
{
    _x = 2.0f / xextent;
    _y = 2.0f / yextent;
}

inline void Euclid::OrthoRasCamera::set_extent(float xextent, float yextent)
{
    _x = 2.0f / xextent;
    _y = 2.0f / yextent;
}

inline Eigen::Matrix4f Euclid::OrthoRasCamera::projection() const
{
    Eigen::Matrix4f proj_mat;
    proj_mat.setZero();
    proj_mat(0, 0) = _x;
    proj_mat(1, 1) = _y;
    proj_mat(2, 2) = -1.0f;
    proj_mat(3, 3) = 1.0f;

    return proj_mat;
}

/************************ Rsterizer *************************/

inline Euclid::Rasterizer::Rasterizer()
{
    // material
    __material.ambient << 0.1f, 0.1f, 0.1f;
    __material.diffuse << 0.7f, 0.7f, 0.7f;

    // vulkan application
    VkApplicationInfo appInfo = {};
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pApplicationName = "Euclid rasterizer";
    appInfo.pEngineName = "Euclid";
    appInfo.apiVersion = VK_API_VERSION_1_0;

    // Vulkan instance creation (without surface extensions)
    VkInstanceCreateInfo instanceCreateInfo = {};
    instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceCreateInfo.pApplicationInfo = &appInfo;

    uint32_t layerCount = 0;

    const char* validationLayers[] = { "VK_LAYER_LUNARG_standard_validation" };
    layerCount = 1;

#ifndef NDEBUG
    // Check if layers are available
    uint32_t instanceLayerCount;
    vkEnumerateInstanceLayerProperties(&instanceLayerCount, nullptr);
    std::vector<VkLayerProperties> instanceLayers(instanceLayerCount);
    vkEnumerateInstanceLayerProperties(&instanceLayerCount,
                                       instanceLayers.data());

    bool layersAvailable = true;
    for (auto layerName : validationLayers) {
        bool layerAvailable = false;
        for (auto instanceLayer : instanceLayers) {
            if (strcmp(instanceLayer.layerName, layerName) == 0) {
                layerAvailable = true;
                break;
            }
        }
        if (!layerAvailable) {
            layersAvailable = false;
            break;
        }
    }

    if (layersAvailable) {
        instanceCreateInfo.ppEnabledLayerNames = validationLayers;
        const char* validationExt = VK_EXT_DEBUG_REPORT_EXTENSION_NAME;
        instanceCreateInfo.enabledLayerCount = layerCount;
        instanceCreateInfo.enabledExtensionCount = 1;
        instanceCreateInfo.ppEnabledExtensionNames = &validationExt;
    }
#endif
    if (vkCreateInstance(&instanceCreateInfo, nullptr, &_instance) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create instance!");
    }

#ifndef NDEBUG
    if (layersAvailable) {
        VkDebugReportCallbackCreateInfoEXT debugReportCreateInfo = {};
        debugReportCreateInfo.sType =
            VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
        debugReportCreateInfo.flags =
            VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
        debugReportCreateInfo.pfnCallback =
            (PFN_vkDebugReportCallbackEXT)_impl::debugMessageCallback;

        // We have to explicitly load this function.
        PFN_vkCreateDebugReportCallbackEXT vkCreateDebugReportCallbackEXT =
            reinterpret_cast<PFN_vkCreateDebugReportCallbackEXT>(
                vkGetInstanceProcAddr(_instance,
                                      "vkCreateDebugReportCallbackEXT"));
        assert(vkCreateDebugReportCallbackEXT);
        if (vkCreateDebugReportCallbackEXT(_instance,
                                           &debugReportCreateInfo,
                                           nullptr,
                                           &_debug_report_callback) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create debugreportcallback!");
        }
    }
#endif

    // physical device creation
    uint32_t deviceCount = 0;
    if (vkEnumeratePhysicalDevices(_instance, &deviceCount, nullptr) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create physical device!");
    }
    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    if (vkEnumeratePhysicalDevices(
            _instance, &deviceCount, physicalDevices.data()) != VK_SUCCESS) {
        throw std::runtime_error("failed to create physical device!");
    }
    _physical_device = physicalDevices[0];

    VkPhysicalDeviceProperties deviceProperties;
    vkGetPhysicalDeviceProperties(_physical_device, &deviceProperties);

    // Request a single graphics queue
    const float defaultQueuePriority(0.0f);
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    uint32_t queueFamilyCount;
    vkGetPhysicalDeviceQueueFamilyProperties(
        _physical_device, &queueFamilyCount, nullptr);
    std::vector<VkQueueFamilyProperties> queueFamilyProperties(
        queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(
        _physical_device, &queueFamilyCount, queueFamilyProperties.data());
    for (uint32_t i = 0;
         i < static_cast<uint32_t>(queueFamilyProperties.size());
         i++) {
        if (queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
            _queue_family_index = i;
            queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
            queueCreateInfo.queueFamilyIndex = i;
            queueCreateInfo.queueCount = 1;
            queueCreateInfo.pQueuePriorities = &defaultQueuePriority;
            break;
        }
    }

    // Create logical device
    VkDeviceCreateInfo deviceCreateInfo = {};
    deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    if (vkCreateDevice(
            _physical_device, &deviceCreateInfo, nullptr, &_device) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create logical device!");
    }

    // Get a graphics queue
    vkGetDeviceQueue(_device, _queue_family_index, 0, &_queue);

    // Command pool
    VkCommandPoolCreateInfo cmdPoolInfo = {};
    cmdPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    cmdPoolInfo.queueFamilyIndex = _queue_family_index;
    cmdPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    if (vkCreateCommandPool(_device, &cmdPoolInfo, nullptr, &_command_pool) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create command pool!");
    }
}

inline Euclid::Rasterizer::~Rasterizer() {}

inline void Euclid::Rasterizer::attach_geometry_buffers(
    const std::vector<float>& positions,
    const std::vector<unsigned>& indices)
{
    std::vector<Vertex> vertices; // final vertex vector used for rendering
    std::vector<Vertex_Color>
        vertices_color; // vector used for binding color buffer
    std::vector<Vertex>
        vertices_ras; // transform the position array into the vertex form
    std::vector<unsigned> indices_ras; // index

    std::vector<Vertex_Index> vertices_index;

    for (int i = 0; i + 2 < positions.size(); i += 3) {
        Vertex tmp;
        tmp.position[0] = positions[i];
        tmp.position[1] = positions[i + 1];
        tmp.position[2] = positions[i + 2];
        vertices_ras.push_back(tmp);
    }
    for (int i = 0; i < indices.size(); i++) {
        indices_ras.push_back(indices[i]);
    }

    // if with color buffer
    if (render_with_color_buffer == true) {
        if (_vertex_color == false) {
            for (int i = 0; i + 2 < indices_ras.size(); i += 3) {
                Vertex_Color tmp;
                // color
                tmp.color[0] = _face_colors[i];     // r
                tmp.color[1] = _face_colors[i + 1]; // g
                tmp.color[2] = _face_colors[i + 2]; // b

                // position
                tmp.position[0] = vertices_ras[indices_ras[i]].position[0];
                tmp.position[1] = vertices_ras[indices_ras[i]].position[1];
                tmp.position[2] = vertices_ras[indices_ras[i]].position[2];
                vertices_color.push_back(tmp);
                tmp.position[0] = vertices_ras[indices_ras[i + 1]].position[0];
                tmp.position[1] = vertices_ras[indices_ras[i + 1]].position[1];
                tmp.position[2] = vertices_ras[indices_ras[i + 1]].position[2];
                vertices_color.push_back(tmp);
                tmp.position[0] = vertices_ras[indices_ras[i + 2]].position[0];
                tmp.position[1] = vertices_ras[indices_ras[i + 2]].position[1];
                tmp.position[2] = vertices_ras[indices_ras[i + 2]].position[2];
                vertices_color.push_back(tmp);
            }
        }
        else if (_vertex_color == true) {
            for (int i = 0; i < indices_ras.size(); i++) {
                Vertex_Color tmp;

                // color
                tmp.color[0] = _face_colors[3 * indices_ras[i]];     // r
                tmp.color[1] = _face_colors[3 * indices_ras[i] + 1]; // g
                tmp.color[2] = _face_colors[3 * indices_ras[i] + 2]; // b

                // position
                tmp.position[0] = vertices_ras[indices_ras[i]].position[0];
                tmp.position[1] = vertices_ras[indices_ras[i]].position[1];
                tmp.position[2] = vertices_ras[indices_ras[i]].position[2];
                vertices_color.push_back(tmp);
            }
        }
        if (_face_mask != nullptr) // eliminate vertex based on _face_mask
        {
            int offset = 0;
            for (int i = 0; i + 2 < indices_ras.size(); i += 3) {
                if (_face_mask[i / 3] == 0) {
                    vertices_color.erase(vertices_color.begin() + offset,
                                         vertices_color.begin() + 3 + offset);
                }
                else {
                    offset += 3;
                }
            }
        }
    }
    else if (render_with_index == true) {
        for (unsigned i = 3; i + 2 < indices_ras.size();
             i += 3) // every three vertex represents a face
        {
            Vertex_Index tmp;
            uint8_t r = ((i / 3) + 1) & 0x000000FF;
            uint8_t g = (((i / 3) + 1) >> 8) & 0x000000FF;
            uint8_t b = (((i / 3) + 1) >> 16) & 0x000000FF;
            tmp.color[0] = r / 255.0f;
            tmp.color[1] = g / 255.0f;
            tmp.color[2] = b / 255.0f; // each face shares a same color
            tmp.position[0] = vertices_ras[indices_ras[i]].position[0];
            tmp.position[1] = vertices_ras[indices_ras[i]].position[1];
            tmp.position[2] = vertices_ras[indices_ras[i]].position[2];
            vertices_index.push_back(tmp);
            tmp.position[0] = vertices_ras[indices_ras[i + 1]].position[0];
            tmp.position[1] = vertices_ras[indices_ras[i + 1]].position[1];
            tmp.position[2] = vertices_ras[indices_ras[i + 1]].position[2];
            vertices_index.push_back(tmp);
            tmp.position[0] = vertices_ras[indices_ras[i + 2]].position[0];
            tmp.position[1] = vertices_ras[indices_ras[i + 2]].position[1];
            tmp.position[2] = vertices_ras[indices_ras[i + 2]].position[2];
            vertices_index.push_back(tmp);
        }
    }
    else {
        for (int i = 0; i + 2 < indices_ras.size(); i += 3) {
            Vertex tmp;
            Eigen::Vector3f a, b, normal;
            a = Eigen::Vector3f(vertices_ras[indices_ras[i + 1]].position[0] -
                                    vertices_ras[indices_ras[i]].position[0],
                                vertices_ras[indices_ras[i + 1]].position[1] -
                                    vertices_ras[indices_ras[i]].position[1],
                                vertices_ras[indices_ras[i + 1]].position[2] -
                                    vertices_ras[indices_ras[i]].position[2]);
            b = Eigen::Vector3f(vertices_ras[indices_ras[i + 2]].position[0] -
                                    vertices_ras[indices_ras[i]].position[0],
                                vertices_ras[indices_ras[i + 2]].position[1] -
                                    vertices_ras[indices_ras[i]].position[1],
                                vertices_ras[indices_ras[i + 2]].position[2] -
                                    vertices_ras[indices_ras[i]].position[2]);

            normal = a.cross(b).normalized();

            tmp.normal[0] = normal[0];
            tmp.normal[1] = normal[1];
            tmp.normal[2] = normal[2];

            tmp.position[0] = vertices_ras[indices_ras[i]].position[0];
            tmp.position[1] = vertices_ras[indices_ras[i]].position[1];
            tmp.position[2] = vertices_ras[indices_ras[i]].position[2];
            vertices.push_back(tmp);
            tmp.position[0] = vertices_ras[indices_ras[i + 1]].position[0];
            tmp.position[1] = vertices_ras[indices_ras[i + 1]].position[1];
            tmp.position[2] = vertices_ras[indices_ras[i + 1]].position[2];
            vertices.push_back(tmp);
            tmp.position[0] = vertices_ras[indices_ras[i + 2]].position[0];
            tmp.position[1] = vertices_ras[indices_ras[i + 2]].position[1];
            tmp.position[2] = vertices_ras[indices_ras[i + 2]].position[2];
            vertices.push_back(tmp);
        }
        if (_face_mask != nullptr) // eliminate vertex based on _face_mask
        {
            int offset = 0;
            for (int i = 0; i + 2 < indices_ras.size(); i += 3) {
                if (_face_mask[i / 3] == 0) {
                    vertices.erase(vertices.begin() + offset,
                                   vertices.begin() + 3 + offset);
                }
                else {
                    offset += 3;
                }
            }
        }
    }

    index_size = indices_ras.size();
    if (_face_mask != nullptr) { index_size = vertices.size(); }
    VkDeviceSize vertexBufferSize;
    if (render_with_color_buffer == true) {
        vertexBufferSize = vertices_color.size() * sizeof(Vertex_Color);
    }
    else if (render_with_index == true) {
        vertexBufferSize = vertices_index.size() * sizeof(Vertex_Index);
    }
    else {
        vertexBufferSize = vertices.size() * sizeof(Vertex);
    }
    const VkDeviceSize indexBufferSize = indices.size() * sizeof(unsigned);

    VkBuffer stagingBuffer;
    VkDeviceMemory stagingMemory;

    // Command buffer for copy commands (reused)
    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        _impl::commandBufferAllocateInfo(
            _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    VkCommandBuffer copyCmd;
    if (vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &copyCmd) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create command buffer!");
    }
    VkCommandBufferBeginInfo cmdBufInfo = _impl::commandBufferBeginInfo();

    // Copy input data to VRAM using a staging buffer
    // Vertices
    VkBufferCreateInfo bufferCreateInfo;
    VkMemoryRequirements memReqs;
    VkMemoryAllocateInfo memAlloc = _impl::memoryAllocateInfo();

    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical_device,
                                        &deviceMemoryProperties);
    if (render_with_color_buffer == true) {
        bufferCreateInfo = _impl::bufferCreateInfo(
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, vertexBufferSize);
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(
                _device, &bufferCreateInfo, nullptr, &stagingBuffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }
        vkGetBufferMemoryRequirements(_device, *&stagingBuffer, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = -1;
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & 1) == 1) {
                if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                    ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                    memAlloc.memoryTypeIndex = i;
                    break;
                }
            }
            memReqs.memoryTypeBits >>= 1;
        }
        if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
        if (vkAllocateMemory(_device, &memAlloc, nullptr, &stagingMemory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate memory!");
        }

        if (vertices_color.data() != nullptr) {
            void* mapped;
            if (vkMapMemory(_device,
                            *&stagingMemory,
                            0,
                            vertexBufferSize,
                            0,
                            &mapped) != VK_SUCCESS) {
                throw std::runtime_error("vkMapMemory failed!");
            }
            memcpy(mapped, vertices_color.data(), vertexBufferSize);
            vkUnmapMemory(_device, *&stagingMemory);
        }

        if (vkBindBufferMemory(_device, *&stagingBuffer, *&stagingMemory, 0) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to bind buffer memory!");
        }
        bufferCreateInfo =
            _impl::bufferCreateInfo(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                    vertexBufferSize);

        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(
                _device, &bufferCreateInfo, nullptr, &_vertex_buffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }
        vkGetBufferMemoryRequirements(_device, *&_vertex_buffer, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = -1;
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & 1) == 1) {
                if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                    memAlloc.memoryTypeIndex = i;
                    break;
                }
            }
            memReqs.memoryTypeBits >>= 1;
        }
        if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex; }
        if (vkAllocateMemory(_device, &memAlloc, nullptr, &_vertex_memory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate memory!");
        }
        if (vkBindBufferMemory(
                _device, *&_vertex_buffer, *&_vertex_memory, 0) != VK_SUCCESS) {
            throw std::runtime_error("failed to bind buffer memory!");
        }
    }
    else if (render_with_index == true) {
        bufferCreateInfo = _impl::bufferCreateInfo(
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, vertexBufferSize);
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(
                _device, &bufferCreateInfo, nullptr, &stagingBuffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }
        vkGetBufferMemoryRequirements(_device, *&stagingBuffer, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = -1;
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & 1) == 1) {
                if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                    ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                    memAlloc.memoryTypeIndex = i;
                    break;
                }
            }
            memReqs.memoryTypeBits >>= 1;
        }
        if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
        if (vkAllocateMemory(_device, &memAlloc, nullptr, &stagingMemory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate memory!");
        }

        if (vertices_index.data() != nullptr) {
            void* mapped;
            if (vkMapMemory(_device,
                            *&stagingMemory,
                            0,
                            vertexBufferSize,
                            0,
                            &mapped) != VK_SUCCESS) {
                throw std::runtime_error("vkMapMemory failed!");
            }
            memcpy(mapped, vertices_index.data(), vertexBufferSize);
            vkUnmapMemory(_device, *&stagingMemory);
        }

        if (vkBindBufferMemory(_device, *&stagingBuffer, *&stagingMemory, 0) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to bind buffer memory!");
        }
        bufferCreateInfo =
            _impl::bufferCreateInfo(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                    vertexBufferSize);
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(
                _device, &bufferCreateInfo, nullptr, &_vertex_buffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }
        vkGetBufferMemoryRequirements(_device, *&_vertex_buffer, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = -1;
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & 1) == 1) {
                if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                    memAlloc.memoryTypeIndex = i;
                    break;
                }
            }
            memReqs.memoryTypeBits >>= 1;
        }
        if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex; }
        if (vkAllocateMemory(_device, &memAlloc, nullptr, &_vertex_memory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate memory!");
        }
        if (vkBindBufferMemory(
                _device, *&_vertex_buffer, *&_vertex_memory, 0) != VK_SUCCESS) {
            throw std::runtime_error("failed to bind buffer memory!");
        }
    }
    else {
        bufferCreateInfo = _impl::bufferCreateInfo(
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, vertexBufferSize);
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(
                _device, &bufferCreateInfo, nullptr, &stagingBuffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }
        vkGetBufferMemoryRequirements(_device, *&stagingBuffer, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = -1;
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & 1) == 1) {
                if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                      VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                    ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                      VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                    memAlloc.memoryTypeIndex = i;
                    break;
                }
            }
            memReqs.memoryTypeBits >>= 1;
        }
        if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
        if (vkAllocateMemory(_device, &memAlloc, nullptr, &stagingMemory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate memory!");
        }

        if (vertices.data() != nullptr) {
            void* mapped;
            if (vkMapMemory(_device,
                            *&stagingMemory,
                            0,
                            vertexBufferSize,
                            0,
                            &mapped) != VK_SUCCESS) {
                throw std::runtime_error("vkMapMemory failed!");
            }
            memcpy(mapped, vertices.data(), vertexBufferSize);
            vkUnmapMemory(_device, *&stagingMemory);
        }

        if (vkBindBufferMemory(_device, *&stagingBuffer, *&stagingMemory, 0) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to bind buffer memory!");
        }
        bufferCreateInfo =
            _impl::bufferCreateInfo(VK_BUFFER_USAGE_VERTEX_BUFFER_BIT |
                                        VK_BUFFER_USAGE_TRANSFER_DST_BIT,
                                    vertexBufferSize);
        bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        if (vkCreateBuffer(
                _device, &bufferCreateInfo, nullptr, &_vertex_buffer) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to create buffer!");
        }
        vkGetBufferMemoryRequirements(_device, *&_vertex_buffer, &memReqs);
        memAlloc.allocationSize = memReqs.size;
        memAlloc.memoryTypeIndex = -1;
        for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
            if ((memReqs.memoryTypeBits & 1) == 1) {
                if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                    memAlloc.memoryTypeIndex = i;
                    break;
                }
            }
            memReqs.memoryTypeBits >>= 1;
        }
        if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex; }
        if (vkAllocateMemory(_device, &memAlloc, nullptr, &_vertex_memory) !=
            VK_SUCCESS) {
            throw std::runtime_error("failed to allocate memory!");
        }
        if (vkBindBufferMemory(
                _device, *&_vertex_buffer, *&_vertex_memory, 0) != VK_SUCCESS) {
            throw std::runtime_error("failed to bind buffer memory!");
        }
    }
    if (vkBeginCommandBuffer(copyCmd, &cmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to create vertex buffer!");
    }

    VkBufferCopy copyRegion = {};
    copyRegion.size = vertexBufferSize;
    vkCmdCopyBuffer(copyCmd, stagingBuffer, _vertex_buffer, 1, &copyRegion);
    if (vkEndCommandBuffer(copyCmd) != VK_SUCCESS) {
        throw std::runtime_error(
            "failed to end command buffer while creating vertex buffer!");
    }
    VkSubmitInfo submitInfo = _impl::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;
    VkFenceCreateInfo fenceInfo = _impl::fenceCreateInfo();
    VkFence fence;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    vkDestroyBuffer(_device, stagingBuffer, nullptr);
    vkFreeMemory(_device, stagingMemory, nullptr);

    // Indices
    bufferCreateInfo = _impl::bufferCreateInfo(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                                               indexBufferSize);
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(_device, &bufferCreateInfo, nullptr, &stagingBuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }
    vkGetBufferMemoryRequirements(_device, *&stagingBuffer, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                memAlloc.memoryTypeIndex = i;
                break;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(_device, &memAlloc, nullptr, &stagingMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }

    if (indices_ras.data() != nullptr) {
        void* mapped;
        if (vkMapMemory(
                _device, *&stagingMemory, 0, indexBufferSize, 0, &mapped) !=
            VK_SUCCESS) {
            throw std::runtime_error("vkMapMemory failed!");
        }
        memcpy(mapped, indices_ras.data(), indexBufferSize);
        vkUnmapMemory(_device, *&stagingMemory);
    }

    if (vkBindBufferMemory(_device, *&stagingBuffer, *&stagingMemory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind buffer memory!");
    }
    bufferCreateInfo = _impl::bufferCreateInfo(
        VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
        indexBufferSize);
    bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    if (vkCreateBuffer(_device, &bufferCreateInfo, nullptr, &_index_buffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create buffer!");
    }
    vkGetBufferMemoryRequirements(_device, *&_index_buffer, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
                break;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex; }
    if (vkAllocateMemory(_device, &memAlloc, nullptr, &_index_memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindBufferMemory(_device, *&_index_buffer, *&_index_memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind buffer memory!");
    }

    if (vkBeginCommandBuffer(copyCmd, &cmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to create index buffer!");
    }
    copyRegion.size = indexBufferSize;
    vkCmdCopyBuffer(copyCmd, stagingBuffer, _index_buffer, 1, &copyRegion);
    if (vkEndCommandBuffer(copyCmd) != VK_SUCCESS) {
        throw std::runtime_error(
            "failed to end command buffer while creating index buffer!");
    }
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    vkDestroyBuffer(_device, stagingBuffer, nullptr);
    vkFreeMemory(_device, stagingMemory, nullptr);
}

inline void Euclid::Rasterizer::attach_color_buffer(const float* colors,
                                                    bool vertex_color)
{
    if (colors == nullptr) {
        _face_colors = colors;
        render_with_color_buffer = false;
        _vertex_color = vertex_color;
    }
    else {
        _face_colors = colors;
        render_with_color_buffer = true;
        _vertex_color = vertex_color;
    }
}

inline void Euclid::Rasterizer::attach_face_mask_buffer(const uint8_t* mask)
{
    _face_mask = mask;
}

inline void Euclid::Rasterizer::release_buffers() {}

inline void Euclid::Rasterizer::set_material(const Material& material)
{
    __material = material;
}

inline void Euclid::Rasterizer::set_background(
    const Eigen::Ref<const Eigen::Array3f>& color)
{
    _background = color;
}

inline void Euclid::Rasterizer::set_background(float r, float g, float b)
{
    _background << r, g, b;
}

inline void Euclid::Rasterizer::enable_light(bool on)
{
    _lighting = on;
}

inline void Euclid::Rasterizer::render_shaded(std::vector<uint8_t>& pixels,
                                              const RasCamera& camera,
                                              int width,
                                              int height,
                                              bool interleaved)
{
    std::array<Eigen::Matrix4f, 2> pushConstants;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical_device,
                                        &deviceMemoryProperties);
    /*
    Create framebuffer attachments
    */
    VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat;

    _impl::getSupportedDepthFormat(_physical_device, &depthFormat);

    // Color attachment
    VkImageCreateInfo image = _impl::imageCreateInfo();
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = colorFormat;
    image.extent.width = width;
    image.extent.height = height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    VkMemoryAllocateInfo memAlloc = _impl::memoryAllocateInfo();
    VkMemoryRequirements memReqs;

    if (vkCreateImage(_device, &image, nullptr, &_color_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _color_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_color_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _color_attachment.image, _color_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo colorImageView = _impl::imageViewCreateInfo();
    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageView.format = colorFormat;
    colorImageView.subresourceRange = {};
    colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorImageView.subresourceRange.baseMipLevel = 0;
    colorImageView.subresourceRange.levelCount = 1;
    colorImageView.subresourceRange.baseArrayLayer = 0;
    colorImageView.subresourceRange.layerCount = 1;
    colorImageView.image = _color_attachment.image;
    if (vkCreateImageView(
            _device, &colorImageView, nullptr, &_color_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    // Depth stencil attachment
    image.format = depthFormat;
    image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (vkCreateImage(_device, &image, nullptr, &_depth_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _depth_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_depth_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _depth_attachment.image, _depth_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo depthStencilView = _impl::imageViewCreateInfo();
    depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format = depthFormat;
    depthStencilView.flags = 0;
    depthStencilView.subresourceRange = {};
    depthStencilView.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel = 0;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = _depth_attachment.image;
    if (vkCreateImageView(
            _device, &depthStencilView, nullptr, &_depth_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    /*
    Create renderpass
    */

    std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
    // Color attachment
    attchmentDescriptions[0].format = colorFormat;
    attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    // Depth attachment
    attchmentDescriptions[1].format = depthFormat;
    attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[1].finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {
        0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    VkAttachmentReference depthReference = {
        1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount =
        static_cast<uint32_t>(attchmentDescriptions.size());
    renderPassInfo.pAttachments = attchmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_render_pass) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create renderpass!");
    }
    VkImageView attachments[2];
    attachments[0] = _color_attachment.view;
    attachments[1] = _depth_attachment.view;

    VkFramebufferCreateInfo framebufferCreateInfo =
        _impl::framebufferCreateInfo();
    framebufferCreateInfo.renderPass = _render_pass;
    framebufferCreateInfo.attachmentCount = 2;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = 1;
    if (vkCreateFramebuffer(
            _device, &framebufferCreateInfo, nullptr, &_framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create frame buffer!");
    }

    /*
    Prepare graphics pipeline
    */

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {};
    VkDescriptorSetLayoutCreateInfo descriptorLayout =
        _impl::descriptorSetLayoutCreateInfo(setLayoutBindings);
    if (vkCreateDescriptorSetLayout(
            _device, &descriptorLayout, nullptr, &_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
        _impl::pipelineLayoutCreateInfo(nullptr, 0);

    // MVP via push constant block
    VkPushConstantRange pushConstantRange = _impl::pushConstantRange(
        VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstants), 0);
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(
            _device, &pipelineLayoutCreateInfo, nullptr, &_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (vkCreatePipelineCache(
            _device, &pipelineCacheCreateInfo, nullptr, &_pipeline_cache) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline cache!");
    }

    // Create pipeline
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        _impl::pipelineInputAssemblyStateCreateInfo(
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        _impl::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL,
                                                    VK_CULL_MODE_BACK_BIT,
                                                    VK_FRONT_FACE_CLOCKWISE);

    VkPipelineColorBlendAttachmentState blendAttachmentState =
        _impl::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

    VkPipelineColorBlendStateCreateInfo colorBlendState =
        _impl::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        _impl::pipelineDepthStencilStateCreateInfo(
            VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

    VkPipelineViewportStateCreateInfo viewportState =
        _impl::pipelineViewportStateCreateInfo(1, 1);

    VkPipelineMultisampleStateCreateInfo multisampleState =
        _impl::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState =
        _impl::pipelineDynamicStateCreateInfo(dynamicStateEnables);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo =
        _impl::pipelineCreateInfo(_pipeline_layout, _render_pass);

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    // Vertex bindings an attributes
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    VkPipelineVertexInputStateCreateInfo vertexInputState;

    if (render_with_color_buffer == true) {
        // Binding description
        vertexInputBindings = {
            _impl::vertexInputBindingDescription(
                0, sizeof(Vertex_Color), VK_VERTEX_INPUT_RATE_VERTEX),
        };

        // Attribute descriptions
        vertexInputAttributes = {
            _impl::vertexInputAttributeDescription(
                0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
            _impl::vertexInputAttributeDescription(
                0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Normal
            _impl::vertexInputAttributeDescription(
                0, 2, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 6), // Color
        };

        vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
        vertexInputState.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions =
            vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions =
            vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        shaderStages[0].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pName = "main";
        shaderStages[1].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pName = "main";
        std::string vert_color = std::string(DATA_DIR) + "vert_color.spv";
        std::string frag_color = std::string(DATA_DIR) + "frag_color.spv";
        shaderStages[0].module = _impl::loadShader(vert_color.data(), _device);
        shaderStages[1].module = _impl::loadShader(frag_color.data(), _device);
    }
    else {

        // Binding description
        vertexInputBindings = {
            _impl::vertexInputBindingDescription(
                0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
        };

        // Attribute descriptions
        vertexInputAttributes = {
            _impl::vertexInputAttributeDescription(
                0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
            _impl::vertexInputAttributeDescription(
                0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Normal
        };

        vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
        vertexInputState.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions =
            vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions =
            vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        shaderStages[0].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pName = "main";
        shaderStages[1].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pName = "main";
        std::string vert = std::string(DATA_DIR) + "vert.spv";
        std::string frag = std::string(DATA_DIR) + "frag.spv";
        shaderStages[0].module = _impl::loadShader(vert.data(), _device);
        shaderStages[1].module = _impl::loadShader(frag.data(), _device);
    }

    _shader_modules = { shaderStages[0].module, shaderStages[1].module };
    if (vkCreateGraphicsPipelines(_device,
                                  _pipeline_cache,
                                  1,
                                  &pipelineCreateInfo,
                                  nullptr,
                                  &_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    /*
    Command buffer creation
    */

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        _impl::commandBufferAllocateInfo(
            _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    if (vkAllocateCommandBuffers(
            _device, &cmdBufAllocateInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo cmdBufInfo = _impl::commandBufferBeginInfo();

    if (vkBeginCommandBuffer(commandBuffer, &cmdBufInfo)) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    // set the background color
    VkClearValue clearValues[2];
    // clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
    clearValues[0].color = {
        { _background.x(), _background.y(), _background.z(), 1.0f }
    };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderArea.extent.width = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;
    renderPassBeginInfo.renderPass = _render_pass;
    renderPassBeginInfo.framebuffer = _framebuffer;

    vkCmdBeginRenderPass(
        commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.height = (float)height;
    viewport.width = (float)width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Update dynamic scissor state
    VkRect2D scissor = {};
    scissor.extent.width = width;
    scissor.extent.height = height;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

    // Render scene
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_vertex_buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _index_buffer, 0, VK_INDEX_TYPE_UINT32);
    Eigen::Matrix4f mvpMatrix = camera.projection() * camera.view();
    pushConstants[0] = mvpMatrix;

    Eigen::Matrix4f ubo;
    ubo(0, 0) = camera.pos[0];
    ubo(1, 0) = camera.pos[1];
    ubo(2, 0) = camera.pos[2];
    ubo(3, 0) = 0.0f;
    ubo(0, 1) = __material.ambient[0];
    ubo(1, 1) = __material.ambient[1];
    ubo(2, 1) = __material.ambient[2];
    ubo(3, 1) = 0.0f;
    ubo(0, 2) = __material.diffuse[0];
    ubo(1, 2) = __material.diffuse[1];
    ubo(2, 2) = __material.diffuse[2];
    ubo(3, 2) = 0.0f;

    if (_lighting == true) {
        ubo(0, 3) = 1.0f;
        ubo(1, 3) = 1.0f;
        ubo(2, 3) = 1.0f;
    }
    else {
        ubo(0, 3) = 0.0f;
        ubo(1, 3) = 0.0f;
        ubo(2, 3) = 0.0f;
    }
    ubo(3, 3) = 0.0f;

    pushConstants[1] = ubo;
    vkCmdPushConstants(commandBuffer,
                       _pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(pushConstants),
                       pushConstants.data());
    vkCmdDraw(commandBuffer, index_size, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(commandBuffer, queue);
    VkSubmitInfo submitInfo = _impl::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkFenceCreateInfo fenceInfo = _impl::fenceCreateInfo();
    VkFence fence;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    vkDeviceWaitIdle(_device);

    /*
    Copy framebuffer image to host visible image
    */
    const char* imagedata;
    // Create the linear tiled destination image to copy to and to read the
    // memory from
    VkImageCreateInfo imgCreateInfo(_impl::imageCreateInfo());
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imgCreateInfo.extent.width = width;
    imgCreateInfo.extent.height = height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // Create the image
    VkImage dstImage;
    if (vkCreateImage(_device, &imgCreateInfo, nullptr, &dstImage) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to end create image!");
    }
    // Create memory to back up the image
    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo memAllocInfo(_impl::memoryAllocateInfo());
    VkDeviceMemory dstImageMemory;
    vkGetImageMemoryRequirements(_device, dstImage, &memRequirements);
    memAllocInfo.allocationSize = memRequirements.size;
    memAllocInfo.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & 1) == 1) {
            if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                memAllocInfo.memoryTypeIndex = i;
            }
        }
        memRequirements.memoryTypeBits >>= 1;
    }
    if (memAllocInfo.memoryTypeIndex == -1) {
        memAllocInfo.memoryTypeIndex = 0;
    }
    if (vkAllocateMemory(_device, &memAllocInfo, nullptr, &dstImageMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(_device, dstImage, dstImageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    // Do the actual blit from the offscreen image to our host visible
    // destination image
    cmdBufAllocateInfo = _impl::commandBufferAllocateInfo(
        _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    VkCommandBuffer copyCmd;
    if (vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &copyCmd) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    cmdBufInfo = _impl::commandBufferBeginInfo();
    if (vkBeginCommandBuffer(copyCmd, &cmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    VkImageMemoryBarrier imageMemoryBarrier = _impl::imageMemoryBarrier();

    // Transition destination image to transfer destination layout
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    // _color_attachment.image is already in
    // VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, and does not need to be
    // transitioned

    VkImageCopy imageCopyRegion{};
    imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.srcSubresource.layerCount = 1;
    imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.dstSubresource.layerCount = 1;
    imageCopyRegion.extent.width = width;
    imageCopyRegion.extent.height = height;
    imageCopyRegion.extent.depth = 1;

    vkCmdCopyImage(copyCmd,
                   _color_attachment.image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dstImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageCopyRegion);

    // Transition destination image to general layout, which is the required
    // layout for mapping the image memory later on
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    if (vkEndCommandBuffer(copyCmd) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(copyCmd, queue);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    // Get layout of the image (including row pitch)
    VkImageSubresource subResource{};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    VkSubresourceLayout subResourceLayout;

    vkGetImageSubresourceLayout(
        _device, dstImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    vkMapMemory(
        _device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&imagedata);
    imagedata += subResourceLayout.offset;

    /*
    Save host visible framebuffer image to pixels
    */

    // If source is BGR (destination is always RGB) and we can't use blit (which
    // does automatic conversion), we'll have to manually swizzle color
    // components
    bool colorSwizzle = false;
    // Check if source is BGR and needs swizzle
    std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB,
                                         VK_FORMAT_B8G8R8A8_UNORM,
                                         VK_FORMAT_B8G8R8A8_SNORM };
    colorSwizzle = (std::find(formatsBGR.begin(),
                              formatsBGR.end(),
                              VK_FORMAT_R8G8B8A8_UNORM) != formatsBGR.end());
    uint8_t r, g, b;
    for (int32_t y = 0; y < height; y++) {
        unsigned int* row = (unsigned int*)imagedata;
        for (int32_t x = 0; x < width; x++) {
            if (colorSwizzle) {
                r = *row >> 16;
                g = *row >> 8;
                b = *row;
            }
            else {
                r = *row;
                g = *row >> 8;
                b = *row >> 16;
            }
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
            row++;
        }
        imagedata += subResourceLayout.rowPitch;
    }

    // Clean up resources
    vkUnmapMemory(_device, dstImageMemory);
    vkFreeMemory(_device, dstImageMemory, nullptr);
    vkDestroyImage(_device, dstImage, nullptr);

    vkQueueWaitIdle(_queue);
}

inline void Euclid::Rasterizer::render_shaded(std::vector<uint8_t>& pixels,
                                              const RasCamera& camera,
                                              int width,
                                              int height,
                                              int samples,
                                              bool interleaved)
{
    std::array<Eigen::Matrix4f, 2> pushConstants;
    VkSampleCountFlagBits msaaSamples;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical_device,
                                        &deviceMemoryProperties);
    switch (samples) {
    case 1: msaaSamples = VK_SAMPLE_COUNT_1_BIT; break;
    case 2: msaaSamples = VK_SAMPLE_COUNT_2_BIT; break;
    case 4: msaaSamples = VK_SAMPLE_COUNT_4_BIT; break;
    case 8: msaaSamples = VK_SAMPLE_COUNT_8_BIT; break;
    case 16: msaaSamples = VK_SAMPLE_COUNT_16_BIT; break;
    case 32: msaaSamples = VK_SAMPLE_COUNT_32_BIT; break;
    case 64: msaaSamples = VK_SAMPLE_COUNT_64_BIT; break;
    default: break;
    }

    /*
    Create framebuffer attachments
    */
    VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat;

    _impl::getSupportedDepthFormat(_physical_device, &depthFormat);

    // Color attachment
    VkImageCreateInfo image = _impl::imageCreateInfo();
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = colorFormat;
    image.extent.width = width;
    image.extent.height = height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    // image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.samples = msaaSamples;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    VkMemoryAllocateInfo memAlloc = _impl::memoryAllocateInfo();
    VkMemoryRequirements memReqs;

    if (vkCreateImage(_device, &image, nullptr, &_color_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _color_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_color_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _color_attachment.image, _color_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo colorImageView = _impl::imageViewCreateInfo();
    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageView.format = colorFormat;
    colorImageView.subresourceRange = {};
    colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorImageView.subresourceRange.baseMipLevel = 0;
    colorImageView.subresourceRange.levelCount = 1;
    colorImageView.subresourceRange.baseArrayLayer = 0;
    colorImageView.subresourceRange.layerCount = 1;
    colorImageView.image = _color_attachment.image;
    if (vkCreateImageView(
            _device, &colorImageView, nullptr, &_color_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
    image.format = colorFormat;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.usage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
    if (vkCreateImage(_device, &image, nullptr, &_dst_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }

    vkGetImageMemoryRequirements(_device, _dst_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_dst_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _dst_attachment.image, _dst_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }
    VkImageViewCreateInfo dstImageView = _impl::imageViewCreateInfo();
    dstImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    dstImageView.format = colorFormat;
    dstImageView.subresourceRange = {};
    dstImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    dstImageView.subresourceRange.baseMipLevel = 0;
    dstImageView.subresourceRange.levelCount = 1;
    dstImageView.subresourceRange.baseArrayLayer = 0;
    dstImageView.subresourceRange.layerCount = 1;
    dstImageView.image = _dst_attachment.image;
    if (vkCreateImageView(
            _device, &dstImageView, nullptr, &_dst_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    // Depth stencil attachment
    image.format = depthFormat;
    image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
    image.samples = msaaSamples;
    if (vkCreateImage(_device, &image, nullptr, &_depth_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _depth_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_depth_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _depth_attachment.image, _depth_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo depthStencilView = _impl::imageViewCreateInfo();
    depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format = depthFormat;
    depthStencilView.flags = 0;
    depthStencilView.subresourceRange = {};
    depthStencilView.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel = 0;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = _depth_attachment.image;
    if (vkCreateImageView(
            _device, &depthStencilView, nullptr, &_depth_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
    /*
    Create renderpass
    */

    std::array<VkAttachmentDescription, 3> attchmentDescriptions = {};
    // Color attachment
    attchmentDescriptions[0].format = colorFormat;
    // attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[0].samples = msaaSamples;
    attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    // attchmentDescriptions[0].finalLayout =
    // VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    attchmentDescriptions[0].finalLayout =
        VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
    // Depth attachment
    attchmentDescriptions[1].format = depthFormat;
    // attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[1].samples = msaaSamples;
    attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[1].finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    attchmentDescriptions[2].format = colorFormat;
    attchmentDescriptions[2].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[2].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[2].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[2].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[2].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

    VkAttachmentReference colorReference = {
        0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    VkAttachmentReference depthReference = {
        1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };
    VkAttachmentReference resolveReference = {
        2, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;
    subpassDescription.pResolveAttachments = &resolveReference;

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount =
        static_cast<uint32_t>(attchmentDescriptions.size());
    renderPassInfo.pAttachments = attchmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_render_pass) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create renderpass!");
    }
    VkImageView attachments[3];
    attachments[0] = _color_attachment.view;
    attachments[1] = _depth_attachment.view;
    attachments[2] = _dst_attachment.view;
    // attachments[3] = depthStencil.view;
    VkFramebufferCreateInfo framebufferCreateInfo =
        _impl::framebufferCreateInfo();
    framebufferCreateInfo.renderPass = _render_pass;
    framebufferCreateInfo.attachmentCount = 3;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = 1;
    if (vkCreateFramebuffer(
            _device, &framebufferCreateInfo, nullptr, &_framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create frame buffer!");
    }
    /*
    Prepare graphics pipeline
    */

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {};
    VkDescriptorSetLayoutCreateInfo descriptorLayout =
        _impl::descriptorSetLayoutCreateInfo(setLayoutBindings);
    if (vkCreateDescriptorSetLayout(
            _device, &descriptorLayout, nullptr, &_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
        _impl::pipelineLayoutCreateInfo(nullptr, 0);

    // MVP via push constant block
    VkPushConstantRange pushConstantRange = _impl::pushConstantRange(
        VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstants), 0);
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(
            _device, &pipelineLayoutCreateInfo, nullptr, &_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (vkCreatePipelineCache(
            _device, &pipelineCacheCreateInfo, nullptr, &_pipeline_cache) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline cache!");
    }

    // Create pipeline
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        _impl::pipelineInputAssemblyStateCreateInfo(
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        _impl::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL,
                                                    VK_CULL_MODE_BACK_BIT,
                                                    VK_FRONT_FACE_CLOCKWISE);

    VkPipelineColorBlendAttachmentState blendAttachmentState =
        _impl::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

    VkPipelineColorBlendStateCreateInfo colorBlendState =
        _impl::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        _impl::pipelineDepthStencilStateCreateInfo(
            VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

    VkPipelineViewportStateCreateInfo viewportState =
        _impl::pipelineViewportStateCreateInfo(1, 1);

    VkPipelineMultisampleStateCreateInfo multisampleState =
        // Euclid::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);
        _impl::pipelineMultisampleStateCreateInfo(msaaSamples);
    // multisampleState.sampleShadingEnable = VK_TRUE;
    // multisampleState.minSampleShading = 0.25f;

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState =
        _impl::pipelineDynamicStateCreateInfo(dynamicStateEnables);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo =
        _impl::pipelineCreateInfo(_pipeline_layout, _render_pass);

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    // Vertex bindings an attributes
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    VkPipelineVertexInputStateCreateInfo vertexInputState;

    if (render_with_color_buffer == true) {
        // Binding description
        vertexInputBindings = {
            _impl::vertexInputBindingDescription(
                0, sizeof(Vertex_Color), VK_VERTEX_INPUT_RATE_VERTEX),
        };

        // Attribute descriptions
        vertexInputAttributes = {
            _impl::vertexInputAttributeDescription(
                0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
            _impl::vertexInputAttributeDescription(
                0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Normal
            _impl::vertexInputAttributeDescription(
                0, 2, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 6), // Color
        };

        vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
        vertexInputState.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions =
            vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions =
            vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        shaderStages[0].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pName = "main";
        shaderStages[1].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pName = "main";
        std::string vert_color = std::string(DATA_DIR) + "vert_color.spv";
        std::string frag_color = std::string(DATA_DIR) + "frag_color.spv";
        shaderStages[0].module = _impl::loadShader(vert_color.data(), _device);
        shaderStages[1].module = _impl::loadShader(frag_color.data(), _device);
    }
    else {

        // Binding description
        vertexInputBindings = {
            _impl::vertexInputBindingDescription(
                0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
        };

        // Attribute descriptions
        vertexInputAttributes = {
            _impl::vertexInputAttributeDescription(
                0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
            _impl::vertexInputAttributeDescription(
                0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Normal
        };

        vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
        vertexInputState.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions =
            vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions =
            vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        shaderStages[0].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pName = "main";
        shaderStages[1].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pName = "main";
        std::string vert = std::string(DATA_DIR) + "vert.spv";
        std::string frag = std::string(DATA_DIR) + "frag.spv";
        shaderStages[0].module = _impl::loadShader(vert.data(), _device);
        shaderStages[1].module = _impl::loadShader(frag.data(), _device);
    }

    _shader_modules = { shaderStages[0].module, shaderStages[1].module };
    if (vkCreateGraphicsPipelines(_device,
                                  _pipeline_cache,
                                  1,
                                  &pipelineCreateInfo,
                                  nullptr,
                                  &_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    /*
    Command buffer creation
    */

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        _impl::commandBufferAllocateInfo(
            _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    if (vkAllocateCommandBuffers(
            _device, &cmdBufAllocateInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo cmdBufInfo = _impl::commandBufferBeginInfo();

    if (vkBeginCommandBuffer(commandBuffer, &cmdBufInfo)) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    // set the background color
    VkClearValue clearValues[2];
    // clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
    clearValues[0].color = {
        { _background.x(), _background.y(), _background.z(), 1.0f }
    };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderArea.extent.width = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;
    renderPassBeginInfo.renderPass = _render_pass;
    renderPassBeginInfo.framebuffer = _framebuffer;

    vkCmdBeginRenderPass(
        commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.height = (float)height;
    viewport.width = (float)width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Update dynamic scissor state
    VkRect2D scissor = {};
    scissor.extent.width = width;
    scissor.extent.height = height;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

    // Render scene
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_vertex_buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _index_buffer, 0, VK_INDEX_TYPE_UINT32);
    Eigen::Matrix4f mvpMatrix = camera.projection() * camera.view();
    pushConstants[0] = mvpMatrix;

    Eigen::Matrix4f ubo;
    ubo(0, 0) = camera.pos[0];
    ubo(1, 0) = camera.pos[1];
    ubo(2, 0) = camera.pos[2];
    ubo(3, 0) = 0.0f;
    ubo(0, 1) = __material.ambient[0];
    ubo(1, 1) = __material.ambient[1];
    ubo(2, 1) = __material.ambient[2];
    ubo(3, 1) = 0.0f;
    ubo(0, 2) = __material.diffuse[0];
    ubo(1, 2) = __material.diffuse[1];
    ubo(2, 2) = __material.diffuse[2];
    ubo(3, 2) = 0.0f;
    if (_lighting == true) {
        ubo(0, 3) = 1.0f;
        ubo(1, 3) = 1.0f;
        ubo(2, 3) = 1.0f;
    }
    else {
        ubo(0, 3) = 0.0f;
        ubo(1, 3) = 0.0f;
        ubo(2, 3) = 0.0f;
    }
    ubo(3, 3) = 0.0f;

    pushConstants[1] = ubo;
    // vkCmdPushConstants(commandBuffer, _pipeline_layout,
    // VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvpMatrix), &mvpMatrix);
    vkCmdPushConstants(commandBuffer,
                       _pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(pushConstants),
                       pushConstants.data());
    vkCmdDraw(commandBuffer, index_size, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(commandBuffer, queue);
    VkSubmitInfo submitInfo = _impl::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkFenceCreateInfo fenceInfo = _impl::fenceCreateInfo();
    VkFence fence;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    vkDeviceWaitIdle(_device);

    /*
    Copy framebuffer image to host visible image
    */
    const char* imagedata;
    // Create the linear tiled destination image to copy to and to read the
    // memory from
    VkImageCreateInfo imgCreateInfo(_impl::imageCreateInfo());
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imgCreateInfo.extent.width = width;
    imgCreateInfo.extent.height = height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // Create the image
    VkImage dstImage;
    if (vkCreateImage(_device, &imgCreateInfo, nullptr, &dstImage) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to end create image!");
    }
    // Create memory to back up the image
    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo memAllocInfo(_impl::memoryAllocateInfo());
    VkDeviceMemory dstImageMemory;
    vkGetImageMemoryRequirements(_device, dstImage, &memRequirements);
    memAllocInfo.allocationSize = memRequirements.size;
    memAllocInfo.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & 1) == 1) {
            if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                memAllocInfo.memoryTypeIndex = i;
            }
        }
        memRequirements.memoryTypeBits >>= 1;
    }
    if (memAllocInfo.memoryTypeIndex == -1) {
        memAllocInfo.memoryTypeIndex = 0;
    }

    if (vkAllocateMemory(_device, &memAllocInfo, nullptr, &dstImageMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(_device, dstImage, dstImageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }
    // Do the actual blit from the offscreen image to our host visible
    // destination image
    cmdBufAllocateInfo = _impl::commandBufferAllocateInfo(
        _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    VkCommandBuffer copyCmd;
    if (vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &copyCmd) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    cmdBufInfo = _impl::commandBufferBeginInfo();
    if (vkBeginCommandBuffer(copyCmd, &cmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    VkImageMemoryBarrier imageMemoryBarrier = _impl::imageMemoryBarrier();

    // Transition destination image to transfer destination layout
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    // _color_attachment.image is already in
    // VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, and does not need to be
    // transitioned

    VkImageCopy imageCopyRegion{};
    imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.srcSubresource.layerCount = 1;
    imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.dstSubresource.layerCount = 1;
    imageCopyRegion.extent.width = width;
    imageCopyRegion.extent.height = height;
    imageCopyRegion.extent.depth = 1;
    vkCmdCopyImage(copyCmd,
                   _dst_attachment.image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dstImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageCopyRegion);

    // Transition destination image to general layout, which is the required
    // layout for mapping the image memory later on
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    if (vkEndCommandBuffer(copyCmd) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(copyCmd, queue);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    // Get layout of the image (including row pitch)
    VkImageSubresource subResource{};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    VkSubresourceLayout subResourceLayout;

    vkGetImageSubresourceLayout(
        _device, dstImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    vkMapMemory(
        _device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&imagedata);
    imagedata += subResourceLayout.offset;

    /*
    Save host visible framebuffer image to pixels
    */

    // If source is BGR (destination is always RGB) and we can't use blit (which
    // does automatic conversion), we'll have to manually swizzle color
    // components
    bool colorSwizzle = false;
    // Check if source is BGR and needs swizzle
    std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB,
                                         VK_FORMAT_B8G8R8A8_UNORM,
                                         VK_FORMAT_B8G8R8A8_SNORM };
    colorSwizzle = (std::find(formatsBGR.begin(),
                              formatsBGR.end(),
                              VK_FORMAT_R8G8B8A8_UNORM) != formatsBGR.end());
    uint8_t r, g, b;
    for (int32_t y = 0; y < height; y++) {
        unsigned int* row = (unsigned int*)imagedata;
        for (int32_t x = 0; x < width; x++) {
            if (colorSwizzle) {
                r = *row >> 16;
                g = *row >> 8;
                b = *row;
            }
            else {
                r = *row;
                g = *row >> 8;
                b = *row >> 16;
            }
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
            row++;
        }
        imagedata += subResourceLayout.rowPitch;
    }

    // Clean up resources
    vkUnmapMemory(_device, dstImageMemory);
    vkFreeMemory(_device, dstImageMemory, nullptr);
    vkDestroyImage(_device, dstImage, nullptr);

    vkQueueWaitIdle(_queue);
}

inline void Euclid::Rasterizer::render_depth(std::vector<uint8_t>& pixels,
                                             const RasCamera& camera,
                                             int width,
                                             int height)
{
    std::array<Eigen::Matrix4f, 2> pushConstants;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical_device,
                                        &deviceMemoryProperties);
    /*
    Create framebuffer attachments
    */
    VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat;

    _impl::getSupportedDepthFormat(_physical_device, &depthFormat);

    // Color attachment
    VkImageCreateInfo image = _impl::imageCreateInfo();
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = colorFormat;
    image.extent.width = width;
    image.extent.height = height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    VkMemoryAllocateInfo memAlloc = _impl::memoryAllocateInfo();
    VkMemoryRequirements memReqs;

    if (vkCreateImage(_device, &image, nullptr, &_color_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _color_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_color_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _color_attachment.image, _color_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo colorImageView = _impl::imageViewCreateInfo();
    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageView.format = colorFormat;
    colorImageView.subresourceRange = {};
    colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorImageView.subresourceRange.baseMipLevel = 0;
    colorImageView.subresourceRange.levelCount = 1;
    colorImageView.subresourceRange.baseArrayLayer = 0;
    colorImageView.subresourceRange.layerCount = 1;
    colorImageView.image = _color_attachment.image;
    if (vkCreateImageView(
            _device, &colorImageView, nullptr, &_color_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    // Depth stencil attachment
    image.format = depthFormat;
    image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (vkCreateImage(_device, &image, nullptr, &_depth_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _depth_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_depth_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _depth_attachment.image, _depth_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo depthStencilView = _impl::imageViewCreateInfo();
    depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format = depthFormat;
    depthStencilView.flags = 0;
    depthStencilView.subresourceRange = {};
    depthStencilView.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel = 0;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = _depth_attachment.image;
    if (vkCreateImageView(
            _device, &depthStencilView, nullptr, &_depth_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
    /*
    Create renderpass
    */

    std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
    // Color attachment
    attchmentDescriptions[0].format = colorFormat;
    attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    // Depth attachment
    attchmentDescriptions[1].format = depthFormat;
    attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[1].finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {
        0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    VkAttachmentReference depthReference = {
        1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount =
        static_cast<uint32_t>(attchmentDescriptions.size());
    renderPassInfo.pAttachments = attchmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_render_pass) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create renderpass!");
    }
    VkImageView attachments[2];
    attachments[0] = _color_attachment.view;
    attachments[1] = _depth_attachment.view;

    VkFramebufferCreateInfo framebufferCreateInfo =
        _impl::framebufferCreateInfo();
    framebufferCreateInfo.renderPass = _render_pass;
    framebufferCreateInfo.attachmentCount = 2;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = 1;
    if (vkCreateFramebuffer(
            _device, &framebufferCreateInfo, nullptr, &_framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create frame buffer!");
    }
    /*
    Prepare graphics pipeline
    */

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {};
    VkDescriptorSetLayoutCreateInfo descriptorLayout =
        _impl::descriptorSetLayoutCreateInfo(setLayoutBindings);
    if (vkCreateDescriptorSetLayout(
            _device, &descriptorLayout, nullptr, &_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
        _impl::pipelineLayoutCreateInfo(nullptr, 0);

    // MVP via push constant block
    // VkPushConstantRange pushConstantRange =
    // Euclid::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT, sizeof(glm::mat4),
    // 0); VkPushConstantRange pushConstantRange =
    // Euclid::pushConstantRange(VK_SHADER_STAGE_VERTEX_BIT,
    // sizeof(Eigen::Matrix4f), 0);
    VkPushConstantRange pushConstantRange = _impl::pushConstantRange(
        VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstants), 0);
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(
            _device, &pipelineLayoutCreateInfo, nullptr, &_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (vkCreatePipelineCache(
            _device, &pipelineCacheCreateInfo, nullptr, &_pipeline_cache) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline cache!");
    }

    // Create pipeline
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        _impl::pipelineInputAssemblyStateCreateInfo(
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        _impl::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL,
                                                    VK_CULL_MODE_BACK_BIT,
                                                    VK_FRONT_FACE_CLOCKWISE);

    VkPipelineColorBlendAttachmentState blendAttachmentState =
        _impl::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

    VkPipelineColorBlendStateCreateInfo colorBlendState =
        _impl::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        _impl::pipelineDepthStencilStateCreateInfo(
            VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

    VkPipelineViewportStateCreateInfo viewportState =
        _impl::pipelineViewportStateCreateInfo(1, 1);

    VkPipelineMultisampleStateCreateInfo multisampleState =
        _impl::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState =
        _impl::pipelineDynamicStateCreateInfo(dynamicStateEnables);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo =
        _impl::pipelineCreateInfo(_pipeline_layout, _render_pass);

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    // Vertex bindings an attributes
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    VkPipelineVertexInputStateCreateInfo vertexInputState;

    if (render_with_color_buffer == true) {
        // Binding description
        vertexInputBindings = {
            _impl::vertexInputBindingDescription(
                0, sizeof(Vertex_Color), VK_VERTEX_INPUT_RATE_VERTEX),
        };

        // Attribute descriptions
        vertexInputAttributes = {
            _impl::vertexInputAttributeDescription(
                0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
            _impl::vertexInputAttributeDescription(
                0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Normal
            _impl::vertexInputAttributeDescription(
                0, 2, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 6), // Color
        };

        vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
        vertexInputState.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions =
            vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions =
            vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        shaderStages[0].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pName = "main";
        shaderStages[1].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pName = "main";
        std::string vert_color = std::string(DATA_DIR) + "vert_color.spv";
        std::string frag_color = std::string(DATA_DIR) + "frag_color.spv";
        shaderStages[0].module = _impl::loadShader(vert_color.data(), _device);
        shaderStages[1].module = _impl::loadShader(frag_color.data(), _device);
    }
    else {

        // Binding description
        vertexInputBindings = {
            _impl::vertexInputBindingDescription(
                0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
        };

        // Attribute descriptions
        vertexInputAttributes = {
            _impl::vertexInputAttributeDescription(
                0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
            _impl::vertexInputAttributeDescription(
                0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Normal
        };

        vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
        vertexInputState.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions =
            vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions =
            vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        shaderStages[0].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pName = "main";
        shaderStages[1].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pName = "main";
        std::string vert_depth = std::string(DATA_DIR) + "vert_depth.spv";
        std::string frag_depth = std::string(DATA_DIR) + "frag_depth.spv";
        shaderStages[0].module = _impl::loadShader(vert_depth.data(), _device);
        shaderStages[1].module = _impl::loadShader(frag_depth.data(), _device);
    }

    _shader_modules = { shaderStages[0].module, shaderStages[1].module };
    if (vkCreateGraphicsPipelines(_device,
                                  _pipeline_cache,
                                  1,
                                  &pipelineCreateInfo,
                                  nullptr,
                                  &_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }
    /*
    Command buffer creation
    */

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        _impl::commandBufferAllocateInfo(
            _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    if (vkAllocateCommandBuffers(
            _device, &cmdBufAllocateInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo cmdBufInfo = _impl::commandBufferBeginInfo();

    if (vkBeginCommandBuffer(commandBuffer, &cmdBufInfo)) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    // set the background color
    VkClearValue clearValues[2];
    // clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
    clearValues[0].color = {
        { _background.x(), _background.y(), _background.z(), 1.0f }
    };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderArea.extent.width = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;
    renderPassBeginInfo.renderPass = _render_pass;
    renderPassBeginInfo.framebuffer = _framebuffer;

    vkCmdBeginRenderPass(
        commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.height = (float)height;
    viewport.width = (float)width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Update dynamic scissor state
    VkRect2D scissor = {};
    scissor.extent.width = width;
    scissor.extent.height = height;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

    // Render scene
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_vertex_buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _index_buffer, 0, VK_INDEX_TYPE_UINT32);
    Eigen::Matrix4f mvpMatrix = camera.projection() * camera.view();

    pushConstants[0] = mvpMatrix;

    Eigen::Matrix4f ubo;
    ubo(0, 0) = camera.pos[0];
    ubo(1, 0) = camera.pos[1];
    ubo(2, 0) = camera.pos[2];
    ubo(3, 0) = 0.0f;
    ubo(0, 1) = __material.ambient[0];
    ubo(1, 1) = __material.ambient[1];
    ubo(2, 1) = __material.ambient[2];
    ubo(3, 1) = 0.0f;
    ubo(0, 2) = __material.diffuse[0];
    ubo(1, 2) = __material.diffuse[1];
    ubo(2, 2) = __material.diffuse[2];
    ubo(3, 2) = 0.0f;
    if (_lighting == true) {
        ubo(0, 3) = 1.0f;
        ubo(1, 3) = 1.0f;
        ubo(2, 3) = 1.0f;
    }
    else {
        ubo(0, 3) = 0.0f;
        ubo(1, 3) = 0.0f;
        ubo(2, 3) = 0.0f;
    }
    ubo(3, 3) = 0.0f;

    pushConstants[1] = ubo;
    // vkCmdPushConstants(commandBuffer, _pipeline_layout,
    // VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvpMatrix), &mvpMatrix);
    vkCmdPushConstants(commandBuffer,
                       _pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(pushConstants),
                       pushConstants.data());
    // vkCmdDrawIndexed(commandBuffer, Euclid::index_size, 1, 0, 0, 0);
    vkCmdDraw(commandBuffer, index_size, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(commandBuffer, queue);
    VkSubmitInfo submitInfo = _impl::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkFenceCreateInfo fenceInfo = _impl::fenceCreateInfo();
    VkFence fence;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    vkDeviceWaitIdle(_device);

    /*
    Copy framebuffer image to host visible image
    */
    const char* imagedata;
    // Create the linear tiled destination image to copy to and to read the
    // memory from
    VkImageCreateInfo imgCreateInfo(_impl::imageCreateInfo());
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imgCreateInfo.extent.width = width;
    imgCreateInfo.extent.height = height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // Create the image
    VkImage dstImage;
    if (vkCreateImage(_device, &imgCreateInfo, nullptr, &dstImage) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to end create image!");
    }
    // Create memory to back up the image
    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo memAllocInfo(_impl::memoryAllocateInfo());
    VkDeviceMemory dstImageMemory;
    vkGetImageMemoryRequirements(_device, dstImage, &memRequirements);
    memAllocInfo.allocationSize = memRequirements.size;
    // Memory must be host visible to copy from
    memAllocInfo.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & 1) == 1) {
            if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                memAllocInfo.memoryTypeIndex = i;
            }
        }
        memRequirements.memoryTypeBits >>= 1;
    }
    if (memAllocInfo.memoryTypeIndex == -1) {
        memAllocInfo.memoryTypeIndex = 0;
    }
    if (vkAllocateMemory(_device, &memAllocInfo, nullptr, &dstImageMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(_device, dstImage, dstImageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    // Do the actual blit from the offscreen image to our host visible
    // destination image
    cmdBufAllocateInfo = _impl::commandBufferAllocateInfo(
        _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    VkCommandBuffer copyCmd;
    if (vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &copyCmd) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    cmdBufInfo = _impl::commandBufferBeginInfo();
    if (vkBeginCommandBuffer(copyCmd, &cmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    VkImageMemoryBarrier imageMemoryBarrier = _impl::imageMemoryBarrier();

    // Transition destination image to transfer destination layout
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    // _color_attachment.image is already in
    // VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, and does not need to be
    // transitioned

    VkImageCopy imageCopyRegion{};
    imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.srcSubresource.layerCount = 1;
    imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.dstSubresource.layerCount = 1;
    imageCopyRegion.extent.width = width;
    imageCopyRegion.extent.height = height;
    imageCopyRegion.extent.depth = 1;

    vkCmdCopyImage(copyCmd,
                   _color_attachment.image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dstImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageCopyRegion);

    // Transition destination image to general layout, which is the required
    // layout for mapping the image memory later on
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    if (vkEndCommandBuffer(copyCmd) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(copyCmd, queue);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    // Get layout of the image (including row pitch)
    VkImageSubresource subResource{};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    VkSubresourceLayout subResourceLayout;

    vkGetImageSubresourceLayout(
        _device, dstImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    vkMapMemory(
        _device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&imagedata);
    imagedata += subResourceLayout.offset;

    /*
    Save host visible framebuffer image to pixels
    */

    // If source is BGR (destination is always RGB) and we can't use blit (which
    // does automatic conversion), we'll have to manually swizzle color
    // components
    bool colorSwizzle = false;
    // Check if source is BGR and needs swizzle
    std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB,
                                         VK_FORMAT_B8G8R8A8_UNORM,
                                         VK_FORMAT_B8G8R8A8_SNORM };
    colorSwizzle = (std::find(formatsBGR.begin(),
                              formatsBGR.end(),
                              VK_FORMAT_R8G8B8A8_UNORM) != formatsBGR.end());

    uint8_t r, g, b;
    for (int32_t y = 0; y < height; y++) {
        unsigned int* row = (unsigned int*)imagedata;
        for (int32_t x = 0; x < width; x++) {
            if (colorSwizzle) {
                r = *row >> 16;
                g = *row >> 8;
                b = *row;
            }
            else {
                r = *row;
                g = *row >> 8;
                b = *row >> 16;
            }
            pixels[3 * ((height - y - 1) * width + x) + 0] = r;
            pixels[3 * ((height - y - 1) * width + x) + 1] = g;
            pixels[3 * ((height - y - 1) * width + x) + 2] = b;

            row++;
        }
        imagedata += subResourceLayout.rowPitch;
    }
    // Clean up resources
    vkUnmapMemory(_device, dstImageMemory);
    vkFreeMemory(_device, dstImageMemory, nullptr);
    vkDestroyImage(_device, dstImage, nullptr);

    vkQueueWaitIdle(_queue);
}

inline void Euclid::Rasterizer::render_silhouette(std::vector<uint8_t>& pixels,
                                                  const RasCamera& camera,
                                                  int width,
                                                  int height)
{
    std::array<Eigen::Matrix4f, 2> pushConstants;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical_device,
                                        &deviceMemoryProperties);
    /*
    Create framebuffer attachments
    */
    VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat;

    _impl::getSupportedDepthFormat(_physical_device, &depthFormat);

    // Color attachment
    VkImageCreateInfo image = _impl::imageCreateInfo();
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = colorFormat;
    image.extent.width = width;
    image.extent.height = height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    VkMemoryAllocateInfo memAlloc = _impl::memoryAllocateInfo();
    VkMemoryRequirements memReqs;

    if (vkCreateImage(_device, &image, nullptr, &_color_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _color_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_color_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _color_attachment.image, _color_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo colorImageView = _impl::imageViewCreateInfo();
    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageView.format = colorFormat;
    colorImageView.subresourceRange = {};
    colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorImageView.subresourceRange.baseMipLevel = 0;
    colorImageView.subresourceRange.levelCount = 1;
    colorImageView.subresourceRange.baseArrayLayer = 0;
    colorImageView.subresourceRange.layerCount = 1;
    colorImageView.image = _color_attachment.image;
    if (vkCreateImageView(
            _device, &colorImageView, nullptr, &_color_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
    // Depth stencil attachment
    image.format = depthFormat;
    image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (vkCreateImage(_device, &image, nullptr, &_depth_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _depth_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_depth_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _depth_attachment.image, _depth_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo depthStencilView = _impl::imageViewCreateInfo();
    depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format = depthFormat;
    depthStencilView.flags = 0;
    depthStencilView.subresourceRange = {};
    depthStencilView.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel = 0;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = _depth_attachment.image;
    if (vkCreateImageView(
            _device, &depthStencilView, nullptr, &_depth_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
    /*
    Create renderpass
    */

    std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
    // Color attachment
    attchmentDescriptions[0].format = colorFormat;
    attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    // Depth attachment
    attchmentDescriptions[1].format = depthFormat;
    attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[1].finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {
        0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    VkAttachmentReference depthReference = {
        1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount =
        static_cast<uint32_t>(attchmentDescriptions.size());
    renderPassInfo.pAttachments = attchmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_render_pass) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create renderpass!");
    }
    VkImageView attachments[2];
    attachments[0] = _color_attachment.view;
    attachments[1] = _depth_attachment.view;

    VkFramebufferCreateInfo framebufferCreateInfo =
        _impl::framebufferCreateInfo();
    framebufferCreateInfo.renderPass = _render_pass;
    framebufferCreateInfo.attachmentCount = 2;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = 1;
    if (vkCreateFramebuffer(
            _device, &framebufferCreateInfo, nullptr, &_framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create frame buffer!");
    }
    /*
    Prepare graphics pipeline
    */

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {};
    VkDescriptorSetLayoutCreateInfo descriptorLayout =
        _impl::descriptorSetLayoutCreateInfo(setLayoutBindings);
    if (vkCreateDescriptorSetLayout(
            _device, &descriptorLayout, nullptr, &_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
        _impl::pipelineLayoutCreateInfo(nullptr, 0);

    // MVP via push constant block
    VkPushConstantRange pushConstantRange = _impl::pushConstantRange(
        VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstants), 0);
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    if (vkCreatePipelineLayout(
            _device, &pipelineLayoutCreateInfo, nullptr, &_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (vkCreatePipelineCache(
            _device, &pipelineCacheCreateInfo, nullptr, &_pipeline_cache) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline cache!");
    }

    // Create pipeline
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        _impl::pipelineInputAssemblyStateCreateInfo(
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        _impl::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL,
                                                    VK_CULL_MODE_BACK_BIT,
                                                    VK_FRONT_FACE_CLOCKWISE);

    VkPipelineColorBlendAttachmentState blendAttachmentState =
        _impl::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

    VkPipelineColorBlendStateCreateInfo colorBlendState =
        _impl::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        _impl::pipelineDepthStencilStateCreateInfo(
            VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

    VkPipelineViewportStateCreateInfo viewportState =
        _impl::pipelineViewportStateCreateInfo(1, 1);

    VkPipelineMultisampleStateCreateInfo multisampleState =
        _impl::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState =
        _impl::pipelineDynamicStateCreateInfo(dynamicStateEnables);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo =
        _impl::pipelineCreateInfo(_pipeline_layout, _render_pass);

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    // Vertex bindings an attributes
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    VkPipelineVertexInputStateCreateInfo vertexInputState;

    if (render_with_color_buffer == true) {
        // Binding description
        vertexInputBindings = {
            _impl::vertexInputBindingDescription(
                0, sizeof(Vertex_Color), VK_VERTEX_INPUT_RATE_VERTEX),
        };

        // Attribute descriptions
        vertexInputAttributes = {
            _impl::vertexInputAttributeDescription(
                0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
            _impl::vertexInputAttributeDescription(
                0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Normal
            _impl::vertexInputAttributeDescription(
                0, 2, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 6), // Color
        };

        vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
        vertexInputState.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions =
            vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions =
            vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        shaderStages[0].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pName = "main";
        shaderStages[1].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pName = "main";
        std::string vert_color = std::string(DATA_DIR) + "vert_color.spv";
        std::string frag_color = std::string(DATA_DIR) + "frag_color.spv";
        shaderStages[0].module = _impl::loadShader(vert_color.data(), _device);
        shaderStages[1].module = _impl::loadShader(frag_color.data(), _device);
    }
    else {

        // Binding description
        vertexInputBindings = {
            _impl::vertexInputBindingDescription(
                0, sizeof(Vertex), VK_VERTEX_INPUT_RATE_VERTEX),
        };

        // Attribute descriptions
        vertexInputAttributes = {
            _impl::vertexInputAttributeDescription(
                0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
            _impl::vertexInputAttributeDescription(
                0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Normal
        };

        vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
        vertexInputState.vertexBindingDescriptionCount =
            static_cast<uint32_t>(vertexInputBindings.size());
        vertexInputState.pVertexBindingDescriptions =
            vertexInputBindings.data();
        vertexInputState.vertexAttributeDescriptionCount =
            static_cast<uint32_t>(vertexInputAttributes.size());
        vertexInputState.pVertexAttributeDescriptions =
            vertexInputAttributes.data();

        pipelineCreateInfo.pVertexInputState = &vertexInputState;

        shaderStages[0].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
        shaderStages[0].pName = "main";
        shaderStages[1].sType =
            VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        shaderStages[1].pName = "main";
        std::string vert_silhouette =
            std::string(DATA_DIR) + "vert_silhouette.spv";
        std::string frag_silhouette =
            std::string(DATA_DIR) + "frag_silhouette.spv";
        shaderStages[0].module =
            _impl::loadShader(vert_silhouette.data(), _device);
        shaderStages[1].module =
            _impl::loadShader(frag_silhouette.data(), _device);
    }

    _shader_modules = { shaderStages[0].module, shaderStages[1].module };
    if (vkCreateGraphicsPipelines(_device,
                                  _pipeline_cache,
                                  1,
                                  &pipelineCreateInfo,
                                  nullptr,
                                  &_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    /*
    Command buffer creation
    */

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        _impl::commandBufferAllocateInfo(
            _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    if (vkAllocateCommandBuffers(
            _device, &cmdBufAllocateInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo cmdBufInfo = _impl::commandBufferBeginInfo();
    if (vkBeginCommandBuffer(commandBuffer, &cmdBufInfo)) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    // set the backgound color
    VkClearValue clearValues[2];
    // clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
    clearValues[0].color = {
        { _background.x(), _background.y(), _background.z(), 1.0f }
    };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderArea.extent.width = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;
    renderPassBeginInfo.renderPass = _render_pass;
    renderPassBeginInfo.framebuffer = _framebuffer;

    vkCmdBeginRenderPass(
        commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.height = (float)height;
    viewport.width = (float)width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Update dynamic scissor state
    VkRect2D scissor = {};
    scissor.extent.width = width;
    scissor.extent.height = height;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

    // Render scene
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_vertex_buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _index_buffer, 0, VK_INDEX_TYPE_UINT32);
    Eigen::Matrix4f mvpMatrix = camera.projection() * camera.view();
    pushConstants[0] = mvpMatrix;

    Eigen::Matrix4f ubo;
    ubo(0, 0) = camera.pos[0];
    ubo(1, 0) = camera.pos[1];
    ubo(2, 0) = camera.pos[2];
    ubo(3, 0) = 0.0f;
    ubo(0, 1) = __material.ambient[0];
    ubo(1, 1) = __material.ambient[1];
    ubo(2, 1) = __material.ambient[2];
    ubo(3, 1) = 0.0f;
    ubo(0, 2) = __material.diffuse[0];
    ubo(1, 2) = __material.diffuse[1];
    ubo(2, 2) = __material.diffuse[2];
    ubo(3, 2) = 0.0f;
    if (_lighting == true) {
        ubo(0, 3) = 1.0f;
        ubo(1, 3) = 1.0f;
        ubo(2, 3) = 1.0f;
    }
    else {
        ubo(0, 3) = 0.0f;
        ubo(1, 3) = 0.0f;
        ubo(2, 3) = 0.0f;
    }
    ubo(3, 3) = 0.0f;

    pushConstants[1] = ubo;
    // vkCmdPushConstants(commandBuffer, _pipeline_layout,
    // VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(mvpMatrix), &mvpMatrix);
    vkCmdPushConstants(commandBuffer,
                       _pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(pushConstants),
                       pushConstants.data());
    vkCmdDraw(commandBuffer, index_size, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(commandBuffer, queue);
    VkSubmitInfo submitInfo = _impl::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkFenceCreateInfo fenceInfo = _impl::fenceCreateInfo();
    VkFence fence;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    vkDeviceWaitIdle(_device);

    /*
    Copy framebuffer image to host visible image
    */
    const char* imagedata;
    // Create the linear tiled destination image to copy to and to read the
    // memory from
    VkImageCreateInfo imgCreateInfo(_impl::imageCreateInfo());
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imgCreateInfo.extent.width = width;
    imgCreateInfo.extent.height = height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // Create the image
    VkImage dstImage;
    if (vkCreateImage(_device, &imgCreateInfo, nullptr, &dstImage) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to end create image!");
    }
    // Create memory to back up the image
    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo memAllocInfo(_impl::memoryAllocateInfo());
    VkDeviceMemory dstImageMemory;
    vkGetImageMemoryRequirements(_device, dstImage, &memRequirements);
    memAllocInfo.allocationSize = memRequirements.size;
    // Memory must be host visible to copy from
    memAllocInfo.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & 1) == 1) {
            if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                memAllocInfo.memoryTypeIndex = i;
            }
        }
        memRequirements.memoryTypeBits >>= 1;
    }
    if (memAllocInfo.memoryTypeIndex == -1) {
        memAllocInfo.memoryTypeIndex = 0;
    }
    if (vkAllocateMemory(_device, &memAllocInfo, nullptr, &dstImageMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(_device, dstImage, dstImageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    // Do the actual blit from the offscreen image to our host visible
    // destination image
    cmdBufAllocateInfo = _impl::commandBufferAllocateInfo(
        _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    VkCommandBuffer copyCmd;
    if (vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &copyCmd) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    cmdBufInfo = _impl::commandBufferBeginInfo();
    if (vkBeginCommandBuffer(copyCmd, &cmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    VkImageMemoryBarrier imageMemoryBarrier = _impl::imageMemoryBarrier();

    // Transition destination image to transfer destination layout
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    // _color_attachment.image is already in
    // VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, and does not need to be
    // transitioned

    VkImageCopy imageCopyRegion{};
    imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.srcSubresource.layerCount = 1;
    imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.dstSubresource.layerCount = 1;
    imageCopyRegion.extent.width = width;
    imageCopyRegion.extent.height = height;
    imageCopyRegion.extent.depth = 1;

    vkCmdCopyImage(copyCmd,
                   _color_attachment.image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dstImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageCopyRegion);

    // Transition destination image to general layout, which is the required
    // layout for mapping the image memory later on
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    if (vkEndCommandBuffer(copyCmd) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(copyCmd, queue);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    // Get layout of the image (including row pitch)
    VkImageSubresource subResource{};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    VkSubresourceLayout subResourceLayout;

    vkGetImageSubresourceLayout(
        _device, dstImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    vkMapMemory(
        _device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&imagedata);
    imagedata += subResourceLayout.offset;

    /*
    Save host visible framebuffer image to pixels
    */

    // If source is BGR (destination is always RGB) and we can't use blit (which
    // does automatic conversion), we'll have to manually swizzle color
    // components
    bool colorSwizzle = false;
    // Check if source is BGR and needs swizzle
    std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB,
                                         VK_FORMAT_B8G8R8A8_UNORM,
                                         VK_FORMAT_B8G8R8A8_SNORM };
    colorSwizzle = (std::find(formatsBGR.begin(),
                              formatsBGR.end(),
                              VK_FORMAT_R8G8B8A8_UNORM) != formatsBGR.end());

    uint8_t r, g, b;
    for (int32_t y = 0; y < height; y++) {
        unsigned int* row = (unsigned int*)imagedata;
        for (int32_t x = 0; x < width; x++) {
            if (colorSwizzle) {
                r = *row >> 16;
                g = *row >> 8;
                b = *row;
            }
            else {
                r = *row;
                g = *row >> 8;
                b = *row >> 16;
            }
            pixels[3 * ((height - y - 1) * width + x) + 0] = r;
            pixels[3 * ((height - y - 1) * width + x) + 1] = g;
            pixels[3 * ((height - y - 1) * width + x) + 2] = b;

            row++;
        }
        imagedata += subResourceLayout.rowPitch;
    }
    // Clean up resources
    vkUnmapMemory(_device, dstImageMemory);
    vkFreeMemory(_device, dstImageMemory, nullptr);
    vkDestroyImage(_device, dstImage, nullptr);

    vkQueueWaitIdle(_queue);
}

inline void Euclid::Rasterizer::render_index(std::vector<uint8_t>& pixels,
                                             const RasCamera& camera,
                                             int width,
                                             int height,
                                             bool interleaved)
{
    std::array<Eigen::Matrix4f, 2> pushConstants;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical_device,
                                        &deviceMemoryProperties);
    /*
    Create framebuffer attachments
    */
    VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat;

    _impl::getSupportedDepthFormat(_physical_device, &depthFormat);

    // Color attachment
    VkImageCreateInfo image = _impl::imageCreateInfo();
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = colorFormat;
    image.extent.width = width;
    image.extent.height = height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    VkMemoryAllocateInfo memAlloc = _impl::memoryAllocateInfo();
    VkMemoryRequirements memReqs;

    if (vkCreateImage(_device, &image, nullptr, &_color_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _color_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_color_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _color_attachment.image, _color_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo colorImageView = _impl::imageViewCreateInfo();
    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageView.format = colorFormat;
    colorImageView.subresourceRange = {};
    colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorImageView.subresourceRange.baseMipLevel = 0;
    colorImageView.subresourceRange.levelCount = 1;
    colorImageView.subresourceRange.baseArrayLayer = 0;
    colorImageView.subresourceRange.layerCount = 1;
    colorImageView.image = _color_attachment.image;
    if (vkCreateImageView(
            _device, &colorImageView, nullptr, &_color_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
    // Depth stencil attachment
    image.format = depthFormat;
    image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (vkCreateImage(_device, &image, nullptr, &_depth_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _depth_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_depth_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _depth_attachment.image, _depth_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo depthStencilView = _impl::imageViewCreateInfo();
    depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format = depthFormat;
    depthStencilView.flags = 0;
    depthStencilView.subresourceRange = {};
    depthStencilView.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel = 0;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = _depth_attachment.image;
    if (vkCreateImageView(
            _device, &depthStencilView, nullptr, &_depth_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
    /*
    Create renderpass
    */

    std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
    // Color attachment
    attchmentDescriptions[0].format = colorFormat;
    attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    // Depth attachment
    attchmentDescriptions[1].format = depthFormat;
    attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[1].finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {
        0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    VkAttachmentReference depthReference = {
        1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount =
        static_cast<uint32_t>(attchmentDescriptions.size());
    renderPassInfo.pAttachments = attchmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_render_pass) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create renderpass!");
    }
    VkImageView attachments[2];
    attachments[0] = _color_attachment.view;
    attachments[1] = _depth_attachment.view;

    VkFramebufferCreateInfo framebufferCreateInfo =
        _impl::framebufferCreateInfo();
    framebufferCreateInfo.renderPass = _render_pass;
    framebufferCreateInfo.attachmentCount = 2;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = 1;
    if (vkCreateFramebuffer(
            _device, &framebufferCreateInfo, nullptr, &_framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create frame buffer!");
    }
    /*
    Prepare graphics pipeline
    */

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {};
    VkDescriptorSetLayoutCreateInfo descriptorLayout =
        _impl::descriptorSetLayoutCreateInfo(setLayoutBindings);
    if (vkCreateDescriptorSetLayout(
            _device, &descriptorLayout, nullptr, &_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
        _impl::pipelineLayoutCreateInfo(nullptr, 0);

    // MVP via push constant block
    VkPushConstantRange pushConstantRange = _impl::pushConstantRange(
        VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstants), 0);
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(
            _device, &pipelineLayoutCreateInfo, nullptr, &_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (vkCreatePipelineCache(
            _device, &pipelineCacheCreateInfo, nullptr, &_pipeline_cache) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline cache!");
    }

    // Create pipeline
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        _impl::pipelineInputAssemblyStateCreateInfo(
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        _impl::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL,
                                                    VK_CULL_MODE_BACK_BIT,
                                                    VK_FRONT_FACE_CLOCKWISE);

    VkPipelineColorBlendAttachmentState blendAttachmentState =
        _impl::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

    VkPipelineColorBlendStateCreateInfo colorBlendState =
        _impl::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        _impl::pipelineDepthStencilStateCreateInfo(
            VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

    VkPipelineViewportStateCreateInfo viewportState =
        _impl::pipelineViewportStateCreateInfo(1, 1);

    VkPipelineMultisampleStateCreateInfo multisampleState =
        _impl::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState =
        _impl::pipelineDynamicStateCreateInfo(dynamicStateEnables);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo =
        _impl::pipelineCreateInfo(_pipeline_layout, _render_pass);

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    // Vertex bindings an attributes
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    VkPipelineVertexInputStateCreateInfo vertexInputState;

    // Binding description
    vertexInputBindings = {
        _impl::vertexInputBindingDescription(
            0, sizeof(Vertex_Index), VK_VERTEX_INPUT_RATE_VERTEX),
    };

    // Attribute descriptions
    vertexInputAttributes = {
        _impl::vertexInputAttributeDescription(
            0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
        _impl::vertexInputAttributeDescription(
            0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Color
    };

    vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
    vertexInputState.vertexBindingDescriptionCount =
        static_cast<uint32_t>(vertexInputBindings.size());
    vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
    vertexInputState.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(vertexInputAttributes.size());
    vertexInputState.pVertexAttributeDescriptions =
        vertexInputAttributes.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputState;

    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].pName = "main";
    std::string vert_index = std::string(DATA_DIR) + "vert_index.spv";
    std::string frag_index = std::string(DATA_DIR) + "frag_index.spv";
    shaderStages[0].module = _impl::loadShader(vert_index.data(), _device);
    shaderStages[1].module = _impl::loadShader(frag_index.data(), _device);

    _shader_modules = { shaderStages[0].module, shaderStages[1].module };
    if (vkCreateGraphicsPipelines(_device,
                                  _pipeline_cache,
                                  1,
                                  &pipelineCreateInfo,
                                  nullptr,
                                  &_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    /*
    Command buffer creation
    */

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        _impl::commandBufferAllocateInfo(
            _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    if (vkAllocateCommandBuffers(
            _device, &cmdBufAllocateInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo cmdBufInfo = _impl::commandBufferBeginInfo();

    if (vkBeginCommandBuffer(commandBuffer, &cmdBufInfo)) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    // set the background color
    VkClearValue clearValues[2];
    // clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
    clearValues[0].color = {
        { _background.x(), _background.y(), _background.z(), 1.0f }
    };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderArea.extent.width = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;
    renderPassBeginInfo.renderPass = _render_pass;
    renderPassBeginInfo.framebuffer = _framebuffer;

    vkCmdBeginRenderPass(
        commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.height = (float)height;
    viewport.width = (float)width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Update dynamic scissor state
    VkRect2D scissor = {};
    scissor.extent.width = width;
    scissor.extent.height = height;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

    // Render scene
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_vertex_buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _index_buffer, 0, VK_INDEX_TYPE_UINT32);
    Eigen::Matrix4f mvpMatrix = camera.projection() * camera.view();
    pushConstants[0] = mvpMatrix;

    Eigen::Matrix4f ubo;
    ubo(0, 0) = camera.pos[0];
    ubo(1, 0) = camera.pos[1];
    ubo(2, 0) = camera.pos[2];
    ubo(3, 0) = 0.0f;
    ubo(0, 1) = __material.ambient[0];
    ubo(1, 1) = __material.ambient[1];
    ubo(2, 1) = __material.ambient[2];
    ubo(3, 1) = 0.0f;
    ubo(0, 2) = __material.diffuse[0];
    ubo(1, 2) = __material.diffuse[1];
    ubo(2, 2) = __material.diffuse[2];
    ubo(3, 2) = 0.0f;
    if (_lighting == true) {
        ubo(0, 3) = 1.0f;
        ubo(1, 3) = 1.0f;
        ubo(2, 3) = 1.0f;
    }
    else {
        ubo(0, 3) = 0.0f;
        ubo(1, 3) = 0.0f;
        ubo(2, 3) = 0.0f;
    }
    ubo(3, 3) = 0.0f;

    pushConstants[1] = ubo;
    vkCmdPushConstants(commandBuffer,
                       _pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(pushConstants),
                       pushConstants.data());
    vkCmdDraw(commandBuffer, index_size, 1, 0, 0);

    vkCmdEndRenderPass(commandBuffer);

    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(commandBuffer, queue);
    VkSubmitInfo submitInfo = _impl::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkFenceCreateInfo fenceInfo = _impl::fenceCreateInfo();
    VkFence fence;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    vkDeviceWaitIdle(_device);

    /*
    Copy framebuffer image to host visible image
    */
    const char* imagedata;
    // Create the linear tiled destination image to copy to and to read the
    // memory from
    VkImageCreateInfo imgCreateInfo(_impl::imageCreateInfo());
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imgCreateInfo.extent.width = width;
    imgCreateInfo.extent.height = height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // Create the image
    VkImage dstImage;
    if (vkCreateImage(_device, &imgCreateInfo, nullptr, &dstImage) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to end create image!");
    }
    // Create memory to back up the image
    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo memAllocInfo(_impl::memoryAllocateInfo());
    VkDeviceMemory dstImageMemory;
    vkGetImageMemoryRequirements(_device, dstImage, &memRequirements);
    memAllocInfo.allocationSize = memRequirements.size;
    // Memory must be host visible to copy from
    memAllocInfo.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & 1) == 1) {
            if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                memAllocInfo.memoryTypeIndex = i;
            }
        }
        memRequirements.memoryTypeBits >>= 1;
    }
    if (memAllocInfo.memoryTypeIndex == -1) {
        memAllocInfo.memoryTypeIndex = 0;
    }
    if (vkAllocateMemory(_device, &memAllocInfo, nullptr, &dstImageMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(_device, dstImage, dstImageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    // Do the actual blit from the offscreen image to our host visible
    // destination image
    cmdBufAllocateInfo = _impl::commandBufferAllocateInfo(
        _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    VkCommandBuffer copyCmd;
    if (vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &copyCmd) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    cmdBufInfo = _impl::commandBufferBeginInfo();
    if (vkBeginCommandBuffer(copyCmd, &cmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    VkImageMemoryBarrier imageMemoryBarrier = _impl::imageMemoryBarrier();

    // Transition destination image to transfer destination layout
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    // _color_attachment.image is already in
    // VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, and does not need to be
    // transitioned

    VkImageCopy imageCopyRegion{};
    imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.srcSubresource.layerCount = 1;
    imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.dstSubresource.layerCount = 1;
    imageCopyRegion.extent.width = width;
    imageCopyRegion.extent.height = height;
    imageCopyRegion.extent.depth = 1;

    vkCmdCopyImage(copyCmd,
                   _color_attachment.image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dstImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageCopyRegion);

    // Transition destination image to general layout, which is the required
    // layout for mapping the image memory later on
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    if (vkEndCommandBuffer(copyCmd) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(copyCmd, queue);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    // Get layout of the image (including row pitch)
    VkImageSubresource subResource{};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    VkSubresourceLayout subResourceLayout;

    vkGetImageSubresourceLayout(
        _device, dstImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    vkMapMemory(
        _device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&imagedata);
    imagedata += subResourceLayout.offset;

    /*
    Save host visible framebuffer image to pixels
    */

    // If source is BGR (destination is always RGB) and we can't use blit (which
    // does automatic conversion), we'll have to manually swizzle color
    // components
    bool colorSwizzle = false;
    // Check if source is BGR and needs swizzle
    std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB,
                                         VK_FORMAT_B8G8R8A8_UNORM,
                                         VK_FORMAT_B8G8R8A8_SNORM };
    colorSwizzle = (std::find(formatsBGR.begin(),
                              formatsBGR.end(),
                              VK_FORMAT_R8G8B8A8_UNORM) != formatsBGR.end());
    uint8_t r, g, b;
    for (int32_t y = 0; y < height; y++) {
        unsigned int* row = (unsigned int*)imagedata;
        for (int32_t x = 0; x < width; x++) {
            if (colorSwizzle) {
                r = *row >> 16;
                g = *row >> 8;
                b = *row;
            }
            else {
                r = *row;
                g = *row >> 8;
                b = *row >> 16;
            }
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
            row++;
        }
        imagedata += subResourceLayout.rowPitch;
    }
    // Clean up resources
    vkUnmapMemory(_device, dstImageMemory);
    vkFreeMemory(_device, dstImageMemory, nullptr);
    vkDestroyImage(_device, dstImage, nullptr);

    vkQueueWaitIdle(_queue);
}

inline void Euclid::Rasterizer::render_index(std::vector<uint32_t>& indices,
                                             const RasCamera& camera,
                                             int width,
                                             int height)
{
    std::array<Eigen::Matrix4f, 2> pushConstants;
    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    vkGetPhysicalDeviceMemoryProperties(_physical_device,
                                        &deviceMemoryProperties);
    /*
    Create framebuffer attachments
    */
    VkFormat colorFormat = VK_FORMAT_R8G8B8A8_UNORM;
    VkFormat depthFormat;

    _impl::getSupportedDepthFormat(_physical_device, &depthFormat);

    // Color attachment
    VkImageCreateInfo image = _impl::imageCreateInfo();
    image.imageType = VK_IMAGE_TYPE_2D;
    image.format = colorFormat;
    image.extent.width = width;
    image.extent.height = height;
    image.extent.depth = 1;
    image.mipLevels = 1;
    image.arrayLayers = 1;
    image.samples = VK_SAMPLE_COUNT_1_BIT;
    image.tiling = VK_IMAGE_TILING_OPTIMAL;
    image.usage =
        VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;

    VkMemoryAllocateInfo memAlloc = _impl::memoryAllocateInfo();
    VkMemoryRequirements memReqs;

    if (vkCreateImage(_device, &image, nullptr, &_color_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _color_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_color_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _color_attachment.image, _color_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo colorImageView = _impl::imageViewCreateInfo();
    colorImageView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    colorImageView.format = colorFormat;
    colorImageView.subresourceRange = {};
    colorImageView.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    colorImageView.subresourceRange.baseMipLevel = 0;
    colorImageView.subresourceRange.levelCount = 1;
    colorImageView.subresourceRange.baseArrayLayer = 0;
    colorImageView.subresourceRange.layerCount = 1;
    colorImageView.image = _color_attachment.image;
    if (vkCreateImageView(
            _device, &colorImageView, nullptr, &_color_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }

    // Depth stencil attachment
    image.format = depthFormat;
    image.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;

    if (vkCreateImage(_device, &image, nullptr, &_depth_attachment.image) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image!");
    }
    vkGetImageMemoryRequirements(_device, _depth_attachment.image, &memReqs);
    memAlloc.allocationSize = memReqs.size;
    memAlloc.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memReqs.memoryTypeBits & 1) == 1) {
            if ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                 VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) ==
                VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) {
                memAlloc.memoryTypeIndex = i;
            }
        }
        memReqs.memoryTypeBits >>= 1;
    }
    if (memAlloc.memoryTypeIndex == -1) { memAlloc.memoryTypeIndex = 0; }
    if (vkAllocateMemory(
            _device, &memAlloc, nullptr, &_depth_attachment.memory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(
            _device, _depth_attachment.image, _depth_attachment.memory, 0) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    VkImageViewCreateInfo depthStencilView = _impl::imageViewCreateInfo();
    depthStencilView.viewType = VK_IMAGE_VIEW_TYPE_2D;
    depthStencilView.format = depthFormat;
    depthStencilView.flags = 0;
    depthStencilView.subresourceRange = {};
    depthStencilView.subresourceRange.aspectMask =
        VK_IMAGE_ASPECT_DEPTH_BIT | VK_IMAGE_ASPECT_STENCIL_BIT;
    depthStencilView.subresourceRange.baseMipLevel = 0;
    depthStencilView.subresourceRange.levelCount = 1;
    depthStencilView.subresourceRange.baseArrayLayer = 0;
    depthStencilView.subresourceRange.layerCount = 1;
    depthStencilView.image = _depth_attachment.image;
    if (vkCreateImageView(
            _device, &depthStencilView, nullptr, &_depth_attachment.view) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create image view!");
    }
    /*
    Create renderpass
    */

    std::array<VkAttachmentDescription, 2> attchmentDescriptions = {};
    // Color attachment
    attchmentDescriptions[0].format = colorFormat;
    attchmentDescriptions[0].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    attchmentDescriptions[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[0].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[0].finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
    // Depth attachment
    attchmentDescriptions[1].format = depthFormat;
    attchmentDescriptions[1].samples = VK_SAMPLE_COUNT_1_BIT;
    attchmentDescriptions[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
    attchmentDescriptions[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    attchmentDescriptions[1].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    attchmentDescriptions[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    attchmentDescriptions[1].finalLayout =
        VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

    VkAttachmentReference colorReference = {
        0, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    };
    VkAttachmentReference depthReference = {
        1, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL
    };

    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorReference;
    subpassDescription.pDepthStencilAttachment = &depthReference;

    // Use subpass dependencies for layout transitions
    std::array<VkSubpassDependency, 2> dependencies;

    dependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[0].dstSubpass = 0;
    dependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[0].dstStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[0].srcAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    dependencies[1].srcSubpass = 0;
    dependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    dependencies[1].srcStageMask =
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    dependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    dependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                                    VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    dependencies[1].dstAccessMask = VK_ACCESS_MEMORY_READ_BIT;
    dependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Create the actual renderpass
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.attachmentCount =
        static_cast<uint32_t>(attchmentDescriptions.size());
    renderPassInfo.pAttachments = attchmentDescriptions.data();
    renderPassInfo.subpassCount = 1;
    renderPassInfo.pSubpasses = &subpassDescription;
    renderPassInfo.dependencyCount = static_cast<uint32_t>(dependencies.size());
    renderPassInfo.pDependencies = dependencies.data();
    if (vkCreateRenderPass(_device, &renderPassInfo, nullptr, &_render_pass) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create renderpass!");
    }
    VkImageView attachments[2];
    attachments[0] = _color_attachment.view;
    attachments[1] = _depth_attachment.view;

    VkFramebufferCreateInfo framebufferCreateInfo =
        _impl::framebufferCreateInfo();
    framebufferCreateInfo.renderPass = _render_pass;
    framebufferCreateInfo.attachmentCount = 2;
    framebufferCreateInfo.pAttachments = attachments;
    framebufferCreateInfo.width = width;
    framebufferCreateInfo.height = height;
    framebufferCreateInfo.layers = 1;
    if (vkCreateFramebuffer(
            _device, &framebufferCreateInfo, nullptr, &_framebuffer) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create frame buffer!");
    }

    /*
    Prepare graphics pipeline
    */

    std::vector<VkDescriptorSetLayoutBinding> setLayoutBindings = {};
    VkDescriptorSetLayoutCreateInfo descriptorLayout =
        _impl::descriptorSetLayoutCreateInfo(setLayoutBindings);
    if (vkCreateDescriptorSetLayout(
            _device, &descriptorLayout, nullptr, &_descriptor_set_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create descriptor set layout!");
    }
    VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo =
        _impl::pipelineLayoutCreateInfo(nullptr, 0);

    // MVP via push constant block
    VkPushConstantRange pushConstantRange = _impl::pushConstantRange(
        VK_SHADER_STAGE_VERTEX_BIT, sizeof(pushConstants), 0);
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;

    if (vkCreatePipelineLayout(
            _device, &pipelineLayoutCreateInfo, nullptr, &_pipeline_layout) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline layout!");
    }

    VkPipelineCacheCreateInfo pipelineCacheCreateInfo = {};
    pipelineCacheCreateInfo.sType =
        VK_STRUCTURE_TYPE_PIPELINE_CACHE_CREATE_INFO;
    if (vkCreatePipelineCache(
            _device, &pipelineCacheCreateInfo, nullptr, &_pipeline_cache) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to create pipeline cache!");
    }

    // Create pipeline
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyState =
        _impl::pipelineInputAssemblyStateCreateInfo(
            VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST, 0, VK_FALSE);

    VkPipelineRasterizationStateCreateInfo rasterizationState =
        _impl::pipelineRasterizationStateCreateInfo(VK_POLYGON_MODE_FILL,
                                                    VK_CULL_MODE_BACK_BIT,
                                                    VK_FRONT_FACE_CLOCKWISE);

    VkPipelineColorBlendAttachmentState blendAttachmentState =
        _impl::pipelineColorBlendAttachmentState(0xf, VK_FALSE);

    VkPipelineColorBlendStateCreateInfo colorBlendState =
        _impl::pipelineColorBlendStateCreateInfo(1, &blendAttachmentState);

    VkPipelineDepthStencilStateCreateInfo depthStencilState =
        _impl::pipelineDepthStencilStateCreateInfo(
            VK_TRUE, VK_TRUE, VK_COMPARE_OP_LESS_OR_EQUAL);

    VkPipelineViewportStateCreateInfo viewportState =
        _impl::pipelineViewportStateCreateInfo(1, 1);

    VkPipelineMultisampleStateCreateInfo multisampleState =
        _impl::pipelineMultisampleStateCreateInfo(VK_SAMPLE_COUNT_1_BIT);

    std::vector<VkDynamicState> dynamicStateEnables = {
        VK_DYNAMIC_STATE_VIEWPORT, VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState =
        _impl::pipelineDynamicStateCreateInfo(dynamicStateEnables);

    VkGraphicsPipelineCreateInfo pipelineCreateInfo =
        _impl::pipelineCreateInfo(_pipeline_layout, _render_pass);

    std::array<VkPipelineShaderStageCreateInfo, 2> shaderStages{};

    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyState;
    pipelineCreateInfo.pRasterizationState = &rasterizationState;
    pipelineCreateInfo.pColorBlendState = &colorBlendState;
    pipelineCreateInfo.pMultisampleState = &multisampleState;
    pipelineCreateInfo.pViewportState = &viewportState;
    pipelineCreateInfo.pDepthStencilState = &depthStencilState;
    pipelineCreateInfo.pDynamicState = &dynamicState;
    pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStages.size());
    pipelineCreateInfo.pStages = shaderStages.data();

    // Vertex bindings an attributes
    std::vector<VkVertexInputBindingDescription> vertexInputBindings;
    std::vector<VkVertexInputAttributeDescription> vertexInputAttributes;
    VkPipelineVertexInputStateCreateInfo vertexInputState;

    // Binding description
    vertexInputBindings = {
        _impl::vertexInputBindingDescription(
            0, sizeof(Vertex_Index), VK_VERTEX_INPUT_RATE_VERTEX),
    };

    // Attribute descriptions
    vertexInputAttributes = {
        _impl::vertexInputAttributeDescription(
            0, 0, VK_FORMAT_R32G32B32_SFLOAT, 0), // Position
        _impl::vertexInputAttributeDescription(
            0, 1, VK_FORMAT_R32G32B32_SFLOAT, sizeof(float) * 3), // Color
    };

    vertexInputState = _impl::pipelineVertexInputStateCreateInfo();
    vertexInputState.vertexBindingDescriptionCount =
        static_cast<uint32_t>(vertexInputBindings.size());
    vertexInputState.pVertexBindingDescriptions = vertexInputBindings.data();
    vertexInputState.vertexAttributeDescriptionCount =
        static_cast<uint32_t>(vertexInputAttributes.size());
    vertexInputState.pVertexAttributeDescriptions =
        vertexInputAttributes.data();

    pipelineCreateInfo.pVertexInputState = &vertexInputState;

    shaderStages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
    shaderStages[0].pName = "main";
    shaderStages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    shaderStages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    shaderStages[1].pName = "main";
    std::string vert_index = std::string(DATA_DIR) + "vert_index.spv";
    std::string frag_index = std::string(DATA_DIR) + "frag_index.spv";
    shaderStages[0].module = _impl::loadShader(vert_index.data(), _device);
    shaderStages[1].module = _impl::loadShader(frag_index.data(), _device);

    _shader_modules = { shaderStages[0].module, shaderStages[1].module };
    if (vkCreateGraphicsPipelines(_device,
                                  _pipeline_cache,
                                  1,
                                  &pipelineCreateInfo,
                                  nullptr,
                                  &_pipeline) != VK_SUCCESS) {
        throw std::runtime_error("failed to create graphics pipeline!");
    }

    /*
    Command buffer creation
    */

    VkCommandBuffer commandBuffer;
    VkCommandBufferAllocateInfo cmdBufAllocateInfo =
        _impl::commandBufferAllocateInfo(
            _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    if (vkAllocateCommandBuffers(
            _device, &cmdBufAllocateInfo, &commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }

    VkCommandBufferBeginInfo cmdBufInfo = _impl::commandBufferBeginInfo();

    if (vkBeginCommandBuffer(commandBuffer, &cmdBufInfo)) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    // set the backgroud color
    VkClearValue clearValues[2];
    // clearValues[0].color = { { 0.0f, 0.0f, 0.2f, 1.0f } };
    clearValues[0].color = {
        { _background.x(), _background.y(), _background.z(), 1.0f }
    };
    clearValues[1].depthStencil = { 1.0f, 0 };

    VkRenderPassBeginInfo renderPassBeginInfo = {};
    renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassBeginInfo.renderArea.extent.width = width;
    renderPassBeginInfo.renderArea.extent.height = height;
    renderPassBeginInfo.clearValueCount = 2;
    renderPassBeginInfo.pClearValues = clearValues;
    renderPassBeginInfo.renderPass = _render_pass;
    renderPassBeginInfo.framebuffer = _framebuffer;

    vkCmdBeginRenderPass(
        commandBuffer, &renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);

    VkViewport viewport = {};
    viewport.height = (float)height;
    viewport.width = (float)width;
    viewport.minDepth = (float)0.0f;
    viewport.maxDepth = (float)1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    // Update dynamic scissor state
    VkRect2D scissor = {};
    scissor.extent.width = width;
    scissor.extent.height = height;
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    vkCmdBindPipeline(
        commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);

    // Render scene
    VkDeviceSize offsets[1] = { 0 };
    vkCmdBindVertexBuffers(commandBuffer, 0, 1, &_vertex_buffer, offsets);
    vkCmdBindIndexBuffer(commandBuffer, _index_buffer, 0, VK_INDEX_TYPE_UINT32);
    Eigen::Matrix4f mvpMatrix = camera.projection() * camera.view();
    pushConstants[0] = mvpMatrix;

    Eigen::Matrix4f ubo;
    ubo(0, 0) = camera.pos[0];
    ubo(1, 0) = camera.pos[1];
    ubo(2, 0) = camera.pos[2];
    ubo(3, 0) = 0.0f;
    ubo(0, 1) = __material.ambient[0];
    ubo(1, 1) = __material.ambient[1];
    ubo(2, 1) = __material.ambient[2];
    ubo(3, 1) = 0.0f;
    ubo(0, 2) = __material.diffuse[0];
    ubo(1, 2) = __material.diffuse[1];
    ubo(2, 2) = __material.diffuse[2];
    ubo(3, 2) = 0.0f;
    if (_lighting == true) {
        ubo(0, 3) = 1.0f;
        ubo(1, 3) = 1.0f;
        ubo(2, 3) = 1.0f;
    }
    else {
        ubo(0, 3) = 0.0f;
        ubo(1, 3) = 0.0f;
        ubo(2, 3) = 0.0f;
    }
    ubo(3, 3) = 0.0f;

    pushConstants[1] = ubo;
    vkCmdPushConstants(commandBuffer,
                       _pipeline_layout,
                       VK_SHADER_STAGE_VERTEX_BIT,
                       0,
                       sizeof(pushConstants),
                       pushConstants.data());
    vkCmdDraw(commandBuffer, index_size, 1, 0, 0);
    vkCmdEndRenderPass(commandBuffer);
    if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(commandBuffer, queue);
    VkSubmitInfo submitInfo = _impl::submitInfo();
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    VkFenceCreateInfo fenceInfo = _impl::fenceCreateInfo();
    VkFence fence;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    vkDeviceWaitIdle(_device);

    /*
    Copy framebuffer image to host visible image
    */
    const char* imagedata;
    // Create the linear tiled destination image to copy to and to read the
    // memory from
    VkImageCreateInfo imgCreateInfo(_impl::imageCreateInfo());
    imgCreateInfo.imageType = VK_IMAGE_TYPE_2D;
    imgCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imgCreateInfo.extent.width = width;
    imgCreateInfo.extent.height = height;
    imgCreateInfo.extent.depth = 1;
    imgCreateInfo.arrayLayers = 1;
    imgCreateInfo.mipLevels = 1;
    imgCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imgCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imgCreateInfo.tiling = VK_IMAGE_TILING_LINEAR;
    imgCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    // Create the image
    VkImage dstImage;
    if (vkCreateImage(_device, &imgCreateInfo, nullptr, &dstImage) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to end create image!");
    }
    // Create memory to back up the image
    VkMemoryRequirements memRequirements;
    VkMemoryAllocateInfo memAllocInfo(_impl::memoryAllocateInfo());
    VkDeviceMemory dstImageMemory;
    vkGetImageMemoryRequirements(_device, dstImage, &memRequirements);
    memAllocInfo.allocationSize = memRequirements.size;
    // Memory must be host visible to copy from
    memAllocInfo.memoryTypeIndex = -1;
    for (uint32_t i = 0; i < deviceMemoryProperties.memoryTypeCount; i++) {
        if ((memRequirements.memoryTypeBits & 1) == 1) {
            if (((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) ||
                ((deviceMemoryProperties.memoryTypes[i].propertyFlags &
                  VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) ==
                 VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) {
                memAllocInfo.memoryTypeIndex = i;
            }
        }
        memRequirements.memoryTypeBits >>= 1;
    }
    if (memAllocInfo.memoryTypeIndex == -1) {
        memAllocInfo.memoryTypeIndex = 0;
    }
    if (vkAllocateMemory(_device, &memAllocInfo, nullptr, &dstImageMemory) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate memory!");
    }
    if (vkBindImageMemory(_device, dstImage, dstImageMemory, 0) != VK_SUCCESS) {
        throw std::runtime_error("failed to bind image memory!");
    }

    // Do the actual blit from the offscreen image to our host visible
    // destination image
    cmdBufAllocateInfo = _impl::commandBufferAllocateInfo(
        _command_pool, VK_COMMAND_BUFFER_LEVEL_PRIMARY, 1);
    VkCommandBuffer copyCmd;
    if (vkAllocateCommandBuffers(_device, &cmdBufAllocateInfo, &copyCmd) !=
        VK_SUCCESS) {
        throw std::runtime_error("failed to allocate command buffers!");
    }
    cmdBufInfo = _impl::commandBufferBeginInfo();
    if (vkBeginCommandBuffer(copyCmd, &cmdBufInfo) != VK_SUCCESS) {
        throw std::runtime_error("failed to begin command buffer!");
    }

    VkImageMemoryBarrier imageMemoryBarrier = _impl::imageMemoryBarrier();

    // Transition destination image to transfer destination layout
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        0,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_IMAGE_LAYOUT_UNDEFINED,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    // _color_attachment.image is already in
    // VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL, and does not need to be
    // transitioned

    VkImageCopy imageCopyRegion{};
    imageCopyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.srcSubresource.layerCount = 1;
    imageCopyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageCopyRegion.dstSubresource.layerCount = 1;
    imageCopyRegion.extent.width = width;
    imageCopyRegion.extent.height = height;
    imageCopyRegion.extent.depth = 1;

    vkCmdCopyImage(copyCmd,
                   _color_attachment.image,
                   VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                   dstImage,
                   VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                   1,
                   &imageCopyRegion);

    // Transition destination image to general layout, which is the required
    // layout for mapping the image memory later on
    _impl::insertImageMemoryBarrier(
        copyCmd,
        dstImage,
        VK_ACCESS_TRANSFER_WRITE_BIT,
        VK_ACCESS_MEMORY_READ_BIT,
        VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
        VK_IMAGE_LAYOUT_GENERAL,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VK_PIPELINE_STAGE_TRANSFER_BIT,
        VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

    if (vkEndCommandBuffer(copyCmd) != VK_SUCCESS) {
        throw std::runtime_error("failed to end command buffer!");
    }

    // submitWork(copyCmd, queue);
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &copyCmd;
    if (vkCreateFence(_device, &fenceInfo, nullptr, &fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to create fence!");
    }
    if (vkQueueSubmit(_queue, 1, &submitInfo, fence) != VK_SUCCESS) {
        throw std::runtime_error("failed to submit queue!");
    }
    if (vkWaitForFences(_device, 1, &fence, VK_TRUE, UINT64_MAX) !=
        VK_SUCCESS) {
        throw std::runtime_error("vkWaitForFences failed!");
    }
    vkDestroyFence(_device, fence, nullptr);

    // Get layout of the image (including row pitch)
    VkImageSubresource subResource{};
    subResource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    VkSubresourceLayout subResourceLayout;

    vkGetImageSubresourceLayout(
        _device, dstImage, &subResource, &subResourceLayout);

    // Map image memory so we can start copying from it
    vkMapMemory(
        _device, dstImageMemory, 0, VK_WHOLE_SIZE, 0, (void**)&imagedata);
    imagedata += subResourceLayout.offset;

    /*
    Save host visible framebuffer image to pixels
    */

    // If source is BGR (destination is always RGB) and we can't use blit (which
    // does automatic conversion), we'll have to manually swizzle color
    // components
    bool colorSwizzle = false;
    // Check if source is BGR and needs swizzle
    std::vector<VkFormat> formatsBGR = { VK_FORMAT_B8G8R8A8_SRGB,
                                         VK_FORMAT_B8G8R8A8_UNORM,
                                         VK_FORMAT_B8G8R8A8_SNORM };
    colorSwizzle = (std::find(formatsBGR.begin(),
                              formatsBGR.end(),
                              VK_FORMAT_R8G8B8A8_UNORM) != formatsBGR.end());
    uint8_t r, g, b;
    for (int32_t y = 0; y < height; y++) {
        unsigned int* row = (unsigned int*)imagedata;
        for (int32_t x = 0; x < width; x++) {
            if (colorSwizzle) {
                r = *row >> 16;
                g = *row >> 8;
                b = *row;
            }
            else {
                r = *row;
                g = *row >> 8;
                b = *row >> 16;
            }
            uint32_t index;
            index = (uint32_t)*row;
            indices[(height - y - 1) * width + x] = index;
            row++;
        }
        imagedata += subResourceLayout.rowPitch;
    }

    // Clean up resources
    vkUnmapMemory(_device, dstImageMemory);
    vkFreeMemory(_device, dstImageMemory, nullptr);
    vkDestroyImage(_device, dstImage, nullptr);

    vkQueueWaitIdle(_queue);
}

inline void Euclid::Rasterizer::enable_index()
{
    render_with_index = true;
}

inline void Euclid::Rasterizer::disable_index()
{
    render_with_index = false;
}

} // namespace Euclid
