/*!*********************************************************************************************************************
\File         vk_getProcAddrs.cpp
\Title        Vulkan GetProcAddrs
\Author       PowerVR by Imagination, Developer Technology Team.
\Copyright    Copyright(c) Imagination Technologies Limited.
\brief        Getting the function pointers for the Vulkan API functions. Doing this avoids the Vulkan loader.
***********************************************************************************************************************/
#include "vk_getProcAddrs.h"

#undef PVR_VULKAN_FUNCTION_POINTER_DECLARATION
#define PVR_VULKAN_FUNCTION_POINTER_DEFINITION(function_name) PFN_vk##function_name vk::function_name;

#ifdef VK_USE_PLATFORM_WIN32_KHR
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateWin32SurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateAndroidSurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateXlibSurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateXcbSurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateWaylandSurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateMacOSSurfaceMVK)
#endif

#ifdef USE_PLATFORM_NULLWS
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceDisplayPropertiesKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetDisplayModePropertiesKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateDisplayPlaneSurfaceKHR)
#endif

PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceSurfaceCapabilitiesKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceSurfaceFormatsKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateSwapchainKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetSwapchainImagesKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(AcquireNextImageKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(QueuePresentKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroySwapchainKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateInstance)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(EnumeratePhysicalDevices)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyInstance)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceSurfacePresentModesKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceSurfaceSupportKHR)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroySurfaceKHR)

PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetInstanceProcAddr)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetDeviceProcAddr)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceFeatures)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceFormatProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceImageFormatProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceQueueFamilyProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceMemoryProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateDevice)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyDevice)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(EnumerateInstanceExtensionProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(EnumerateDeviceExtensionProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(EnumerateInstanceLayerProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(EnumerateDeviceLayerProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetDeviceQueue)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(QueueSubmit)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(QueueWaitIdle)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DeviceWaitIdle)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(AllocateMemory)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(FreeMemory)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(MapMemory)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(UnmapMemory)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(FlushMappedMemoryRanges)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(InvalidateMappedMemoryRanges)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetDeviceMemoryCommitment)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(BindBufferMemory)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(BindImageMemory)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetBufferMemoryRequirements)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetImageMemoryRequirements)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetImageSparseMemoryRequirements)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPhysicalDeviceSparseImageFormatProperties)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(QueueBindSparse)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateFence)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyFence)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(ResetFences)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetFenceStatus)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(WaitForFences)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateSemaphore)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroySemaphore)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateEvent)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyEvent)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetEventStatus)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(SetEvent)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(ResetEvent)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateQueryPool)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyQueryPool)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetQueryPoolResults)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateBufferView)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyBufferView)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateImage)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyImage)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetImageSubresourceLayout)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateImageView)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyImageView)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateShaderModule)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyShaderModule)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreatePipelineCache)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyPipelineCache)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetPipelineCacheData)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(MergePipelineCaches)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateGraphicsPipelines)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateComputePipelines)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyPipeline)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreatePipelineLayout)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyPipelineLayout)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateSampler)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroySampler)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateDescriptorSetLayout)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyDescriptorSetLayout)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateDescriptorPool)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyDescriptorPool)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(ResetDescriptorPool)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(AllocateDescriptorSets)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(FreeDescriptorSets)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(UpdateDescriptorSets)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateFramebuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyFramebuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateRenderPass)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyRenderPass)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(GetRenderAreaGranularity)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateCommandPool)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyCommandPool)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(ResetCommandPool)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(AllocateCommandBuffers)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(FreeCommandBuffers)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(BeginCommandBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(EndCommandBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(ResetCommandBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdBindPipeline)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetViewport)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetScissor)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetLineWidth)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetDepthBias)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetBlendConstants)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetDepthBounds)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetStencilCompareMask)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetStencilWriteMask)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetStencilReference)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdBindDescriptorSets)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdBindIndexBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdBindVertexBuffers)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdDraw)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdDrawIndexed)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdDrawIndirect)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdDrawIndexedIndirect)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdDispatch)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdDispatchIndirect)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdCopyBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdCopyImage)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdBlitImage)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdCopyBufferToImage)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdCopyImageToBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdUpdateBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdFillBuffer)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdClearColorImage)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdClearDepthStencilImage)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdClearAttachments)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdResolveImage)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdSetEvent)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdResetEvent)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdWaitEvents)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdPipelineBarrier)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdBeginQuery)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdEndQuery)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdResetQueryPool)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdWriteTimestamp)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdCopyQueryPoolResults)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdPushConstants)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdBeginRenderPass)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdNextSubpass)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdEndRenderPass)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CmdExecuteCommands)

