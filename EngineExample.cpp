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

    //applyRotation(eng.appManager.frameId);
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

         ubo.lightDirection.x = 0.5f;
         ubo.lightDirection.y = 0.5f;
         ubo.lightDirection.z = 0.0f;

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


/// <summary>Calculate a rotation matrix which provides a rotation around the z axis using the given angle</summary>
/// <param name="angle">The angle through which the rotation will be applied</param>
/// <param name="outRotationMatrix">The output rotation matrix. This matrix must have dimensions 4x4</param>
void rotateAroundZ(float angle, std::array<std::array<float, 4>, 4>& outRotationMatrix)
{
	const float c = cos(angle);
	const float s = sin(angle);

	// Rotation around z axis (0, 0, 1)
	outRotationMatrix[0][0] = c;
    outRotationMatrix[0][1] = 0;
    outRotationMatrix[0][2] = -s;
	outRotationMatrix[0][3] = 0;

    outRotationMatrix[1][0] = 0;
    outRotationMatrix[1][1] = 1;
	outRotationMatrix[1][2] = 0;
	outRotationMatrix[1][3] = 0;

    outRotationMatrix[2][0] = s;
    outRotationMatrix[2][1] = 0;
    outRotationMatrix[2][2] = c;
	outRotationMatrix[2][3] = 0;

	outRotationMatrix[3][0] = 0;
	outRotationMatrix[3][1] = 0;
	outRotationMatrix[3][2] = 0;
	outRotationMatrix[3][3] = 1;
}

/// <summary>Multiple two matrices together.</summary>
/// <param name="first">The first matrix</param>
/// <param name="second">The second matrix</param>
/// <param name="outMatrix">The output matrix</param>
void multiplyMatrices(std::array<std::array<float, 4>, 4>& first, std::array<std::array<float, 4>, 4>& second, std::array<std::array<float, 4>, 4>& outMatrix)
{
	for (uint32_t i = 0; i < 4; i++)
	{
		for (uint32_t j = 0; j < 4; j++)
		{
            for (uint32_t k = 0; k < 4; k++) { outMatrix[i][j] += first[i][k] * second[k][j]; }
		}
	}
}

/// <summary>Updates the dynamic uniform buffer with the new rotation value</summary>
/// <param name="idx">Index which selects the correct area of the buffer</param>
void EngineExample::applyRotation(int idx)
{
	// This is called on every frame to update the dynamic uniform buffer with the new rotation
	// value.

	// An offset is used to point to the correct slice of the buffer that corresponds to the current
	// frame. The current frame is specified by the parameter, idx.
	// This memory is mapped persistently so it does not need to be mapped again on every frame. The pointer to this
    // consistently mapped memory is the variable eng.appManager.dynamicUniformBufferData.mappedData.

	// Calculate the offset.
    VkDeviceSize offset = (eng.appManager.offset * idx);

	// Update the angle of rotation and calculate the transformation matrix using the fixed projection
	// matrix and a freshly-calculated rotation matrix.
    eng.appManager.angle += 0.02f;

	auto rotation = std::array<std::array<float, 4>, 4>();
    rotateAroundZ(eng.appManager.angle, rotation);

    UBO ubo;
    auto mvp = std::array<std::array<float, 4>, 4>();
    multiplyMatrices(rotation, viewProj, mvp);

    ubo.matrixMVP.f[ 0] = mvp[0][0]; ubo.matrixMVP.f[ 1] = mvp[1][0]; ubo.matrixMVP.f[ 2] = mvp[2][0]; ubo.matrixMVP.f[ 3] = mvp[3][0];
    ubo.matrixMVP.f[ 4] = mvp[0][1]; ubo.matrixMVP.f[ 5] = mvp[1][1]; ubo.matrixMVP.f[ 6] = mvp[2][1]; ubo.matrixMVP.f[ 7] = mvp[3][1];
    ubo.matrixMVP.f[ 8] = mvp[0][2]; ubo.matrixMVP.f[ 9] = mvp[1][2]; ubo.matrixMVP.f[10] = mvp[2][2]; ubo.matrixMVP.f[11] = mvp[3][2];
    ubo.matrixMVP.f[12] = mvp[0][3]; ubo.matrixMVP.f[13] = mvp[1][3]; ubo.matrixMVP.f[14] = mvp[2][3]; ubo.matrixMVP.f[15] = mvp[3][3];

    ubo.lightDirection.x = 0.5f;
    ubo.lightDirection.y = 0.5f;
    ubo.lightDirection.z = 0.0f;

    size_t minimumUboAlignment = static_cast<size_t>(eng.appManager.deviceProperties.limits.minUniformBufferOffsetAlignment);
    uint32_t bufferDataSize = static_cast<uint32_t>(_getAlignedDataSize(sizeof(UBO), minimumUboAlignment));
    uint32_t scene_offset = 0;
    for (int i=0; i<eng.appManager.meshes.size(); i++)
    {
        // Copy the matrix to the mapped memory using the offset calculated above.
        memcpy(static_cast<unsigned char*>(eng.appManager.dynamicUniformBufferData.mappedData) + eng.appManager.dynamicUniformBufferData.bufferInfo.range * idx + scene_offset, &ubo, sizeof(UBO));
        scene_offset += bufferDataSize;
    }

	VkMappedMemoryRange mapMemRange = {
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		nullptr,
        eng.appManager.dynamicUniformBufferData.memory,
		offset,
        eng.appManager.dynamicUniformBufferData.bufferInfo.range,
	};

	// ONLY flush the memory if it does not support VK_MEMORY_PROPERTY_HOST_COHERENT_BIT.
    if ((eng.appManager.dynamicUniformBufferData.memPropFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) { vk::FlushMappedMemoryRanges(eng.appManager.device, 1, &mapMemRange); }
}

/// <summary>Initialises all Vulkan objects</summary>
void EngineExample::initialize(const char* appName)
{
	// All the Vulkan objects are initialised here.
	// The vk::initVulkan() function is used to load the Vulkan library and definitions.

    // eng.appManager holds all the object handles which need to be accessed "globally" such as the angle
	// of the rotation of the triangle that is going to be rendered on screen.
    eng.appManager.angle = 45.0f;

    eng.initialize(appName);

	float aspect = 0.0f;
	// The screen is rotated.
    if (eng.surfaceData.width < eng.surfaceData.height) { aspect = eng.surfaceData.height / eng.surfaceData.width; }
	else
	{
        aspect = eng.surfaceData.width / eng.surfaceData.height;
	}

	float left = aspect;
	float right = -aspect;
	float bottom = 1.0;
	float top = -1.0f;

	viewProj[0][0] = 2.0f / (right - left);
	viewProj[1][1] = 2.0f / (top - bottom);
	viewProj[2][2] = -1.0f;
	viewProj[3][0] = -(right + left) / (right - left);
	viewProj[3][1] = -(top + bottom) / (top - bottom);
	viewProj[3][3] = 1.0f;
}
