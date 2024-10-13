#ifndef VKSURFACES_H
#define VKSURFACES_H

#include <limits>
#include "vkStructs.h"

// currentBuffer will be used to point to the correct frame/command buffer/uniform buffer data.
// It is going to be the general index of the data being worked on.
inline void _startCurrentBuffer(AppManager& appManager)
{
    VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

    // Acquire and get the index of the next available swapchain image.
    debugAssertFunctionResult(
        vk::AcquireNextImageKHR(appManager.device, appManager.swapchain, std::numeric_limits<uint64_t>::max(), appManager.acquireSemaphore[appManager.frameId], VK_NULL_HANDLE, & appManager.currentBuffer),
        "Draw - Acquire Image");

    // Wait for the fence to be signalled before starting to render the current frame, then reset it so it can be reused.
    debugAssertFunctionResult(vk::WaitForFences(appManager.device, 1, &appManager.frameFences[appManager.currentBuffer], true, FENCE_TIMEOUT), "Fence - Signalled");

    vk::ResetFences(appManager.device, 1, &appManager.frameFences[appManager.currentBuffer]);
}

// Submit the command buffer to the queue to start rendering.
// The command buffer is submitted to the graphics queue which was created earlier.
// Notice the wait (acquire) and signal (present) semaphores, and the fence.
inline void _presentCurrentBuffer(AppManager& appManager){

    VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.pWaitDstStageMask = &pipe_stage_flags;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &appManager.acquireSemaphore[appManager.frameId];
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &appManager.presentSemaphores[appManager.frameId];
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &appManager.cmdBuffers[appManager.currentBuffer];

    debugAssertFunctionResult(vk::QueueSubmit(appManager.graphicQueue, 1, &submitInfo, appManager.frameFences[appManager.currentBuffer]), "Draw - Submit to Graphic Queue");

    // Queue the rendered image for presentation to the surface.
    // The currentBuffer is again used to select the correct swapchain images to present. A wait
    // semaphore is also set here which will be signalled when the command buffer has
    // finished execution.
    VkPresentInfoKHR presentInfo;
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.pNext = nullptr;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &appManager.swapchain;
    presentInfo.pImageIndices = &appManager.currentBuffer;
    presentInfo.pWaitSemaphores = &appManager.presentSemaphores[appManager.frameId];
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pResults = nullptr;

    debugAssertFunctionResult(vk::QueuePresentKHR(appManager.presentQueue, &presentInfo), "Draw - Submit to Present Queue");

    // Update the appManager.frameId to get the next suitable one.
    appManager.frameId = (appManager.frameId + 1) % appManager.swapChainImages.size();
}

/// <summary>Creates a number of framebuffer objects equal to the number of images in the swapchain</summary>
inline void _initFrameBuffers(AppManager& appManager)
{
    // Concept: Framebuffers
    // In Vulkan, all the attachments used by the render pass are defined in framebuffers. Each frame in a framebuffer defines
    // the attachments related to it. This includes the textures (including the colour and depth / stencil attachments) and
    // the input attachments. This way of separating descriptions in render passes and definitions in framebuffers gives the option
    // of using different render passes with different framebuffers. However, the degree of flexibility with which this can be done is based on the
    // compatibility of the two.

    // This function creates a framebuffer for each swapchain image.

    // This is a placeholder handle for the attachment which will be stored in the VkFramebufferCreateInfo.
    VkImageView attachment[] = {VK_NULL_HANDLE, VK_NULL_HANDLE};

    // Populate a framebuffer info struct with the information that is needed to create the framebuffers. This includes its dimensions, its attachments, and the associated render
    // pass that will use the specified attachments. The attachment member will be a null variable for now.
    VkFramebufferCreateInfo frameBufferInfo = {};
    frameBufferInfo.flags = 0;
    frameBufferInfo.pNext = nullptr;
    frameBufferInfo.attachmentCount = 2;
    frameBufferInfo.height = appManager.swapchainExtent.height;
    frameBufferInfo.width = appManager.swapchainExtent.width;
    frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    frameBufferInfo.renderPass = appManager.renderPass;
    frameBufferInfo.pAttachments = attachment;
    frameBufferInfo.layers = 1;

    // Resize the vector which will contain all of the framebuffers based on the number of images in the swap chain.
    appManager.frameBuffers.resize(appManager.swapChainImages.size());

    // Create as many framebuffer objects as swapchain images and assign each image to a framebuffer.
    // Note that, above, the pAttachments variable has been assigned to the address of the
    // local variable "attachment". Every time pAttachments is reassigned a new image is used during framebuffer creation.
    for (size_t i = 0; i < appManager.swapChainImages.size(); ++i)
    {
        attachment[0] = appManager.swapChainImages[i].view;
        attachment[1] = appManager.swapChainImages[i].depth_view;

        debugAssertFunctionResult(vk::CreateFramebuffer(appManager.device, &frameBufferInfo, nullptr, &appManager.frameBuffers[i]), "Swapchain Frame buffer creation");
    }
}


