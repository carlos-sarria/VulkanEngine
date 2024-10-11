/*!*********************************************************************************************************************
\File         EngineExample.cpp
\Title        Vulkan HelloAPI
\Author       PowerVR by Imagination, Developer Technology Team.
\Copyright    Copyright(c) Imagination Technologies Limited.
\brief        Build an introductory Vulkan application to show the process of getting started with Vulkan.
***********************************************************************************************************************/
#include "EngineExample.h"

/// <summary>Executes the recorded command buffers. The recorded operations will end up rendering and presenting the frame to the surface</summary>
void EngineExample::drawFrame()
{
    eng.startCurrentBuffer();

    updateUniformBuffers(eng.appManager.frameId);

    eng.presentCurrentBuffer();
}

VEC3 EngineExample::getDirection(Transform transform)
{
    VEC3 retOut;
    MATRIX	m;
    VEC4 vOut, vDir = { 0.0f, 0.0f, -1.0f, 0.0f};

    m.matrixRotationQ(transform.rotation);
    vOut = m.vectorMultiply(vDir);

    retOut.x = vOut.x; retOut.y = vOut.y; retOut.z = vOut.z;
    return retOut;
}

void EngineExample::updateUniformBuffers(int idx)
{
    VEC3 cameraPos, cameraTo;
    VEC3 LightDir;

    VEC3 cameraDir;
    // Get the camera (the first one)
    if(eng.appManager.cameras.size()>0)
    {
        cameraPos = eng.appManager.cameras[0].transform.translation;
        cameraDir = getDirection(eng.appManager.cameras[0].transform);
        cameraTo.x = cameraPos.x + cameraDir.x;
        cameraTo.y = cameraPos.y + cameraDir.y;
        cameraTo.z = cameraPos.z + cameraDir.z;
    }
    else
    {
        cameraPos.x = 0.0f; cameraPos.y = 0.0f; cameraPos.z = 10.0f;
        cameraTo.x  = 0.0f; cameraTo.y  = 0.0f; cameraTo.z  = 9.0f;
    }

    // Get the lights (first one only)
    if(eng.appManager.lights.size()>0)
    {
        LightDir = eng.appManager.cameras[0].transform.translation;
        //LightDir.x  = 10.0f*cos(eng.appManager.angle); LightDir.y  = 10.0f*sin(eng.appManager.angle); cameraTo.z  = 0.0f;
    }
    else
    {
        LightDir.x  = 10.0f; LightDir.y  = 10.0f; cameraTo.z  = 0.0f;
    }

    MATRIX mProjection, mView;
    VEC3 vUp = {0.00001f,0.000001f, 1.0f}; // TODO: FIXME if x=y=0.0f and camera x=y=0.0f, the cross product will make the vector 0,0,0
    mView.matrixLookAtRH(cameraPos, cameraTo, vUp);

    float aspectRatio = eng.surfaceData.width / eng.surfaceData.height;
    bool isRotated = (eng.surfaceData.width > eng.surfaceData.height);
    mProjection.matrixPerspectiveFovRH(0.78539819f, aspectRatio,  2.0f, 5000.0f, isRotated);

    // Set the tarnsformation matrix for each mesh
    size_t minimumUboAlignment = static_cast<size_t>(eng.appManager.deviceProperties.limits.minUniformBufferOffsetAlignment);
    uint32_t bufferDataSize = static_cast<uint32_t>(_getAlignedDataSize(sizeof(UBO), minimumUboAlignment));
    uint32_t scene_offset = 0;

    for (Mesh mesh : eng.appManager.meshes)
    {
         MATRIX	mRot, mRotX, mTrans, mScale;
         mRot.matrixRotationQ(mesh.transform.rotation);
         mRotX.matrixRotationZ(0.0f);//eng.appManager.angle);
         mTrans.matrixTranslation(mesh.transform.translation.x, mesh.transform.translation.y, mesh.transform.translation.z);
         mScale.matrixScaling(mesh.transform.scale.x, mesh.transform.scale.y, mesh.transform.scale.z);

         eng.appManager.angle += 0.002f;

         // ModelmView (Model = S*R*T)
         MATRIX mModel, mMVP;
         mModel = mScale * mRot * mTrans * mRotX;
         mMVP = mModel * mView * mProjection;

         UBO ubo;
         ubo.matrixMVP = mMVP;

         MATRIX mInv;
         mInv.matrixInverse(mModel);
         VEC4 vOut, vIn = {LightDir.x, LightDir.y, LightDir.z, 0.0f};
         vOut = mInv * vIn;
         ubo.lightDirection.x =  vOut.x;
         ubo.lightDirection.y =  vOut.y;
         ubo.lightDirection.z =  vOut.z;

         // Copy the matrix to the mapped memory using the offset calculated above.
         memcpy(static_cast<unsigned char*>(eng.appManager.dynamicUniformBufferData.mappedData) + eng.appManager.dynamicUniformBufferData.bufferInfo.range * idx + scene_offset, &ubo, sizeof(UBO));
         scene_offset += bufferDataSize;
    }

    VkMappedMemoryRange mapMemRange = {
        VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
        nullptr,
        eng.appManager.dynamicUniformBufferData.memory,
        eng.appManager.offset * idx,
        eng.appManager.dynamicUniformBufferData.bufferInfo.range,
    };

    // ONLY flush the memory if it does not support VK_MEMORY_PROPERTY_HOST_COHERENT_BIT.
    if ((eng.appManager.dynamicUniformBufferData.memPropFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) { vk::FlushMappedMemoryRanges(eng.appManager.device, 1, &mapMemRange); }
}
///////////////////////////////////////////////////////
/// <summary>Initialises all Vulkan objects</summary>
/// ///////////////////////////////////////////////////
void EngineExample::initialize(const char* appName)
{
    // Initialise all the pointers to Vulkan functions.
    vk::initVulkan();

    std::vector<std::string> layers = eng.initLayers();
    std::vector<std::string> instanceExtensions = eng.initInstanceExtensions();

    eng.initApplicationAndInstance(appName, instanceExtensions, layers);
    eng.initPhysicalDevice();
    eng.initSurface();
    eng.initQueuesFamilies();
    std::vector<std::string> deviceExtensions = eng.initDeviceExtensions();
    eng.initLogicalDevice(deviceExtensions);
    eng.initQueues();
    eng.initSwapChain();
    eng.initImagesAndViews();
    eng.loadGLTF();
    eng.initCommandPoolAndBuffer();
    eng.initShaders();
    eng.initUniformBuffers();
    eng.initRenderPass();
    eng.loadTexture(eng.appManager.texture);
    eng.initDescriptorPoolAndSet();
    eng.initFrameBuffers();
    eng.initPipeline();
    eng.initViewportAndScissor();
    eng.initSemaphoreAndFence();
    eng.recordCommandBuffer();
}
