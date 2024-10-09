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

    updateUniforms(eng.appManager.frameId);

    eng.presentCurrentBuffer();
}

VEC3 EngineExample::getDirection(Transform transform)
{
    VEC3 retOut;
    MATRIX	m;
    VEC4 vOut, vDir = { 0.0f, 0.0f, -1.0f, 0.0f};

    _matrixRotationQ(m, transform.rotation);
    _vectorMultiply(vOut, vDir, m);

    retOut.x = vOut.x; retOut.y = vOut.y; retOut.z = vOut.z;
    return retOut;
}

void EngineExample::updateUniforms(int idx)
{
    VEC3 cameraPos, cameraTo;
    VEC3 LightDir;
    MATRIX modelMatrix;
    MATRIX viewMatrix;
    MATRIX projectionMatrix;

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
        LightDir = getDirection(eng.appManager.lights[0].transform);
    }
    else
    {
        LightDir.x  = 0.0f; LightDir.y  = 0.0f; cameraTo.z  = 1.0f;
    }

    MATRIX mProjection, mView;
    VEC3 vUp = {0.00001f,0.000001f,-1.0f}; // TODO: FIXME if x=y=0.0f and camera x=y=0.0f, the cross product will make the vector 0,0,0
    _matrixLookAtRH(mView, cameraPos, cameraTo, vUp);

    float aspectRatio = eng.surfaceData.width / eng.surfaceData.height;
    bool isRotated = (eng.surfaceData.width > eng.surfaceData.height);
    _matrixPerspectiveFovRH(mProjection, 0.78539819f, aspectRatio,  2.0f, 5000.0f, isRotated);

    // Set the tarnsformation matrix for each mesh
    size_t minimumUboAlignment = static_cast<size_t>(eng.appManager.deviceProperties.limits.minUniformBufferOffsetAlignment);
    uint32_t bufferDataSize = static_cast<uint32_t>(_getAlignedDataSize(sizeof(UBO), minimumUboAlignment));
    uint32_t scene_offset = 0;

    for (Mesh mesh : eng.appManager.meshes)
    {
         MATRIX	mRot, mRotX, mTrans, mScale;
         _matrixRotationQ(mRot, mesh.transform.rotation);
         _matrixRotationY(mRotX, eng.appManager.angle);
         _matrixTranslation(mTrans, mesh.transform.translation.x, mesh.transform.translation.y, mesh.transform.translation.z);
         _matrixScaling(mScale, mesh.transform.scale.x, mesh.transform.scale.y, mesh.transform.scale.z);

         eng.appManager.angle += 0.02f;

         // ModelViewMatrix (Model = S*R*T)
         MATRIX matrixMVP;
         _matrixIdentity(matrixMVP);
         _matrixMultiply(matrixMVP, matrixMVP, mScale);
         _matrixMultiply(matrixMVP, matrixMVP, mRot);
         _matrixMultiply(matrixMVP, matrixMVP, mRotX);
         _matrixMultiply(matrixMVP, matrixMVP, mTrans);
         _matrixMultiply(matrixMVP, matrixMVP, mView);
         _matrixMultiply(matrixMVP, matrixMVP, mProjection);

         UBO ubo;
         ubo.matrixMVP = matrixMVP;

         MATRIX matrixL, matrixInv;
         _matrixIdentity(matrixL);
         _matrixMultiply(matrixL, matrixMVP, mRot);
         _matrixMultiply(matrixL, matrixMVP, mRotX);
         _matrixInverse(matrixInv, matrixL);
         VEC4 vOut, vIn = {LightDir.x, LightDir.y, LightDir.z, 1.0f};
         _vectorMultiply(vOut, vIn, matrixInv);
         ubo.lightDirection.x = vOut.x;
         ubo.lightDirection.y = vOut.y;
         ubo.lightDirection.z = vOut.z;

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
/// <summary>Initialises all Vulkan objects</summary>
void EngineExample::initialize(const char* appName)
{
    eng.initialize(appName);
}
