#ifndef VKSTRUCTS_H
#define VKSTRUCTS_H

#include <limits>

#include "vk_getProcAddrs.h"

// Constants used throughout the example.
#define FENCE_TIMEOUT std::numeric_limits<uint64_t>::max()
#define NUM_DESCRIPTOR_SETS 2

struct SwapchainImage
{
    VkImage image;
    VkImageView view;
};

struct BufferData
{
    VkBuffer buffer;
    VkDeviceMemory memory;
    size_t size;
    VkMemoryPropertyFlags memPropFlags;
    void* mappedData;
    VkDescriptorBufferInfo bufferInfo;

    BufferData() : buffer(VK_NULL_HANDLE), memory(VK_NULL_HANDLE), size(0), memPropFlags(0), mappedData(nullptr) {}
};

struct TextureData
{
    std::vector<uint8_t> data;
    VkExtent2D textureDimensions;
    VkImage image;
    VkDeviceMemory memory;
    VkImageView view;
    VkSampler sampler;
};

struct AppManager
{
    std::vector<const char*> instanceLayerNames;
    std::vector<const char*> instanceExtensionNames;
    std::vector<const char*> deviceExtensionNames;

    std::vector<VkPhysicalDevice> gpus;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    std::vector<SwapchainImage> swapChainImages;
    std::vector<VkCommandBuffer> cmdBuffers;
    std::vector<VkFramebuffer> frameBuffers;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;

    std::vector<VkSemaphore> acquireSemaphore;
    std::vector<VkSemaphore> presentSemaphores;
    std::vector<VkFence> frameFences;

    VkInstance instance;
    VkPhysicalDevice physicalDevice;

    VkPhysicalDeviceMemoryProperties deviceMemoryProperties;
    VkPhysicalDeviceProperties deviceProperties;
    uint32_t graphicsQueueFamilyIndex;
    uint32_t presentQueueFamilyIndex;
    VkDevice device;
    VkQueue graphicQueue;
    VkQueue presentQueue;
    VkSurfaceKHR surface;
    VkSurfaceFormatKHR surfaceFormat;
    VkSwapchainKHR swapchain;
    VkPresentModeKHR presentMode;
    VkExtent2D swapchainExtent;
    VkPipelineShaderStageCreateInfo shaderStages[2];
    VkRenderPass renderPass;
    VkPipelineLayout pipelineLayout;
    VkPipeline pipeline;
    VkCommandPool commandPool;
    VkViewport viewport;
    VkRect2D scissor;
    VkDescriptorPool descriptorPool;
    VkDescriptorSet dynamicDescSet;
    VkDescriptorSet staticDescSet;
    VkDescriptorSetLayout staticDescriptorSetLayout;
    VkDescriptorSetLayout dynamicDescriptorSetLayout;

    BufferData vertexBuffer;
    BufferData dynamicUniformBufferData;
    TextureData texture;

    float angle;
    uint32_t offset;
};

struct Vertex
{
    float x, y, z, w; // coordinates.
    float u, v; // texture UVs.
};

#endif // VKSTRUCTS_H
