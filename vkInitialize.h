#ifndef VKINITIALIZE_H
#define VKINITIALIZE_H

#include "vkStructs.h"
#include "vkLayers.h"
#include "vkExtensions.h"
#include "vkDevice.h"
#include "vkQueues.h"
#include "vkSurfaces.h"
#include "vkTextures.h"
#include "vkShaders.h"
#include "vkGeometry.h"
#include "vkRenderPass.h"
#include "vkDescriptor.h"
#include "vkPipeline.h"
#include "vkFences.h"
#include "vkCommandBuffer.h"

inline void _initialize(AppManager& appManager, SurfaceData& surfaceData)
{
    // Initialise all the pointers to Vulkan functions.
    vk::initVulkan();

    std::vector<std::string> layers = _initLayers();
    std::vector<std::string> instanceExtensions = _initInstanceExtensions();

    _initApplicationAndInstance(appManager, instanceExtensions, layers);
    _initPhysicalDevice(appManager);

    _initSurface(appManager, surfaceData);

    _initQueuesFamilies(appManager);

    std::vector<std::string> deviceExtensions = _initDeviceExtensions();
    _initLogicalDevice(appManager, deviceExtensions);
    _initQueues(appManager);
    _initSwapChain(appManager, surfaceData);
    _initImagesAndViews(appManager);
    _initCommandPoolAndBuffer(appManager);

    _initShaders(appManager);
    _initVertexBuffers(appManager);
    _initUniformBuffers(appManager);
    _initRenderPass(appManager);
    _loadTexture(appManager);
    _initDescriptorPoolAndSet(appManager);

    _initFrameBuffers(appManager);
    _initPipeline(appManager);

    _initViewportAndScissor(appManager, surfaceData);
    _initSemaphoreAndFence(appManager);

    _recordCommandBuffer(appManager);
}

#endif // VKINITIALIZE_H
