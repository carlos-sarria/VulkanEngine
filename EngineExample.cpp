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

void EngineExample::initializeCamera()
{
    Camera *camera = &eng.appManager.defaultCamera;

    // Get the camera (the first one)
    if(eng.appManager.cameras.size()>0)
    {
        camera->transform = eng.appManager.cameras[0].transform;
        camera->from = eng.appManager.cameras[0].transform.translation;
        camera->to = VEC3(0.0f, 0.0f, 0.0f);
        camera->yfov = eng.appManager.cameras[0].yfov;
        camera->zfar = eng.appManager.cameras[0].zfar;
        camera->znear = eng.appManager.cameras[0].znear;
    }
    else
    {
        camera->transform.rotation = QUATERNION(0.7f,0.0f,0.0f,0.7f);
        camera->transform.translation = VEC3(0.0f,-30.0f,0.0f);
        camera->from = camera->transform.translation;
        camera->to = VEC3(0.0f, 0.0f, 0.0f);
        camera->yfov = 0.39959f;
        camera->zfar = 5000.0f;
        camera->znear = 0.01f;
    }
}

#define ROT_SPEED (0.05f*PI/180.0f)
#define MOV_SPEED 0.3f
void EngineExample::updateCamera(char keyPressed, const bool mousePressed, long mousePointX, long mousePointY)
{
    Camera *camera = &eng.appManager.defaultCamera;
    static VEC3 cameraPosition;
    static long mousePrevX, mousePrevY;
    static bool bFirstTime = true;
    static QUATERNION cameraRotation;

    if (bFirstTime || !mousePressed){
        mousePrevX = mousePointX;
        mousePrevY = mousePointY;
        if(bFirstTime) {
            initializeCamera();
            cameraPosition = camera->transform.translation;
            cameraRotation = camera->transform.rotation;
        }
        bFirstTime = false;
    }

    // Compose mouse movement with camera rotation
    VEC3 angle = cameraRotation.toEuler();
    angle.z += (float)(mousePointX-mousePrevX)*ROT_SPEED;
    angle.x += (float)(mousePointY-mousePrevY)*ROT_SPEED;
    cameraRotation = QUATERNION().fromEuler(angle);

    // Rotate the Blender LookAt vector (0,0,-1) using the Quaternion
    // up = cam.matrix_world.to_quaternion() * Vector((0.0, 1.0, 0.0))
    // cam_direction = cam.matrix_world.to_quaternion() * Vector((0.0, 0.0, -1.0))
    MATRIX mLookAt;
    mLookAt.rotationQ(cameraRotation);
    VEC3 vLookAt = mLookAt * VEC3(0.0f,0.0f,-1.0);

    // Move camera with keyboard
    float zoom = 0.0f, pan = 0.0f;
    if(keyPressed == 'W') zoom = -MOV_SPEED;
    if(keyPressed == 'S') zoom =  MOV_SPEED;
    if(keyPressed == 'A') pan  = -MOV_SPEED;
    if(keyPressed == 'D') pan  =  MOV_SPEED;
    if(zoom!=0.0f)
    {
        cameraPosition = cameraPosition - vLookAt * zoom;
    }
    if(pan!=0.0f)
    {
        VEC3 cross = vLookAt.crossProduct(VEC3(0.0f,0.0f,1.0f));
        cameraPosition = cameraPosition + cross * pan;
    }

    mousePrevX = mousePointX;
    mousePrevY = mousePointY;

    camera->from = cameraPosition;
    camera->to = cameraPosition + vLookAt;
}

//////////////////////////////////////////////////////////////////////////////
/// \brief EngineExample::updateUniformBuffers
/// \param idx
//////////////////////////////////////////////////////////////////////////////
void EngineExample::updateUniformBuffers(int idx)
{
    Camera camera = eng.appManager.defaultCamera;
    VEC3 lightDir;
    float yfov, zfar, znear;

    // Get the lights (first one only)
    if(eng.appManager.lights.size()>0)
    {
        lightDir = eng.appManager.lights[0].transform.translation; // Position as we use point light
    }
    else
    {
        lightDir.x  = 10.0f; lightDir.y  = 10.0f; lightDir.z  = 0.0f;
    }

    MATRIX mView, mProjection;
    mView.lookAtRH(camera.from, camera.to, VEC3(0.0f,0.0f,1.0f));

    float aspectRatio = eng.surfaceData.width / eng.surfaceData.height;
    bool isRotated = (eng.surfaceData.width < eng.surfaceData.height);

    mProjection.perspectiveFovRH(camera.yfov, aspectRatio, camera.znear, camera.zfar, isRotated);

    // Set the tarnsformation matrix for each mesh
    size_t minimumUboAlignment = static_cast<size_t>(eng.appManager.deviceProperties.limits.minUniformBufferOffsetAlignment);
    uint32_t bufferDataSize = static_cast<uint32_t>(_getAlignedDataSize(sizeof(UBO), minimumUboAlignment));
    uint32_t scene_offset = 0;

    for (Mesh mesh : eng.appManager.meshes)
    {
         MATRIX mModel, mMVP;
         mModel.scaling(mesh.transform.scale.x, mesh.transform.scale.y, mesh.transform.scale.z);
         mModel.rotationQ(mesh.transform.rotation);
         mModel.translation(mesh.transform.translation.x, mesh.transform.translation.y, mesh.transform.translation.z);
         //mModel.rotationZ(eng.appManager.angle); // FOR TESTING

         mMVP = mModel * mView * mProjection;

         UBO ubo;
         ubo.matrixMVP = mMVP;

         // Transform the light using the inverse model matrix. This will
         // allow to do smooth shading with just a dot product in the vertex shader
         mModel.inverse();
         VEC3 vOut, vIn = {lightDir.x, lightDir.y, lightDir.z};
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
void EngineExample::initialize(const char* appName, const char* gltfFile)
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
    eng.initCommandPoolAndBuffer();

    eng.loadGLTF(gltfFile);
    eng.initShaders(); // requires num meshes from gltf
    eng.initUniformBuffers();

    eng.initRenderPass();
    eng.initDescriptorPoolAndSet();
    eng.initFrameBuffers();
    eng.initPipeline();
    eng.initViewportAndScissor();
    eng.initSemaphoreAndFence();
    eng.recordCommandBuffer();
}