/// <summary>Sets up the view port and also sets up the scissor</summary>
inline void _initViewportAndScissor(AppManager& appManager, SurfaceData& surfaceData)
{
    // The viewport is essentially the dimensions of the rendering area and
    // the scissor is a sub-section of this viewport which is actually stored.

    // Viewport and scissors are set dynamically with vkCmdSetViewport and vkCmdSetScissor.

    // This code sets up the values that will be used by these commands. In this example,
    // the extents of the scissor are the same as the viewport.

    // Set the viewport dimensions, depth, and starting coordinates.
    appManager.viewport.width = surfaceData.width;
    appManager.viewport.height = surfaceData.height;
    appManager.viewport.minDepth = 0.0f;
    appManager.viewport.maxDepth = 1.0f;
    appManager.viewport.x = 0;
    appManager.viewport.y = 0;

    // Set the extent to the dimensions of the surface and set the offset in both directions to 0.
    appManager.scissor.extent.width = static_cast<uint32_t>(surfaceData.width);
    appManager.scissor.extent.height = static_cast<uint32_t>(surfaceData.height);
    appManager.scissor.offset.x = 0;
    appManager.scissor.offset.y = 0;

    // The viewport and scissor are now ready to be set.
}

/// <summary>Initialises the surface that will be presented to</summary>
inline void _initSurface(AppManager& appManager, SurfaceData& surfaceData)
{
    // This function initialises the surface that will be needed to present this rendered example.

// Surfaces are based on the platform (OS) that is being deployed to.
// Pre-processors are used to select the correct function call and info struct data type to create a surface.

// For Win32.
#ifdef VK_USE_PLATFORM_WIN32_KHR

    // Declare and populate the Win32 surface creation info struct with Win32 window instance and window handles.
    VkWin32SurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.flags = 0;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.hinstance = surfaceData.connection;
    surfaceInfo.hwnd = surfaceData.window;
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;

    // Create the surface that will be rendered on using the creation info defined above.
    debugAssertFunctionResult(vk::CreateWin32SurfaceKHR(appManager.instance, &surfaceInfo, nullptr, &appManager.surface), "Windows Surface Creation");
#endif

// For Xlib.
#ifdef VK_USE_PLATFORM_XLIB_KHR
    // Declare and populate the Xlib surface creation info struct, passing the Xlib display and window handles.
    VkXlibSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_XLIB_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.flags = 0;
    surfaceInfo.pNext = nullptr;
    surfaceInfo.dpy = surfaceData.display;
    surfaceInfo.window = surfaceData.window;

    // Create the xlib surface that will be presented on using the creation info defined above.
    debugAssertFunctionResult(vk::CreateXlibSurfaceKHR(appManager.instance, &surfaceInfo, nullptr, &appManager.surface), "XLIB Surface Creation");
#endif

// For Xcb.
#ifdef VK_USE_PLATFORM_XCB_KHR

    // Declare and populate the Xlib surface creation info struct, passing the Xcb display and window handles.
    VkXcbSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_XCB_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.connection = surfaceData.connection;
    surfaceInfo.window = surfaceData.window;

    // Create the xcb surface that will be presented on.
    debugAssertFunctionResult(vk::CreateXcbSurfaceKHR(appManager.instance, &surfaceInfo, nullptr, &appManager.surface), "XCB Surface Creation");
#endif

// For Android.
#ifdef VK_USE_PLATFORM_ANDROID_KHR

    // Declare and populate the Android surface creation info struct, passing the Android window handle.
    VkAndroidSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.flags = 0;
    surfaceInfo.pNext = 0;
    surfaceInfo.sType = VK_STRUCTURE_TYPE_ANDROID_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.window = surfaceData.window;

    // Create the Android surface that will be presented on using the creation info defined above.
    debugAssertFunctionResult(vk::CreateAndroidSurfaceKHR(appManager.instance, &surfaceInfo, nullptr, &appManager.surface), "Android Surface Creation");

#endif

// For Wayland.
#ifdef VK_USE_PLATFORM_WAYLAND_KHR

    // Declare and populate the Wayland surface creation info struct, passing the Wayland display and surface handles.
    VkWaylandSurfaceCreateInfoKHR surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_WAYLAND_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.display = surfaceData.display;
    surfaceInfo.surface = surfaceData.surface;

    // Create the Wayland surface that will be presented on using the creation info defined above.
    debugAssertFunctionResult(vk::CreateWaylandSurfaceKHR(appManager.instance, &surfaceInfo, NULL, &appManager.surface), "Wayland Surface Creation");

#endif

// For MoltenVK
#ifdef VK_USE_PLATFORM_MACOS_MVK

    // Create the MacOS surface info, passing the NSView handle
    VkMacOSSurfaceCreateInfoMVK surfaceInfo = {};
    surfaceInfo.sType = VK_STRUCTURE_TYPE_MACOS_SURFACE_CREATE_INFO_MVK;
    surfaceInfo.pNext = 0;
    surfaceInfo.flags = 0;
    // pView must be a valid NSView and must be backed by a CALayer instance of type CAMetalLayer.
    surfaceInfo.pView = surfaceData.view;

    // Create the MacOS surface that will be presented on.
    debugAssertFunctionResult(vk::CreateMacOSSurfaceMVK(appManager.instance, &surfaceInfo, NULL, &appManager.surface), "MacOS Surface Creation");

#endif

// For NullWS
#ifdef USE_PLATFORM_NULLWS

    VkDisplayPropertiesKHR properties;
    uint32_t propertiesCount = 1;
    if (vk::GetPhysicalDeviceDisplayPropertiesKHR) { vk::GetPhysicalDeviceDisplayPropertiesKHR(appManager.physicalDevice, &propertiesCount, &properties); }

    std::string supportedTransforms;
    if (properties.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) { supportedTransforms.append("none "); }
    if (properties.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_90_BIT_KHR) { supportedTransforms.append("rot90 "); }
    if (properties.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_180_BIT_KHR) { supportedTransforms.append("rot180 "); }
    if (properties.supportedTransforms & VK_SURFACE_TRANSFORM_ROTATE_270_BIT_KHR) { supportedTransforms.append("rot270 "); }
    if (properties.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_BIT_KHR) { supportedTransforms.append("h_mirror "); }
    if (properties.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_90_BIT_KHR) { supportedTransforms.append("h_mirror+rot90 "); }
    if (properties.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_180_BIT_KHR) { supportedTransforms.append("hmirror+rot180 "); }
    if (properties.supportedTransforms & VK_SURFACE_TRANSFORM_HORIZONTAL_MIRROR_ROTATE_270_BIT_KHR) { supportedTransforms.append("hmirror+rot270 "); }
    if (properties.supportedTransforms & VK_SURFACE_TRANSFORM_INHERIT_BIT_KHR) { supportedTransforms.append("inherit "); }

    VkDisplayKHR nativeDisplay = properties.display;

    uint32_t modeCount = 0;
    vk::GetDisplayModePropertiesKHR(appManager.physicalDevice, nativeDisplay, &modeCount, NULL);
    std::vector<VkDisplayModePropertiesKHR> modeProperties;
    modeProperties.resize(modeCount);
    vk::GetDisplayModePropertiesKHR(appManager.physicalDevice, nativeDisplay, &modeCount, modeProperties.data());

    VkDisplaySurfaceCreateInfoKHR surfaceInfo = {};

    surfaceInfo.sType = VK_STRUCTURE_TYPE_DISPLAY_SURFACE_CREATE_INFO_KHR;
    surfaceInfo.pNext = NULL;

    surfaceInfo.displayMode = modeProperties[0].displayMode;
    surfaceInfo.planeIndex = 0;
    surfaceInfo.planeStackIndex = 0;
    surfaceInfo.transform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    surfaceInfo.globalAlpha = 0.0f;
    surfaceInfo.alphaMode = VK_DISPLAY_PLANE_ALPHA_PER_PIXEL_BIT_KHR;
    surfaceInfo.imageExtent = modeProperties[0].parameters.visibleRegion;

    debugAssertFunctionResult(vk::CreateDisplayPlaneSurfaceKHR(appManager.instance, &surfaceInfo, nullptr, &appManager.surface), "Surface Creation");
#endif
}

