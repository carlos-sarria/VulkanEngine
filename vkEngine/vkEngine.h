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
#include "vkInitialize.h"
#include "vkCloseDown.h"
#include "vkMath.h"

class vkEngine
{
public:
    vkEngine() {}

    AppManager appManager;
    SurfaceData surfaceData;

    void initialize(const char* appName) {
        _initialize(appManager,surfaceData, appName);
    }

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
    void initVertexBuffers(){
        _initVertexBuffers(appManager);
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

    // Matrix operations
    void matrixIdentity(MATRIX &mOut){
        _matrixIdentity(mOut);
    }

    void matrixMultiply(MATRIX &mOut,const MATRIX &mA,const MATRIX &mB){
        _matrixMultiply(mOut,mA,mB);
    }

    void matrixTranslation(MATRIX &mOut,const float fX,const float fY,const float fZ){
        _matrixTranslation(mOut,fX,fY,fZ);
    }

    void matrixScaling(MATRIX &mOut,const float fX,const float fY,const float fZ){
        _matrixScaling(mOut,fX,fY,fZ);
    }

    void matrixRotationX(MATRIX &mOut,const float fAngle){
        _matrixRotationX(mOut,fAngle);
    }

    void matrixRotationY(MATRIX &mOut,const float fAngle){
        _matrixRotationY(mOut, fAngle);
    }

    void matrixRotationZ(MATRIX &mOut,const float fAngle){
        _matrixRotationZ(mOut, fAngle);
    }

    inline void matrixRotationQ(MATRIX &mOut, VEC4 quaternion){
        _matrixRotationQ(mOut, quaternion);
    }

    void matrixTranspose(MATRIX &mOut,const MATRIX &mIn){
        _matrixTranspose(mOut,mIn);
    }

    void matrixInverse(MATRIX &mOut,const MATRIX &mIn){
        _matrixInverse(mOut,mIn);
    }

    void matrixInverseEx(MATRIX &mOut,const MATRIX &mIn){
        _matrixInverseEx(mOut,mIn);
    }

    void matrixLookAtLH(MATRIX &mOut,const VEC3 &vEye,const VEC3 &vAt,const VEC3 &vUp){
        _matrixLookAtLH(mOut,vEye,vAt,vUp);
    }

    void matrixLookAtRH(MATRIX &mOut,const VEC3 &vEye,const VEC3 &vAt,const VEC3 &vUp){
        _matrixLookAtRH(mOut,vEye,vAt,vUp);
    }

    void matrixPerspectiveFovLH(MATRIX &mOut,const float fFOVy,const float fAspect,const float fNear,const float fFar,const bool bRotate = false){
        _matrixPerspectiveFovLH(mOut,fFOVy,fAspect,fNear,fFar,bRotate);
    }

    void matrixPerspectiveFovRH(MATRIX &mOut,const float fFOVy,const float fAspect,const float fNear,const float fFar,const bool bRotate = false){
        _matrixPerspectiveFovRH(mOut,fFOVy,fAspect,fNear,fFar,bRotate);
    }

    void matrixOrthoLH(MATRIX &mOut,const float w,const float h,const float zn,const float zf,const bool bRotate = false){
        _matrixOrthoLH(mOut,w,h,zn,zf,bRotate);
    }

    void matrixOrthoRH(MATRIX &mOut,const float w,const float h,const float zn,const float zf,const bool bRotate = false){
        _matrixOrthoRH(mOut,w,h,zn,zf,bRotate);
    }

    void matrixVec3Lerp(VEC3 &vOut,const VEC3 &v1,const VEC3 &v2,const float s){
        _matrixLerp(vOut,v1,v2,s);
    }

    float matrixVec3DotProduct(const VEC3 &v1,const VEC3 &v2){
        return _matrixDotProduct(v1,v2);
    }

    void matrixVec3CrossProduct(VEC3 &vOut,const VEC3 &v1,const VEC3 &v2){
        _matrixCrossProduct(vOut,v1,v2);
    }

    void matrixVec3Normalize(VEC3 &vOut,const VEC3 &vIn){
        _matrixNormalize(vOut,vIn);
    }

    float matrixVec3Length(const VEC3 &vIn){
        return _matrixLength(vIn);
    }

    void matrixLinearEqSolve(float * const pRes,float ** const pSrc,const int nCnt){
        _matrixLinearEqSolve(pRes,pSrc,nCnt);
    }

    void vectorMultiply(VEC4 &mOut, const VEC4 &vA, const MATRIX &mB){
        _vectorMultiply(mOut,vA,mB);
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
