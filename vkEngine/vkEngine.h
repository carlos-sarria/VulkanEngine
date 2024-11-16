#ifndef VKENGINE_H
#define VKENGINE_H

#include "vkStructs.h"
#include "vkLayers.h"
#include "vkExtensions.h"
#include "vkDevice.h"
#include "vkQueues.h"
#include "vkSurfaces.h"
#include "vkMemory.h"
#include "vkTextures.h"
#include "vkShaders.h"
#include "vkGeometry.h"
#include "vkRenderPass.h"
#include "vkDescriptor.h"
#include "vkPipeline.h"
#include "vkFences.h"
#include "vkCommandBuffer.h"
#include "vkCloseDown.h"

class vkEngine
{
public:
    vkEngine() {}

    AppManager appManager;
    SurfaceData surfaceData;

    void closeDown() {
        _closeDown(appManager);
    }

    // Initialise the validation layers.
    std::vector<std::string> initLayers(){
        return _initLayers();
    }

    // Initialise the needed extensions.
    std::vector<std::string> initInstanceExtensions(){
        return _initInstanceExtensions();
    }
    std::vector<std::string> initDeviceExtensions(){
        return _initDeviceExtensions();
    }

    // Initialise the application and instance.
    void initApplicationAndInstance(const char* appName, std::vector<std::string>& extensionNames, std::vector<std::string>& layerNames){
        _initApplicationAndInstance(appManager, appName, extensionNames, layerNames);
    }

    // Fetch the physical devices and get a compatible one.
    void initPhysicalDevice(){
        _initPhysicalDevice(appManager);
    }

    // Find queues families and individual queues from device.
    void initQueuesFamilies(){
        _initQueuesFamilies(appManager);
    }

    // Initialise the logical device.
    void initLogicalDevice(std::vector<std::string>& deviceExtensions){
        _initLogicalDevice(appManager, deviceExtensions);
    }

    // Fetch queues from device.
    void initQueues(){
        _initQueues(appManager);
    }

    // Create the surface to be rendered on (platform-dependent).
    void initSurface(){
        _initSurface(appManager, surfaceData);
    }

    // Create the swapchain.
    void initSwapChain(){
        _initSwapChain(appManager, surfaceData);
    }

    // Create the images and image views to be used with the swapchain.
    void initImagesAndViews(){
        _initImagesAndViews(appManager);
    }

    // Create vertex buffers to draw the primitive.
    void loadGLTF(const char* fileName){
        _loadGLTF(appManager, fileName);
    }

    // Create a texture to apply to the primitive.
    void loadTexture(TextureData& texture){
        _loadTexture(appManager, texture);
    }

    // Create a descriptor pool and allocate descriptor sets for the buffers.
    void initDescriptorPoolAndSet(){
        _initDescriptorPoolAndSet(appManager);
    }

    // Compile and convert the shaders that will be used.
    void initShaders(){
        _initShaders(appManager);
    }

    // Create the pipeline to use for the rendering.
    void initPipeline(){
        _initPipeline(appManager);
    }

    // Create the render pass to use for rendering the triangle.
    void initRenderPass(){
        _initRenderPass(appManager);
    }

    // Create the frame buffers for rendering.
    void initFrameBuffers(){
        _initFrameBuffers(appManager);
    }

    // Create the command buffer to be sent to the GPU from the command pool.
    void initCommandPoolAndBuffer(){
        _initCommandPoolAndBuffer(appManager);
    }

    // Initialise the viewport and scissor for the rendering.
    void initViewportAndScissor(){
        _initViewportAndScissor(appManager, surfaceData);
    }

    // Create the semaphore to deal with the command queue.
    void initSemaphoreAndFence(){
        _initSemaphoreAndFence(appManager);
    }

    // Generic method to initialise buffers. Both vertex and uniform buffers use this.
    void createBuffer(BufferData& inBuffer, const uint8_t* inData, const VkBufferUsageFlags& inUsage){
        _createBuffer(appManager, inBuffer, inData, inUsage);
    }

    // Generic method for creating a shader module.
    void createShaderModule(const uint32_t* spvShader, size_t spvShaderSize, int indx, VkShaderStageFlagBits shaderStage){
        createShaderModule(spvShader, spvShaderSize, indx, shaderStage);
    }

    // Record the command buffer for rendering.
    void recordCommandBuffer(){
        _recordCommandBuffer(appManager);
    }

    void initUniformBuffers(){
        _initUniformBuffers(appManager);
    }

    uint32_t startCurrentBuffer(){
        _startCurrentBuffer(appManager);
    }

    uint32_t presentCurrentBuffer(){
        _presentCurrentBuffer(appManager);
    }

private:
    // This method checks for physical device compatibility.
    VkPhysicalDevice getCompatibleDevice(){
        return _getCompatibleDevice(appManager);
    }

    // Get a compatible queue family with the properties, and it needs to be a graphical one.
    void getCompatibleQueueFamilies(uint32_t& graphicsfamilyindex, uint32_t& presentfamilyindex){
        _getCompatibleQueueFamilies(appManager, graphicsfamilyindex, presentfamilyindex);
    }

    // Make sure the extent is correct, and if not, set the same sizes as the window.
    VkExtent2D getCorrectExtent(const VkSurfaceCapabilitiesKHR& inSurfCap){
        return _getCorrectExtent(appManager, surfaceData, inSurfCap);
    }

};
#endif // VKENGINE_H