/// <summary>Checks if the extents are correct based on the capabilities of the surface</summary>
/// <param name="inSurfCap">Capabilities of the current surface</param>
/// <returns>A valid correct extent</returns>
inline VkExtent2D _getCorrectExtent(AppManager& appManager, SurfaceData& surfaceData, const VkSurfaceCapabilitiesKHR& inSurfCap)
{
    // This function makes sure the extents are correct for the surface, based on the surface
    // capabilities. It also checks whether the extents are valid and the same as the one picked in
    // initSurface().

    // The width and height of the swapchain are either both 0xFFFFFFFF (max value for uint_32t) or they are both NOT 0xFFFFFFFF.
    if (inSurfCap.currentExtent.width == 0xFFFFFFFF || inSurfCap.currentExtent.height == 0xFFFFFFFF)
    {
        // Pass the width and height from the surface.
        appManager.swapchainExtent.width = static_cast<uint32_t>(surfaceData.width);
        appManager.swapchainExtent.height = static_cast<uint32_t>(surfaceData.height);
        VkExtent2D currentExtent = appManager.swapchainExtent;

        // The swapchain extent width and height cannot be less than the minimum surface capability or greater than
        // the maximum surface capability.
        if (appManager.swapchainExtent.width < inSurfCap.minImageExtent.width) { currentExtent.width = inSurfCap.minImageExtent.width; }
        else if (appManager.swapchainExtent.width > inSurfCap.maxImageExtent.width)
        {
            currentExtent.width = inSurfCap.maxImageExtent.width;
        }

        if (appManager.swapchainExtent.height < inSurfCap.minImageExtent.height) { currentExtent.height = inSurfCap.minImageExtent.height; }
        else if (appManager.swapchainExtent.height > inSurfCap.maxImageExtent.height)
        {
            currentExtent.height = inSurfCap.maxImageExtent.height;
        }

        // If the extents are zero, use the values picked from the surface data.
        if (currentExtent.width == 0 && currentExtent.height == 0)
        {
            currentExtent.width = static_cast<uint32_t>(surfaceData.width);
            currentExtent.height = static_cast<uint32_t>(surfaceData.height);
        }

        return currentExtent;
    }

    // In the case where the width and height are both not 0xFFFFFFFF, make sure the extents are not zero.
    // As before, if they are zero then use values picked from the surface data.
    if (inSurfCap.currentExtent.width == 0 && inSurfCap.currentExtent.height == 0)
    {
        VkExtent2D currentExtent;
        currentExtent.width = static_cast<uint32_t>(surfaceData.width);
        currentExtent.height = static_cast<uint32_t>(surfaceData.height);
        return currentExtent;
    }

    return inSurfCap.currentExtent;
}