PVR_VULKAN_FUNCTION_POINTER_DEFINITION(CreateDebugReportCallbackEXT)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DebugReportMessageEXT)
PVR_VULKAN_FUNCTION_POINTER_DEFINITION(DestroyDebugReportCallbackEXT)

#undef PVR_VULKAN_FUNCTION_POINTER_DEFINITION

static NativeLibrary& vkglueLib()
{
#if _WIN32
	static NativeLibrary mylib("vulkan-1.dll");
#endif
#if _LINUX || _ANDROID
	static NativeLibrary mylib("libvulkan.so");
#endif
#if _APPLE
	static NativeLibrary mylib("libMoltenVK.dylib");
#endif
	return mylib;
}

bool vk::initVulkan()
{
	GetInstanceProcAddr = (PFN_vkGetInstanceProcAddr)vkglueLib().getFunction("vkGetInstanceProcAddr");

	if (GetInstanceProcAddr)
	{
        VULKAN_GET_INSTANCE_POINTER(VK_NULL_HANDLE, EnumerateInstanceExtensionProperties);
        VULKAN_GET_INSTANCE_POINTER(VK_NULL_HANDLE, EnumerateInstanceLayerProperties);
        VULKAN_GET_INSTANCE_POINTER(VK_NULL_HANDLE, CreateInstance);
	}

	if (!GetInstanceProcAddr || !EnumerateInstanceExtensionProperties || !EnumerateInstanceLayerProperties || !CreateInstance || !DestroyInstance) { return false; }

	return true;
}

bool vk::initVulkanInstance(VkInstance instance)
{
#ifdef VK_USE_PLATFORM_WIN32_KHR
    VULKAN_GET_INSTANCE_POINTER(instance, CreateWin32SurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    VULKAN_GET_INSTANCE_POINTER(instance, CreateAndroidSurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    VULKAN_GET_INSTANCE_POINTER(instance, CreateXlibSurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    VULKAN_GET_INSTANCE_POINTER(instance, CreateXcbSurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    VULKAN_GET_INSTANCE_POINTER(instance, CreateWaylandSurfaceKHR)
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
    VULKAN_GET_INSTANCE_POINTER(instance, CreateMacOSSurfaceMVK)
#endif
#ifdef USE_PLATFORM_NULLWS
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceDisplayPropertiesKHR)
    VULKAN_GET_INSTANCE_POINTER(instance, GetDisplayModePropertiesKHR)
    VULKAN_GET_INSTANCE_POINTER(instance, CreateDisplayPlaneSurfaceKHR)
#endif

    VULKAN_GET_INSTANCE_POINTER(instance, DestroyInstance)
    VULKAN_GET_INSTANCE_POINTER(instance, EnumerateDeviceLayerProperties)
    VULKAN_GET_INSTANCE_POINTER(instance, EnumerateDeviceExtensionProperties)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceSurfaceCapabilitiesKHR)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceSurfaceFormatsKHR)
    VULKAN_GET_INSTANCE_POINTER(instance, EnumeratePhysicalDevices)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceQueueFamilyProperties)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceFeatures)
    VULKAN_GET_INSTANCE_POINTER(instance, CreateDevice)
    VULKAN_GET_INSTANCE_POINTER(instance, GetDeviceProcAddr)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceMemoryProperties)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceSurfacePresentModesKHR)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceSurfaceSupportKHR)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceFormatProperties)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceProperties)
    VULKAN_GET_INSTANCE_POINTER(instance, DestroySurfaceKHR)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceImageFormatProperties)
    VULKAN_GET_INSTANCE_POINTER(instance, GetPhysicalDeviceSparseImageFormatProperties)

    VULKAN_GET_INSTANCE_POINTER(instance, CreateDebugReportCallbackEXT)
    VULKAN_GET_INSTANCE_POINTER(instance, DebugReportMessageEXT)
    VULKAN_GET_INSTANCE_POINTER(instance, DestroyDebugReportCallbackEXT)

	if (!DestroyInstance || !EnumerateDeviceLayerProperties || !EnumerateDeviceExtensionProperties || !GetPhysicalDeviceSurfaceCapabilitiesKHR || !GetPhysicalDeviceSurfaceFormatsKHR ||
		!EnumeratePhysicalDevices || !GetPhysicalDeviceQueueFamilyProperties || !GetPhysicalDeviceFeatures || !CreateDevice || !GetDeviceProcAddr ||
		!GetPhysicalDeviceMemoryProperties || !GetPhysicalDeviceSurfacePresentModesKHR || !GetPhysicalDeviceSurfaceSupportKHR || !GetPhysicalDeviceFormatProperties ||
		!GetPhysicalDeviceProperties || !DestroySurfaceKHR || !GetPhysicalDeviceImageFormatProperties || !GetPhysicalDeviceSparseImageFormatProperties)
	{ return false; }

	return true;
}

