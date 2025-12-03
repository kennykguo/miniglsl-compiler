// vulkan_viewer.cpp
// minimal vulkan renderer for spir-v fragment shaders
// renders 3d mesh with your compiled spir-v shader to png

#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <fstream>
#include <iostream>
#include <cstring>
#include <cmath>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

// simple 3d vector math
struct Vec3 { float x, y, z; };
struct Vec4 { float x, y, z, w; };
struct Mat4 { float m[16]; };

Vec3 vec3(float x, float y, float z) { return {x, y, z}; }
float dot(Vec3 a, Vec3 b) { return a.x*b.x + a.y*b.y + a.z*b.z; }
Vec3 normalize(Vec3 v) {
    float len = std::sqrt(dot(v, v));
    return len > 1e-8f ? vec3(v.x/len, v.y/len, v.z/len) : vec3(0,0,0);
}

// load spir-v binary
std::vector<uint32_t> load_spirv(const char* path) {
    std::ifstream file(path, std::ios::binary | std::ios::ate);
    if (!file) {
        std::cerr << "failed to open " << path << std::endl;
        exit(1);
    }
    size_t size = file.tellg();
    file.seekg(0);
    
    std::vector<uint32_t> buffer(size / 4);
    file.read((char*)buffer.data(), size);
    return buffer;
}

// simple obj loader
struct Vertex {
    Vec3 pos;
    Vec3 normal;
    Vec4 color;
};

std::vector<Vertex> load_obj(const char* path) {
    std::ifstream file(path);
    if (!file) {
        std::cerr << "failed to open " << path << std::endl;
        exit(1);
    }
    
    std::vector<Vec3> positions;
    std::vector<Vertex> vertices;
    
    std::string line;
    while (std::getline(file, line)) {
        if (line.substr(0, 2) == "v ") {
            Vec3 v;
            sscanf(line.c_str(), "v %f %f %f", &v.x, &v.y, &v.z);
            positions.push_back(v);
        } else if (line.substr(0, 2) == "f ") {
            int a, b, c;
            if (sscanf(line.c_str(), "f %d %d %d", &a, &b, &c) == 3) {
                // simple flat shading - compute face normal
                Vec3 p0 = positions[a-1];
                Vec3 p1 = positions[b-1];
                Vec3 p2 = positions[c-1];
                
                Vec3 e1 = {p1.x-p0.x, p1.y-p0.y, p1.z-p0.z};
                Vec3 e2 = {p2.x-p0.x, p2.y-p0.y, p2.z-p0.z};
                Vec3 n = normalize({
                    e1.y*e2.z - e1.z*e2.y,
                    e1.z*e2.x - e1.x*e2.z,
                    e1.x*e2.y - e1.y*e2.x
                });
                
                vertices.push_back({p0, n, {0.9f, 0.8f, 0.3f, 1.0f}});
                vertices.push_back({p1, n, {0.9f, 0.8f, 0.3f, 1.0f}});
                vertices.push_back({p2, n, {0.9f, 0.8f, 0.3f, 1.0f}});
            }
        }
    }
    
    std::cout << "loaded " << vertices.size() << " vertices" << std::endl;
    return vertices;
}

// vulkan helpers - check result
#define VK_CHECK(x) do { \
    VkResult res = (x); \
    if (res != VK_SUCCESS) { \
        std::cerr << "vulkan error at " << __FILE__ << ":" << __LINE__ << " = " << res << std::endl; \
        exit(1); \
    } \
} while(0)

// main vulkan renderer
class VulkanRenderer {
public:
    VkInstance instance;
    VkPhysicalDevice physicalDevice;
    VkDevice device;
    VkQueue queue;
    uint32_t queueFamily;
    
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    
    VkImage colorImage, depthImage;
    VkDeviceMemory colorMemory, depthMemory;
    VkImageView colorView, depthView;
    VkFramebuffer framebuffer;
    
    VkBuffer vertexBuffer;
    VkDeviceMemory vertexMemory;
    
    VkCommandPool commandPool;
    VkCommandBuffer commandBuffer;
    