/// <summary>Creates a swapchain and defines its properties</summary>
inline void _initSwapChain(AppManager& appManager, SurfaceData& surfaceData)
{
    // If an application being developed needs to display something, then a swapchain is required.
    // This function creates a swapchain and defines its properties.

    // Concept: Swapchain
    // A swapchain is a series of images that are used to render and then present to the surface.
    // On changing the screen size or other changes, the swapchain needs to be destroyed
    // and recreated at runtime.

    // These variables are used to store the surface formats that have been retrieved from the physical device.
    uint32_t formatsCount;
    std::vector<VkSurfaceFormatKHR> formats;

    // Get the number of surface formats supported by the physical device.
    debugAssertFunctionResult(vk::GetPhysicalDeviceSurfaceFormatsKHR(appManager.physicalDevice, appManager.surface, &formatsCount, nullptr), "Swap Chain Format - Get Count");

    // Resize formats vector to the number of supported surface formats.
    formats.resize(formatsCount);

    // Populate the vector list with the surface formats.
    debugAssertFunctionResult(vk::GetPhysicalDeviceSurfaceFormatsKHR(appManager.physicalDevice, appManager.surface, &formatsCount, formats.data()), "Swap Chain Format - Allocate Data");

    // If the first format is undefined then pick a default format. VK_FORMAT_B8G8R8A8_UNORM is a very common image format.
    // Otherwise if the first format is defined choose that one.
    if (formatsCount == 1 && formats[0].format == VK_FORMAT_UNDEFINED)
    {
        appManager.surfaceFormat.format = VK_FORMAT_B8G8R8A8_UNORM; // unsigned normalised BGRA with 8-bit in each component.
    }
    else
    {
        appManager.surfaceFormat = formats[0];
    }

    // Get the surface capabilities from the surface and the physical device.
    VkSurfaceCapabilitiesKHR surface_capabilities;
    debugAssertFunctionResult(vk::GetPhysicalDeviceSurfaceCapabilitiesKHR(appManager.physicalDevice, appManager.surface, &surface_capabilities), "Fetch Surface Capabilities");

    // Concept: Present Modes
    // Present modes are the methods with which images are presented to the surface.

    // The presentation modes that are supported by the surface need to be determined.

    // These variables are used to store the presentation modes that have been retrieved from the physical device.
    uint32_t presentModesCount;
    std::vector<VkPresentModeKHR> presentModes;

    // Get the number of supported present modes.
    debugAssertFunctionResult(
        vk::GetPhysicalDeviceSurfacePresentModesKHR(appManager.physicalDevice, appManager.surface, &presentModesCount, nullptr), "Surface Present Modes - Get Count");

    // Resize the vector and retrieve the supported present modes.
    presentModes.resize(presentModesCount);
    debugAssertFunctionResult(vk::GetPhysicalDeviceSurfacePresentModesKHR(appManager.physicalDevice, appManager.surface, &presentModesCount, presentModes.data()),
                              "Surface Present Modes - Allocate Data");

    // Make use of VK_PRESENT_MODE_FIFO_KHR for presentation.
    appManager.presentMode = VK_PRESENT_MODE_FIFO_KHR;

    // Get the correct extent (dimensions) of the surface using a helper function.
    appManager.swapchainExtent = _getCorrectExtent(appManager, surfaceData, surface_capabilities);

    // Get the minimum number of images supported on this surface.
    uint32_t surfaceImageCount = std::max<uint32_t>(3, surface_capabilities.minImageCount);

    // Populate a swapchain creation info struct with the information specified above.
    // The additional parameters specified here include what transformations to apply to the image before
    // presentation, how this surface will be composited with other surfaces, whether the implementation
    // can discard rendering operations that affect regions of the surface that are not visible, and the intended
    // usage of the swapchain images.
    VkSwapchainCreateInfoKHR swapchainInfo = {};
    swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
    swapchainInfo.flags = 0;
    swapchainInfo.pNext = nullptr;
    swapchainInfo.surface = appManager.surface;
    swapchainInfo.imageFormat = appManager.surfaceFormat.format;
    swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
    if ((surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR) == 0)
    {
        Log(true, "Surface does not support VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR transformation");
        assert(surface_capabilities.supportedTransforms & VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR);
    }

    VkCompositeAlphaFlagBitsKHR supportedCompositeAlphaFlags = (VkCompositeAlphaFlagBitsKHR)0;
    if ((surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR) != 0) { supportedCompositeAlphaFlags = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; }
    else if ((surface_capabilities.supportedCompositeAlpha & VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR) != 0)
    {
        supportedCompositeAlphaFlags = VK_COMPOSITE_ALPHA_INHERIT_BIT_KHR;
    }
    swapchainInfo.compositeAlpha = supportedCompositeAlphaFlags;
    swapchainInfo.presentMode = appManager.presentMode;
    swapchainInfo.minImageCount = surfaceImageCount;
    swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
    swapchainInfo.clipped = VK_TRUE; // TODO Enable proper clipping of poligons VK_POINT_CLIPPING_BEHAVIOR_USER_CLIP_PLANES_ONLY
    swapchainInfo.imageExtent.width = appManager.swapchainExtent.width;
    swapchainInfo.imageExtent.height = appManager.swapchainExtent.height;
    swapchainInfo.imageArrayLayers = 1;
    swapchainInfo.imageColorSpace = appManager.surfaceFormat.colorSpace;
    swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

    // Fix the height and width of the surface in case they are not defined.
    if (surfaceData.width == 0 || surfaceData.height == 0)
    {
        surfaceData.width = static_cast<float>(swapchainInfo.imageExtent.width);
        surfaceData.height = static_cast<float>(swapchainInfo.imageExtent.height);
    }

    // Check if the present queue and the graphics queue are the same.
    // If they are, images do not need to be shared between multiple queues, so exclusive mode is selected.
    // If not, sharing mode concurrent is selected to allow these images to be accessed from multiple queue families simultaneously.
    if (appManager.graphicsQueueFamilyIndex == appManager.presentQueueFamilyIndex)
    {
        swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        swapchainInfo.queueFamilyIndexCount = 0;
        swapchainInfo.pQueueFamilyIndices = nullptr;
    }
    else
    {
        uint32_t queueFamilyIndices[] = { appManager.graphicsQueueFamilyIndex, appManager.presentQueueFamilyIndex };

        swapchainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
        swapchainInfo.queueFamilyIndexCount = 2;
        swapchainInfo.pQueueFamilyIndices = queueFamilyIndices;
    }

    // Finally, create the swapchain.
    debugAssertFunctionResult(vk::CreateSwapchainKHR(appManager.device, &swapchainInfo, nullptr, &appManager.swapchain), "SwapChain Creation");
}