bool vk::initVulkanDevice(VkDevice device)
{
    VULKAN_GET_DEVICE_POINTER(device, CreateSwapchainKHR)
    VULKAN_GET_DEVICE_POINTER(device, GetSwapchainImagesKHR)
    VULKAN_GET_DEVICE_POINTER(device, AcquireNextImageKHR)
    VULKAN_GET_DEVICE_POINTER(device, QueuePresentKHR)
    VULKAN_GET_DEVICE_POINTER(device, DestroySwapchainKHR)

    VULKAN_GET_DEVICE_POINTER(device, DestroyDevice)
    VULKAN_GET_DEVICE_POINTER(device, GetDeviceQueue)
    VULKAN_GET_DEVICE_POINTER(device, QueueSubmit)
    VULKAN_GET_DEVICE_POINTER(device, QueueWaitIdle)
    VULKAN_GET_DEVICE_POINTER(device, DeviceWaitIdle)
    VULKAN_GET_DEVICE_POINTER(device, AllocateMemory)
    VULKAN_GET_DEVICE_POINTER(device, FreeMemory)
    VULKAN_GET_DEVICE_POINTER(device, MapMemory)
    VULKAN_GET_DEVICE_POINTER(device, UnmapMemory)
    VULKAN_GET_DEVICE_POINTER(device, FlushMappedMemoryRanges)
    VULKAN_GET_DEVICE_POINTER(device, InvalidateMappedMemoryRanges)
    VULKAN_GET_DEVICE_POINTER(device, GetDeviceMemoryCommitment)
    VULKAN_GET_DEVICE_POINTER(device, BindBufferMemory)
    VULKAN_GET_DEVICE_POINTER(device, BindImageMemory)
    VULKAN_GET_DEVICE_POINTER(device, GetBufferMemoryRequirements)
    VULKAN_GET_DEVICE_POINTER(device, GetImageMemoryRequirements)
    VULKAN_GET_DEVICE_POINTER(device, GetImageSparseMemoryRequirements)
    VULKAN_GET_DEVICE_POINTER(device, QueueBindSparse)
    VULKAN_GET_DEVICE_POINTER(device, CreateFence)
    VULKAN_GET_DEVICE_POINTER(device, DestroyFence)
    VULKAN_GET_DEVICE_POINTER(device, ResetFences)
    VULKAN_GET_DEVICE_POINTER(device, GetFenceStatus)
    VULKAN_GET_DEVICE_POINTER(device, WaitForFences)
    VULKAN_GET_DEVICE_POINTER(device, CreateSemaphore)
    VULKAN_GET_DEVICE_POINTER(device, DestroySemaphore)
    VULKAN_GET_DEVICE_POINTER(device, CreateEvent)
    VULKAN_GET_DEVICE_POINTER(device, DestroyEvent)
    VULKAN_GET_DEVICE_POINTER(device, GetEventStatus)
    VULKAN_GET_DEVICE_POINTER(device, SetEvent)
    VULKAN_GET_DEVICE_POINTER(device, ResetEvent)
    VULKAN_GET_DEVICE_POINTER(device, CreateQueryPool)
    VULKAN_GET_DEVICE_POINTER(device, DestroyQueryPool)
    VULKAN_GET_DEVICE_POINTER(device, GetQueryPoolResults)
    VULKAN_GET_DEVICE_POINTER(device, CreateBuffer)
    VULKAN_GET_DEVICE_POINTER(device, DestroyBuffer)
    VULKAN_GET_DEVICE_POINTER(device, CreateBufferView)
    VULKAN_GET_DEVICE_POINTER(device, DestroyBufferView)
    VULKAN_GET_DEVICE_POINTER(device, CreateImage)
    VULKAN_GET_DEVICE_POINTER(device, DestroyImage)
    VULKAN_GET_DEVICE_POINTER(device, GetImageSubresourceLayout)
    VULKAN_GET_DEVICE_POINTER(device, CreateImageView)
    VULKAN_GET_DEVICE_POINTER(device, DestroyImageView)
    VULKAN_GET_DEVICE_POINTER(device, CreateShaderModule)
    VULKAN_GET_DEVICE_POINTER(device, DestroyShaderModule)
    VULKAN_GET_DEVICE_POINTER(device, CreatePipelineCache)
    VULKAN_GET_DEVICE_POINTER(device, DestroyPipelineCache)
    VULKAN_GET_DEVICE_POINTER(device, GetPipelineCacheData)
    VULKAN_GET_DEVICE_POINTER(device, MergePipelineCaches)
    VULKAN_GET_DEVICE_POINTER(device, CreateGraphicsPipelines)
    VULKAN_GET_DEVICE_POINTER(device, CreateComputePipelines)
    VULKAN_GET_DEVICE_POINTER(device, DestroyPipeline)
    VULKAN_GET_DEVICE_POINTER(device, CreatePipelineLayout)
    VULKAN_GET_DEVICE_POINTER(device, DestroyPipelineLayout)
    VULKAN_GET_DEVICE_POINTER(device, CreateSampler)
    VULKAN_GET_DEVICE_POINTER(device, DestroySampler)
    VULKAN_GET_DEVICE_POINTER(device, CreateDescriptorSetLayout)
    VULKAN_GET_DEVICE_POINTER(device, DestroyDescriptorSetLayout)
    VULKAN_GET_DEVICE_POINTER(device, CreateDescriptorPool)
    VULKAN_GET_DEVICE_POINTER(device, DestroyDescriptorPool)
    VULKAN_GET_DEVICE_POINTER(device, ResetDescriptorPool)
    VULKAN_GET_DEVICE_POINTER(device, AllocateDescriptorSets)
    VULKAN_GET_DEVICE_POINTER(device, FreeDescriptorSets)
    VULKAN_GET_DEVICE_POINTER(device, UpdateDescriptorSets)
    VULKAN_GET_DEVICE_POINTER(device, CreateFramebuffer)
    VULKAN_GET_DEVICE_POINTER(device, DestroyFramebuffer)
    VULKAN_GET_DEVICE_POINTER(device, CreateRenderPass)
    VULKAN_GET_DEVICE_POINTER(device, DestroyRenderPass)
    VULKAN_GET_DEVICE_POINTER(device, GetRenderAreaGranularity)
    VULKAN_GET_DEVICE_POINTER(device, CreateCommandPool)
    VULKAN_GET_DEVICE_POINTER(device, DestroyCommandPool)
    VULKAN_GET_DEVICE_POINTER(device, ResetCommandPool)
    VULKAN_GET_DEVICE_POINTER(device, AllocateCommandBuffers)
    VULKAN_GET_DEVICE_POINTER(device, FreeCommandBuffers)
    VULKAN_GET_DEVICE_POINTER(device, BeginCommandBuffer)
    VULKAN_GET_DEVICE_POINTER(device, EndCommandBuffer)
    VULKAN_GET_DEVICE_POINTER(device, ResetCommandBuffer)
    VULKAN_GET_DEVICE_POINTER(device, CmdBindPipeline)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetViewport)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetScissor)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetLineWidth)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetDepthBias)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetBlendConstants)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetDepthBounds)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetStencilCompareMask)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetStencilWriteMask)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetStencilReference)
    VULKAN_GET_DEVICE_POINTER(device, CmdBindDescriptorSets)
    VULKAN_GET_DEVICE_POINTER(device, CmdBindIndexBuffer)
    VULKAN_GET_DEVICE_POINTER(device, CmdBindVertexBuffers)
    VULKAN_GET_DEVICE_POINTER(device, CmdDraw)
    VULKAN_GET_DEVICE_POINTER(device, CmdDrawIndexed)
    VULKAN_GET_DEVICE_POINTER(device, CmdDrawIndirect)
    VULKAN_GET_DEVICE_POINTER(device, CmdDrawIndexedIndirect)
    VULKAN_GET_DEVICE_POINTER(device, CmdDispatch)
    VULKAN_GET_DEVICE_POINTER(device, CmdDispatchIndirect)
    VULKAN_GET_DEVICE_POINTER(device, CmdCopyBuffer)
    VULKAN_GET_DEVICE_POINTER(device, CmdCopyImage)
    VULKAN_GET_DEVICE_POINTER(device, CmdBlitImage)
    VULKAN_GET_DEVICE_POINTER(device, CmdCopyBufferToImage)
    VULKAN_GET_DEVICE_POINTER(device, CmdCopyImageToBuffer)
    VULKAN_GET_DEVICE_POINTER(device, CmdUpdateBuffer)
    VULKAN_GET_DEVICE_POINTER(device, CmdFillBuffer)
    VULKAN_GET_DEVICE_POINTER(device, CmdClearColorImage)
    VULKAN_GET_DEVICE_POINTER(device, CmdClearDepthStencilImage)
    VULKAN_GET_DEVICE_POINTER(device, CmdClearAttachments)
    VULKAN_GET_DEVICE_POINTER(device, CmdResolveImage)
    VULKAN_GET_DEVICE_POINTER(device, CmdSetEvent)
    VULKAN_GET_DEVICE_POINTER(device, CmdResetEvent)
    VULKAN_GET_DEVICE_POINTER(device, CmdWaitEvents)
    VULKAN_GET_DEVICE_POINTER(device, CmdPipelineBarrier)
    VULKAN_GET_DEVICE_POINTER(device, CmdBeginQuery)
    VULKAN_GET_DEVICE_POINTER(device, CmdEndQuery)
    VULKAN_GET_DEVICE_POINTER(device, CmdResetQueryPool)
    VULKAN_GET_DEVICE_POINTER(device, CmdWriteTimestamp)
    VULKAN_GET_DEVICE_POINTER(device, CmdCopyQueryPoolResults)
    VULKAN_GET_DEVICE_POINTER(device, CmdPushConstants)
    VULKAN_GET_DEVICE_POINTER(device, CmdBeginRenderPass)
    VULKAN_GET_DEVICE_POINTER(device, CmdNextSubpass)
    VULKAN_GET_DEVICE_POINTER(device, CmdEndRenderPass)
    VULKAN_GET_DEVICE_POINTER(device, CmdExecuteCommands)

	return true;
}

#ifdef VK_USE_PLATFORM_WIN32_KHR
VKAPI_ATTR VkBool32 VKAPI_CALL CustomDebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode,
	const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	Log(false, "LAYER_VALIDATION: %s", pMessage);

	return VK_FALSE;
}
#endif

#ifdef VK_USE_PLATFORM_XLIB_KHR
VKAPI_ATTR VkBool32 VKAPI_CALL CustomDebugReportCallback(VkDebugReportFlagsEXT flags, VkDebugReportObjectTypeEXT objectType, uint64_t object, size_t location, int32_t messageCode,
	const char* pLayerPrefix, const char* pMessage, void* pUserData)
{
	LOGE("LAYER_VALIDATION: %s\n", pMessage);

	return VK_FALSE;
}
#endif
