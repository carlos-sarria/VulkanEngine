/*!*********************************************************************************************************************
\File         VulkanHelloAPI.cpp
\Title        Vulkan HelloAPI
\Author       PowerVR by Imagination, Developer Technology Team.
\Copyright    Copyright(c) Imagination Technologies Limited.
\brief        Build an introductory Vulkan application to show the process of getting started with Vulkan.
***********************************************************************************************************************/
#include "VulkanEngine.h"

/// <summary>Executes the recorded command buffers. The recorded operations will end up rendering and presenting the frame to the surface</summary>
void VulkanHelloAPI::drawFrame()
{
	// This is where the recorded command buffers are executed. The recorded operations will end up rendering
	// and presenting the frame to the surface.

	// currentBuffer will be used to point to the correct frame/command buffer/uniform buffer data.
	// It is going to be the general index of the data being worked on.
	uint32_t currentBuffer = 0;
	VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	// Acquire and get the index of the next available swapchain image.
	debugAssertFunctionResult(
        vk::AcquireNextImageKHR(eng.appManager.device, eng.appManager.swapchain, std::numeric_limits<uint64_t>::max(), eng.appManager.acquireSemaphore[frameId], VK_NULL_HANDLE, &currentBuffer),
		"Draw - Acquire Image");

	// Wait for the fence to be signalled before starting to render the current frame, then reset it so it can be reused.
    debugAssertFunctionResult(vk::WaitForFences(eng.appManager.device, 1, &eng.appManager.frameFences[currentBuffer], true, FENCE_TIMEOUT), "Fence - Signalled");

    vk::ResetFences(eng.appManager.device, 1, &eng.appManager.frameFences[currentBuffer]);

	// Use a helper function with the current frame index to calculate the transformation matrix and write it into the correct
	// slice of the uniform buffer.
	applyRotation(currentBuffer);

	// Submit the command buffer to the queue to start rendering.
	// The command buffer is submitted to the graphics queue which was created earlier.
	// Notice the wait (acquire) and signal (present) semaphores, and the fence.
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.pWaitDstStageMask = &pipe_stage_flags;
	submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = &eng.appManager.acquireSemaphore[frameId];
	submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = &eng.appManager.presentSemaphores[frameId];
	submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &eng.appManager.cmdBuffers[currentBuffer];

    debugAssertFunctionResult(vk::QueueSubmit(eng.appManager.graphicQueue, 1, &submitInfo, eng.appManager.frameFences[currentBuffer]), "Draw - Submit to Graphic Queue");

	// Queue the rendered image for presentation to the surface.
	// The currentBuffer is again used to select the correct swapchain images to present. A wait
	// semaphore is also set here which will be signalled when the command buffer has
	// finished execution.
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = &eng.appManager.swapchain;
	presentInfo.pImageIndices = &currentBuffer;
    presentInfo.pWaitSemaphores = &eng.appManager.presentSemaphores[frameId];
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pResults = nullptr;

    debugAssertFunctionResult(vk::QueuePresentKHR(eng.appManager.presentQueue, &presentInfo), "Draw - Submit to Present Queue");

	// Update the frameId to get the next suitable one.
    frameId = (frameId + 1) % eng.appManager.swapChainImages.size();
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
	outRotationMatrix[0][1] = s;
	outRotationMatrix[0][2] = 0;
	outRotationMatrix[0][3] = 0;

	outRotationMatrix[1][0] = -s;
	outRotationMatrix[1][1] = c;
	outRotationMatrix[1][2] = 0;
	outRotationMatrix[1][3] = 0;

	outRotationMatrix[2][0] = 0;
	outRotationMatrix[2][1] = 0;
	outRotationMatrix[2][2] = 1;
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
void VulkanHelloAPI::applyRotation(int idx)
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

	auto mvp = std::array<std::array<float, 4>, 4>();
	multiplyMatrices(rotation, viewProj, mvp);

	// Copy the matrix to the mapped memory using the offset calculated above.
    memcpy(static_cast<unsigned char*>(eng.appManager.dynamicUniformBufferData.mappedData) + eng.appManager.dynamicUniformBufferData.bufferInfo.range * idx, &mvp, sizeof(mvp));

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
void VulkanHelloAPI::initialize()
{
	// All the Vulkan objects are initialised here.
	// The vk::initVulkan() function is used to load the Vulkan library and definitions.

	// frameId is the index that will be used for synchronisation. It is going to be used mostly by
	// fences and semaphores to keep track of which one is currently free to work on.
	frameId = 0;

    // eng.appManager holds all the object handles which need to be accessed "globally" such as the angle
	// of the rotation of the triangle that is going to be rendered on screen.
    eng.appManager.angle = 45.0f;

    eng.initialize();

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