inline uint32_t findMemoryType(AppManager& appManager, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
    VkPhysicalDeviceMemoryProperties memProperties;
    vk::GetPhysicalDeviceMemoryProperties(appManager.physicalDevice, &memProperties);

    for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
        if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
            return i;
        }
    }
    return 0;
}

inline void createImage(AppManager& appManager,
                        uint32_t width, uint32_t height,
                        VkFormat format,
                        VkImageUsageFlags usage,
                        VkMemoryPropertyFlags properties,
                        VkImage& image, VkDeviceMemory& imageMemory) {
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

    debugAssertFunctionResult(vk::CreateImage(appManager.device, &imageInfo, nullptr, &image), "createImage - CreateImage");

    VkMemoryRequirements memRequirements;
    vk::GetImageMemoryRequirements(appManager.device, image, &memRequirements);

    VkMemoryAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocInfo.allocationSize = memRequirements.size;
    allocInfo.memoryTypeIndex = findMemoryType(appManager, memRequirements.memoryTypeBits, properties);

    debugAssertFunctionResult(vk::AllocateMemory(appManager.device, &allocInfo, nullptr, &imageMemory), "createImage - AllocateMemory");

    debugAssertFunctionResult(vk::BindImageMemory(appManager.device, image, imageMemory, 0), "createImage - BindImageMemory");
}