    uint32_t width, height;
    std::vector<Vertex> vertices;
    
    VulkanRenderer(uint32_t w, uint32_t h) : width(w), height(h) {}
    
    void init() {
        create_instance();
        select_physical_device();
        create_device();
        create_render_pass();
        create_framebuffer();
        create_command_pool();
    }
    
    void create_instance() {
        VkApplicationInfo appInfo = {};
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "spirv viewer";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "no engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;
        
        VkInstanceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        
        VK_CHECK(vkCreateInstance(&createInfo, nullptr, &instance));
        std::cout << "✓ vulkan instance created" << std::endl;
    }
    
    void select_physical_device() {
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        
        if (deviceCount == 0) {
            std::cerr << "no vulkan devices found" << std::endl;
            exit(1);
        }
        
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());
        
        physicalDevice = devices[0]; // use first device
        
        VkPhysicalDeviceProperties props;
        vkGetPhysicalDeviceProperties(physicalDevice, &props);
        std::cout << "✓ using device: " << props.deviceName << std::endl;
    }
    
    void create_device() {
        // find queue family with graphics support
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);
        
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilies.data());
        
        queueFamily = 0;
        for (uint32_t i = 0; i < queueFamilyCount; i++) {
            if (queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
                queueFamily = i;
                break;
            }
        }
        
        float queuePriority = 1.0f;
        VkDeviceQueueCreateInfo queueCreateInfo = {};
        queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
        queueCreateInfo.queueFamilyIndex = queueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &queuePriority;
        
        VkPhysicalDeviceFeatures deviceFeatures = {};
        
        VkDeviceCreateInfo createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
        createInfo.queueCreateInfoCount = 1;
        createInfo.pQueueCreateInfos = &queueCreateInfo;
        createInfo.pEnabledFeatures = &deviceFeatures;
        
        VK_CHECK(vkCreateDevice(physicalDevice, &createInfo, nullptr, &device));
        vkGetDeviceQueue(device, queueFamily, 0, &queue);
        
        std::cout << "✓ logical device created" << std::endl;
    }
    
    void create_render_pass() {
        // color attachment
        VkAttachmentDescription colorAttachment = {};
        colorAttachment.format = VK_FORMAT_R8G8B8A8_UNORM;
        colorAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        colorAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
        colorAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        colorAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        colorAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        colorAttachment.finalLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
        
        // depth attachment
        VkAttachmentDescription depthAttachment = {};
        depthAttachment.format = VK_FORMAT_D32_SFLOAT;
        depthAttachment.samples = VK_SAMPLE_COUNT_1_BIT;
        depthAttachment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
        depthAttachment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
        depthAttachment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
        depthAttachment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        depthAttachment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        VkAttachmentReference colorRef = {};
        colorRef.attachment = 0;
        colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
        
        VkAttachmentReference depthRef = {};
        depthRef.attachment = 1;
        depthRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        
        VkSubpassDescription subpass = {};
        subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
        subpass.colorAttachmentCount = 1;
        subpass.pColorAttachments = &colorRef;
        subpass.pDepthStencilAttachment = &depthRef;
        
        VkAttachmentDescription attachments[] = {colorAttachment, depthAttachment};
        
        VkRenderPassCreateInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
        renderPassInfo.attachmentCount = 2;
        renderPassInfo.pAttachments = attachments;
        renderPassInfo.subpassCount = 1;
        renderPassInfo.pSubpasses = &subpass;
        
        VK_CHECK(vkCreateRenderPass(device, &renderPassInfo, nullptr, &renderPass));
        std::cout << "✓ render pass created" << std::endl;
    }
    
    uint32_t find_memory_type(uint32_t typeFilter, VkMemoryPropertyFlags properties) {
        VkPhysicalDeviceMemoryProperties memProperties;
        vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);
        
        for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
            if ((typeFilter & (1 << i)) && 
                (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
                return i;
            }
        }
        
        std::cerr << "failed to find suitable memory type" << std::endl;
        exit(1);
    }
    
    void create_image(VkImage& image, VkDeviceMemory& memory, 
                     VkFormat format, VkImageUsageFlags usage) {
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = width;
        imageInfo.extent.height = height;
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = 1;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.usage = usage;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        VK_CHECK(vkCreateImage(device, &imageInfo, nullptr, &image));
        
        VkMemoryRequirements memRequirements;
        vkGetImageMemoryRequirements(device, image, &memRequirements);
        
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits, 
                                                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
        
        VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &memory));
        vkBindImageMemory(device, image, memory, 0);
    }
    
    void create_framebuffer() {
        // color image
        create_image(colorImage, colorMemory, VK_FORMAT_R8G8B8A8_UNORM,
                    VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT);
        
        VkImageViewCreateInfo colorViewInfo = {};
        colorViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        colorViewInfo.image = colorImage;
        colorViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
        colorViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
        colorViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        colorViewInfo.subresourceRange.baseMipLevel = 0;
        colorViewInfo.subresourceRange.levelCount = 1;
        colorViewInfo.subresourceRange.baseArrayLayer = 0;
        colorViewInfo.subresourceRange.layerCount = 1;
        
        VK_CHECK(vkCreateImageView(device, &colorViewInfo, nullptr, &colorView));
        
        // depth image
        create_image(depthImage, depthMemory, VK_FORMAT_D32_SFLOAT,
                    VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT);
        
        VkImageViewCreateInfo depthViewInfo = colorViewInfo;
        depthViewInfo.image = depthImage;
        depthViewInfo.format = VK_FORMAT_D32_SFLOAT;
        depthViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
        
        VK_CHECK(vkCreateImageView(device, &depthViewInfo, nullptr, &depthView));
        
        VkImageView attachments[] = {colorView, depthView};
        
        VkFramebufferCreateInfo framebufferInfo = {};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = renderPass;
        framebufferInfo.attachmentCount = 2;
        framebufferInfo.pAttachments = attachments;
        framebufferInfo.width = width;
        framebufferInfo.height = height;
        framebufferInfo.layers = 1;
        
        VK_CHECK(vkCreateFramebuffer(device, &framebufferInfo, nullptr, &framebuffer));
        std::cout << "✓ framebuffer created (" << width << "x" << height << ")" << std::endl;
    }
    
    void create_command_pool() {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = queueFamily;
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        
        VK_CHECK(vkCreateCommandPool(device, &poolInfo, nullptr, &commandPool));
        
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.commandPool = commandPool;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandBufferCount = 1;
        
        VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer));
        std::cout << "✓ command pool created" << std::endl;
    }
    
    void create_pipeline(const char* frag_spirv_path) {
        // load shaders
        auto vertCode = load_spirv("spirv/basic_proper.vert.spv");
        auto fragCode = load_spirv(frag_spirv_path);
        
        VkShaderModuleCreateInfo vertModuleInfo = {};
        vertModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        vertModuleInfo.codeSize = vertCode.size() * 4;
        vertModuleInfo.pCode = vertCode.data();
        
        VkShaderModule vertModule;
        VK_CHECK(vkCreateShaderModule(device, &vertModuleInfo, nullptr, &vertModule));
        
        VkShaderModuleCreateInfo fragModuleInfo = {};
        fragModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        fragModuleInfo.codeSize = fragCode.size() * 4;
        fragModuleInfo.pCode = fragCode.data();
        
        VkShaderModule fragModule;
        VK_CHECK(vkCreateShaderModule(device, &fragModuleInfo, nullptr, &fragModule));
        
        VkPipelineShaderStageCreateInfo vertStage = {};
        vertStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        vertStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
        vertStage.module = vertModule;
        vertStage.pName = "main";
        
        VkPipelineShaderStageCreateInfo fragStage = {};
        fragStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
        fragStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
        fragStage.module = fragModule;
        fragStage.pName = "main";
        
        VkPipelineShaderStageCreateInfo stages[] = {vertStage, fragStage};
        
        // vertex input
        VkVertexInputBindingDescription binding = {};
        binding.binding = 0;
        binding.stride = sizeof(Vertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        
        VkVertexInputAttributeDescription attributes[3] = {};
        // position
        attributes[0].binding = 0;
        attributes[0].location = 0;
        attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[0].offset = offsetof(Vertex, pos);
        // normal
        attributes[1].binding = 0;
        attributes[1].location = 1;
        attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[1].offset = offsetof(Vertex, normal);
        // color
        attributes[2].binding = 0;
        attributes[2].location = 2;
        attributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
        attributes[2].offset = offsetof(Vertex, color);
        
        VkPipelineVertexInputStateCreateInfo vertexInput = {};
        vertexInput.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
        vertexInput.vertexBindingDescriptionCount = 1;
        vertexInput.pVertexBindingDescriptions = &binding;
        vertexInput.vertexAttributeDescriptionCount = 3;
        vertexInput.pVertexAttributeDescriptions = attributes;
        
        VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        
        VkViewport viewport = {};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)width;
        viewport.height = (float)height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        
        VkRect2D scissor = {};
        scissor.offset = {0, 0};
        scissor.extent = {width, height};
        
        VkPipelineViewportStateCreateInfo viewportState = {};
        viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.pViewports = &viewport;
        viewportState.scissorCount = 1;
        viewportState.pScissors = &scissor;
        
        VkPipelineRasterizationStateCreateInfo rasterizer = {};
        rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
        
        VkPipelineMultisampleStateCreateInfo multisampling = {};
        multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
        
        VkPipelineDepthStencilStateCreateInfo depthStencil = {};
        depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
        
        VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
        colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | 
                                             VK_COLOR_COMPONENT_G_BIT |
                                             VK_COLOR_COMPONENT_B_BIT | 
                                             VK_COLOR_COMPONENT_A_BIT;
        
        VkPipelineColorBlendStateCreateInfo colorBlending = {};
        colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        
        VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        
        VK_CHECK(vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &pipelineLayout));
        
        VkGraphicsPipelineCreateInfo pipelineInfo = {};
        pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = 2;
        pipelineInfo.pStages = stages;
        pipelineInfo.pVertexInputState = &vertexInput;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.layout = pipelineLayout;
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        
        VK_CHECK(vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &pipeline));
        
        vkDestroyShaderModule(device, vertModule, nullptr);
        vkDestroyShaderModule(device, fragModule, nullptr);
        
        std::cout << "✓ pipeline created with " << frag_spirv_path << std::endl;
    }
    
    void upload_vertices(const std::vector<Vertex>& verts) {
        vertices = verts;
        
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = sizeof(Vertex) * vertices.size();
        bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &vertexBuffer));
        
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, vertexBuffer, &memRequirements);
        
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &vertexMemory));
        vkBindBufferMemory(device, vertexBuffer, vertexMemory, 0);
        
        void* data;
        vkMapMemory(device, vertexMemory, 0, bufferInfo.size, 0, &data);
        memcpy(data, vertices.data(), bufferInfo.size);
        vkUnmapMemory(device, vertexMemory);
        
        std::cout << "✓ uploaded " << vertices.size() << " vertices" << std::endl;
    }
    
    void render() {
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        
        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));
        
        VkClearValue clearValues[2];
        clearValues[0].color = {{0.05f, 0.07f, 0.10f, 1.0f}};
        clearValues[1].depthStencil = {1.0f, 0};
        
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = renderPass;
        renderPassInfo.framebuffer = framebuffer;
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = {width, height};
        renderPassInfo.clearValueCount = 2;
        renderPassInfo.pClearValues = clearValues;
        
        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
        vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, pipeline);
        
        VkBuffer vertexBuffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, vertexBuffers, offsets);
        
        vkCmdDraw(commandBuffer, vertices.size(), 1, 0, 0);
        
        vkCmdEndRenderPass(commandBuffer);
        
        VK_CHECK(vkEndCommandBuffer(commandBuffer));
        
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        
        VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
        VK_CHECK(vkQueueWaitIdle(queue));
        
        std::cout << "✓ rendered frame" << std::endl;
    }
    
    void save_png(const char* path) {
        // create staging buffer to read back image
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = width * height * 4;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        
        VkBuffer stagingBuffer;
        VK_CHECK(vkCreateBuffer(device, &bufferInfo, nullptr, &stagingBuffer));
        
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(device, stagingBuffer, &memRequirements);
        
        VkMemoryAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocInfo.allocationSize = memRequirements.size;
        allocInfo.memoryTypeIndex = find_memory_type(memRequirements.memoryTypeBits,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
        
        VkDeviceMemory stagingMemory;
        VK_CHECK(vkAllocateMemory(device, &allocInfo, nullptr, &stagingMemory));
        vkBindBufferMemory(device, stagingBuffer, stagingMemory, 0);
        
        // copy image to buffer
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        VK_CHECK(vkBeginCommandBuffer(commandBuffer, &beginInfo));
        
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};
        
        vkCmdCopyImageToBuffer(commandBuffer, colorImage, 
                              VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                              stagingBuffer, 1, &region);
        
        VK_CHECK(vkEndCommandBuffer(commandBuffer));
        
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &commandBuffer;
        
        VK_CHECK(vkQueueSubmit(queue, 1, &submitInfo, VK_NULL_HANDLE));
        VK_CHECK(vkQueueWaitIdle(queue));
        
        // read back and save
        void* data;
        vkMapMemory(device, stagingMemory, 0, width * height * 4, 0, &data);
        
        stbi_write_png(path, width, height, 4, data, width * 4);
        
        vkUnmapMemory(device, stagingMemory);
        
        vkDestroyBuffer(device, stagingBuffer, nullptr);
        vkFreeMemory(device, stagingMemory, nullptr);
        
        std::cout << "✓ saved " << path << std::endl;
    }
    
    ~VulkanRenderer() {
        vkDeviceWaitIdle(device);
        
        if (pipeline) vkDestroyPipeline(device, pipeline, nullptr);
        if (pipelineLayout) vkDestroyPipelineLayout(device, pipelineLayout, nullptr);
        if (framebuffer) vkDestroyFramebuffer(device, framebuffer, nullptr);
        if (colorView) vkDestroyImageView(device, colorView, nullptr);
        if (depthView) vkDestroyImageView(device, depthView, nullptr);
        if (colorImage) vkDestroyImage(device, colorImage, nullptr);
        if (depthImage) vkDestroyImage(device, depthImage, nullptr);
        if (colorMemory) vkFreeMemory(device, colorMemory, nullptr);
        if (depthMemory) vkFreeMemory(device, depthMemory, nullptr);
        if (vertexBuffer) vkDestroyBuffer(device, vertexBuffer, nullptr);
        if (vertexMemory) vkFreeMemory(device, vertexMemory, nullptr);
        if (commandPool) vkDestroyCommandPool(device, commandPool, nullptr);
        if (renderPass) vkDestroyRenderPass(device, renderPass, nullptr);
        if (device) vkDestroyDevice(device, nullptr);
        if (instance) vkDestroyInstance(instance, nullptr);
    }
};

int main(int argc, char** argv) {
    if (argc < 4) {
        std::cerr << "usage: " << argv[0] << " <frag.spv> <mesh.obj> <output.png>" << std::endl;
        std::cerr << "example: " << argv[0] << " spirv/my_flat.spv assets/teapot.obj flat.png" << std::endl;
        return 1;
    }
    
    const char* frag_spv = argv[1];
    const char* obj_path = argv[2];
    const char* png_path = argv[3];
    
    std::cout << "=== vulkan spir-v viewer ===" << std::endl;
    std::cout << "fragment shader: " << frag_spv << std::endl;
    std::cout << "mesh: " << obj_path << std::endl;
    std::cout << "output: " << png_path << std::endl;
    std::cout << std::endl;
    
    VulkanRenderer renderer(800, 600);
    renderer.init();
    
    auto vertices = load_obj(obj_path);
    renderer.upload_vertices(vertices);
    
    renderer.create_pipeline(frag_spv);
    renderer.render();
    renderer.save_png(png_path);
    
    std::cout << std::endl << "✓ done!" << std::endl;
    return 0;
}