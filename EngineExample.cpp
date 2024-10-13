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

void EngineExample::updateCamera(char keyPressed, const long mousePointX, const long mousePointY)
{
    Camera *camera = &eng.appManager.defaultCamera;
    static VEC3 pos = {0.0f, 0.0f,10.0f};
    static VEC3 to = {0.0f, 0.0f,9.0f};

    if(keyPressed == 'W') pos.z += 0.1f;
    if(keyPressed == 'S') pos.z -= 0.1f;

    camera->from.x = pos.x; camera->from.y = pos.y; camera->from.z = pos.z;
    camera->to.x = to.x; camera->to.y = to.y; camera->to.z = to.z;
    camera->yfov = 0.39959f;
    camera->zfar = 1000.0f;
    camera->znear = 0.1f;

    // float radHalfAngle = ... / 2.0; //See below
    // float sinVal = Math.Sin(radHalfAngle);
    // float cosVal = Math.Cos(radHalfAngle);
    // float xVal = 1.0f * sinVal;
    // float yVal = 0.0f * sinVal;  //Here for completeness.
    // float zVal = 0.0f * sinVal;  //Here for completeness.
    // Quaternion rot = new Quaternion(xVal, yVal, zVal, cosVal);
}


VEC3 EngineExample::getDirection(Transform transform, VEC3 vUp)
{
    VEC3 retOut;
    MATRIX	m;
    VEC4 vOut, vUp4 = {vUp.x,vUp.y,vUp.z,0.0f};

    m.matrixRotationQ(transform.rotation);
    vOut = m * vUp4;

    retOut.x = vOut.x; retOut.y = vOut.y; retOut.z = vOut.z;
    return retOut;
}

void EngineExample::updateUniformBuffers(int idx)
{
    Camera camera = eng.appManager.defaultCamera;
    VEC3 cameraPos, cameraTo;
    VEC3 LightDir;
    VEC3 vUp = {0.0f,0.0f,-1.0f};

    VEC3 cameraDir;
    float yfov, zfar, znear;

    // Get the camera (the first one)
    if(eng.appManager.cameras.size()>0)
    {
        cameraPos = eng.appManager.cameras[0].transform.translation;
        cameraDir = getDirection(eng.appManager.cameras[0].transform, vUp);
        cameraTo.x = cameraPos.x + cameraDir.x;
        cameraTo.y = cameraPos.y + cameraDir.y;
        cameraTo.z = cameraPos.z + cameraDir.z;
        yfov = eng.appManager.cameras[0].yfov;
        zfar = eng.appManager.cameras[0].zfar;
        znear = eng.appManager.cameras[0].znear;
    }
    else
    {
        cameraPos.x = 0.0f; cameraPos.y = 0.0f; cameraPos.z = 10.0f;
        cameraTo.x  = 0.0f; cameraTo.y  = 0.0f; cameraTo.z  = 9.0f;
        yfov = 0.39959f;
        zfar = 100.0f;
        znear = 0.1f;
    }

    // Get the lights (first one only)
    if(eng.appManager.lights.size()>0)
    {
        LightDir = eng.appManager.lights[0].transform.translation; // Position as we use point light
        //LightDir = getDirection(eng.appManager.lights[0].transform, LightDir);
        //LightDir.x  = 10.0f*cos(eng.appManager.angle); LightDir.y  = 10.0f*sin(eng.appManager.angle); cameraTo.z  = 0.0f;
    }
    else
    {
        LightDir.x  = 10.0f; LightDir.y  = 10.0f; LightDir.z  = 0.0f;
    }

    // Blender:
    // up = cam.matrix_world.to_quaternion() * Vector((0.0, 1.0, 0.0))
    // cam_direction = cam.matrix_world.to_quaternion() * Vector((0.0, 0.0, -1.0))
    MATRIX mView, mProjection;
    // vUp.x = 0.0f; vUp.y = 1.0f; vUp.z = 0.0f;
    // VEC3 camRoll = getDirection(eng.appManager.cameras[0].transform, vUp);
    mView.matrixLookAtRH(camera.from, camera.to, vUp);

    float aspectRatio = eng.surfaceData.width / eng.surfaceData.height;
    bool isRotated = (eng.surfaceData.width < eng.surfaceData.height);

    mProjection.matrixPerspectiveFovRH(camera.yfov, aspectRatio, camera.znear, camera.zfar, isRotated);

    // Set the tarnsformation matrix for each mesh
    size_t minimumUboAlignment = static_cast<size_t>(eng.appManager.deviceProperties.limits.minUniformBufferOffsetAlignment);
    uint32_t bufferDataSize = static_cast<uint32_t>(_getAlignedDataSize(sizeof(UBO), minimumUboAlignment));
    uint32_t scene_offset = 0;

    for (Mesh mesh : eng.appManager.meshes)
    {
         MATRIX mModel, mMVP;
         mModel.matrixScaling(mesh.transform.scale.x, mesh.transform.scale.y, mesh.transform.scale.z);
         mModel.matrixRotationQ(mesh.transform.rotation);
         mModel.matrixTranslation(mesh.transform.translation.x, mesh.transform.translation.y, mesh.transform.translation.z);
         mModel.matrixRotationZ(eng.appManager.angle);

         mMVP = mModel * mView * mProjection;

         UBO ubo;
         ubo.matrixMVP = mMVP;

         // Transform the light using the inverse model matrix. This will
         // allow to do smooth shading with just a dot product in the vertex shader
         mModel.matrixInverse();
         VEC4 vOut, vIn = {LightDir.x, LightDir.y, LightDir.z, 0.0f};
         vOut = mModel * vIn;
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

    eng.appManager.angle += 0.02f;
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
    eng.loadGLTF("..\\..\\suzanne.glb");
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
