#ifndef VKSTRUCTS_H
#define VKSTRUCTS_H

#include "vk_getProcAddrs.h"
#include <array>

#define FENCE_TIMEOUT 0xFFFFFFFFFFFFFFFFL
#define NUM_DESCRIPTOR_SETS 2

inline size_t _getAlignedDataSize(size_t dataSize, size_t minimumAlignment){
    return (dataSize / minimumAlignment) * minimumAlignment + ((dataSize % minimumAlignment) > 0 ? minimumAlignment : 0);
}

struct SwapchainImage
{
    VkImage image;
    VkImageView view;
    VkImageView depth_view;
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

typedef struct
{
    union{
        float x;
        float u;
    };
    union{
        float y;
        float v;
    };
} VEC2;

typedef struct
{
    float x;	/*!< x coordinate */
    float y;	/*!< y coordinate */
    float z;	/*!< z coordinate */
} VEC3;

typedef struct
{
    float x;	/*!< x coordinate */
    float y;	/*!< y coordinate */
    float z;	/*!< z coordinate */
    float w;	/*!< w coordinate */
} VEC4;


struct Vertex
{
    VEC4 pos; // coordinates.
    VEC3 nor; // normals.
    VEC2 tex; // texture UVs.
};

struct Transform
{
    VEC3 translation;
    VEC4 rotation;
    VEC3 scale;
};

struct Mesh
{
    BufferData vertexBuffer;
    BufferData indexBuffer;
    uint32_t vertexCount;
    Transform transform;
};

struct Light
{
    uint32_t type;
    Transform transform;
};

struct Camera
{
    uint32_t type;
    Transform transform;
};

struct UBO
{
    std::array<std::array<float, 4>, 4> matrixMVP;
    VEC3 lightDirection;
};

struct AppManager
{
    const char* appName;
    std::vector<const char*> instanceLayerNames;
    std::vector<const char*> instanceExtensionNames;
    std::vector<const char*> deviceExtensionNames;

    std::vector<VkPhysicalDevice> gpus;
    std::vector<VkQueueFamilyProperties> queueFamilyProperties;
    std::vector<SwapchainImage> swapChainImages;
    std::vector<VkCommandBuffer> cmdBuffers;
    std::vector<VkFramebuffer> frameBuffers;
    std::vector<VkDescriptorSetLayout> descriptorSetLayouts;
    std::vector<Mesh> meshes;
    std::vector<Camera> cameras;
    std::vector<Light> lights;

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

    BufferData dynamicUniformBufferData;
    TextureData texture;

    VkImage depth_image;
    VkDeviceMemory depth_memory;

    float angle;
    uint32_t offset;

    int frameId;
    uint32_t currentBuffer;

    uint32_t padding;
};

// The Surface Data structure is different based on the platform being used.
// The structure is defined and its members, inside Vulkan-provided preprocessors.
#ifdef VK_USE_PLATFORM_WIN32_KHR
struct SurfaceData
{
    float width, height;

    HINSTANCE connection;
    HWND window;

    SurfaceData() {};
};
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
struct SurfaceData
{
    float width, height;

    Display* display;
    Window window;

    SurfaceData() {}
};
#endif

#ifdef VK_USE_PLATFORM_XCB_KHR
struct SurfaceData
{
    float width, height;

    xcb_connection_t* connection;
    xcb_screen_t* screen;
    xcb_window_t window;

    uint32_t deleteWindowAtom;

    SurfaceData() {}
};
#endif

#ifdef VK_USE_PLATFORM_ANDROID_KHR
struct SurfaceData
{
    float width, height;

    ANativeWindow* window;

    SurfaceData() { width = height = 0; }
};

#endif

#ifdef VK_USE_PLATFORM_WAYLAND_KHR
struct SurfaceData
{
    float width, height;

    wl_display* display;
    wl_surface* surface;
    wl_registry* wlRegistry;
    wl_compositor* wlCompositor;
    wl_shell* wlShell;
    wl_seat* wlSeat;
    wl_pointer* wlPointer;
    wl_shell_surface* wlShellSurface;

    SurfaceData()
    {
        width = height = 0;
        display = NULL;
        surface = NULL;
        wlRegistry = NULL;
        wlCompositor = NULL;
        wlShell = NULL;
        wlSeat = NULL;
        wlPointer = NULL;
        wlShellSurface = NULL;
    }
};
#endif

#ifdef VK_USE_PLATFORM_MACOS_MVK
struct SurfaceData
{
    float width, height;

    void* view;

    SurfaceData()
    {
        width = height = 0;
        view = NULL;
    }
};
#endif

#ifdef USE_PLATFORM_NULLWS
struct SurfaceData
{
    float width, height;

    VkDisplayKHR nativeDisplay;
    VkSurfaceKHR surface;

    SurfaceData()
    {
        nativeDisplay = VK_NULL_HANDLE;
        surface = VK_NULL_HANDLE;
        width = height = 0;
    }
};
#endif

#endif // VKSTRUCTS_H