/// <summary>Initialises the images of a previously created swapchain and creates an associated image view for each image</summary>
inline void _initImagesAndViews(AppManager& appManager)
{
    // Concept: Images and Views
    // Images in Vulkan are the object representation of data. It can take many forms such as attachments, textures, and so on.
    // Views are a snapshot of the image's parameters. It describes how to access the image and which parts to access.
    // It helps to distinguish the type of image that is being working with.

    // Image objects are used to hold the swapchain images. When the swapchain was created, the
    // images were automatically created alongside it. This function creates an image view for each swapchain image.

    // This vector is used as a temporary vector to hold the retrieved images.
    uint32_t swapchainImageCount;
    std::vector<VkImage> images;
    std::vector<VkImage> depth_images;
    std::vector<VkDeviceMemory> depth_memory;

    // Get the number of the images which are held by the swapchain. This is set in InitSwapchain function and is the minimum number of images supported.
    debugAssertFunctionResult(vk::GetSwapchainImagesKHR(appManager.device, appManager.swapchain, &swapchainImageCount, nullptr), "SwapChain Images - Get Count");

    // Resize the temporary images vector to hold the number of images.
    images.resize(swapchainImageCount);
    depth_images.resize(swapchainImageCount);
    depth_memory.resize(swapchainImageCount);

    // Resize the application's permanent swapchain images vector to be able to hold the number of images.
    appManager.swapChainImages.resize(swapchainImageCount);

    // Get all of the images from the swapchain and save them in a temporary vector.
    debugAssertFunctionResult(vk::GetSwapchainImagesKHR(appManager.device, appManager.swapchain, &swapchainImageCount, images.data()), "SwapChain Images - Allocate Data");


  //  appManager.depth_image = depth_image; // The swapchain can use the same depth buffer for each image as it is cleared at the render start. The views have to be one per chain image
  //  appManager.depth_memory = depth_memory;

    // Iterate over each image in order to create an image view for each one.
    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        // Create a depth buffer per chain
        createImage(appManager,
                    appManager.swapchainExtent.width, appManager.swapchainExtent.height,
                    VK_FORMAT_D32_SFLOAT, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    depth_images[i], depth_memory[i]);

        // Copy over the images to the permanent vector.
        appManager.swapChainImages[i].image = images[i];

        // Create the image view object itself, referencing one of the retrieved swapchain images.
        VkImageViewCreateInfo image_view_info = {};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.pNext = nullptr;
        image_view_info.flags = 0;
        image_view_info.image = appManager.swapChainImages[i].image;
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = appManager.surfaceFormat.format;

        image_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
        image_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
        image_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
        image_view_info.components.a = VK_COMPONENT_SWIZZLE_A;

        image_view_info.subresourceRange.layerCount = 1;
        image_view_info.subresourceRange.levelCount = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        // Create the image view object itself, referencing one of the retrieved swapchain images.
        VkImageViewCreateInfo image_depth_view_info = {};
        image_depth_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_depth_view_info.pNext = nullptr;
        image_depth_view_info.image = depth_images[i];
        image_depth_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_depth_view_info.format = VK_FORMAT_D32_SFLOAT;
        image_depth_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;

        debugAssertFunctionResult(vk::CreateImageView(appManager.device, &image_view_info, nullptr, &appManager.swapChainImages[i].view), "SwapChain Images View Creation");
        debugAssertFunctionResult(vk::CreateImageView(appManager.device, &image_depth_view_info, nullptr, &appManager.swapChainImages[i].depth_view), "SwapChain Images DepthBuffer View Creation");
    }
}

#endif // VKSURFACES_H
