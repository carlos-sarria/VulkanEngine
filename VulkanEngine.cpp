/*!*********************************************************************************************************************
\File         VulkanHelloAPI.cpp
\Title        Vulkan HelloAPI
\Author       PowerVR by Imagination, Developer Technology Team.
\Copyright    Copyright(c) Imagination Technologies Limited.
\brief        Build an introductory Vulkan application to show the process of getting started with Vulkan.
***********************************************************************************************************************/
#include "VulkanEngine.h"

#include "vkLayers.h"
#include "vkExtensions.h"
#include "vkInit.h"
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
		vk::AcquireNextImageKHR(appManager.device, appManager.swapchain, std::numeric_limits<uint64_t>::max(), appManager.acquireSemaphore[frameId], VK_NULL_HANDLE, &currentBuffer),
		"Draw - Acquire Image");

	// Wait for the fence to be signalled before starting to render the current frame, then reset it so it can be reused.
	debugAssertFunctionResult(vk::WaitForFences(appManager.device, 1, &appManager.frameFences[currentBuffer], true, FENCE_TIMEOUT), "Fence - Signalled");

	vk::ResetFences(appManager.device, 1, &appManager.frameFences[currentBuffer]);

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
	submitInfo.pWaitSemaphores = &appManager.acquireSemaphore[frameId];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &appManager.presentSemaphores[frameId];
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &appManager.cmdBuffers[currentBuffer];

	debugAssertFunctionResult(vk::QueueSubmit(appManager.graphicQueue, 1, &submitInfo, appManager.frameFences[currentBuffer]), "Draw - Submit to Graphic Queue");

	// Queue the rendered image for presentation to the surface.
	// The currentBuffer is again used to select the correct swapchain images to present. A wait
	// semaphore is also set here which will be signalled when the command buffer has
	// finished execution.
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &appManager.swapchain;
	presentInfo.pImageIndices = &currentBuffer;
	presentInfo.pWaitSemaphores = &appManager.presentSemaphores[frameId];
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pResults = nullptr;

	debugAssertFunctionResult(vk::QueuePresentKHR(appManager.presentQueue, &presentInfo), "Draw - Submit to Present Queue");

	// Update the frameId to get the next suitable one.
	frameId = (frameId + 1) % appManager.swapChainImages.size();
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
	// consistently mapped memory is the variable appManager.dynamicUniformBufferData.mappedData.

	// Calculate the offset.
	VkDeviceSize offset = (appManager.offset * idx);

	// Update the angle of rotation and calculate the transformation matrix using the fixed projection
	// matrix and a freshly-calculated rotation matrix.
	appManager.angle += 0.02f;

	auto rotation = std::array<std::array<float, 4>, 4>();
	rotateAroundZ(appManager.angle, rotation);

	auto mvp = std::array<std::array<float, 4>, 4>();
	multiplyMatrices(rotation, viewProj, mvp);

	// Copy the matrix to the mapped memory using the offset calculated above.
	memcpy(static_cast<unsigned char*>(appManager.dynamicUniformBufferData.mappedData) + appManager.dynamicUniformBufferData.bufferInfo.range * idx, &mvp, sizeof(mvp));

	VkMappedMemoryRange mapMemRange = {
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		nullptr,
		appManager.dynamicUniformBufferData.memory,
		offset,
		appManager.dynamicUniformBufferData.bufferInfo.range,
	};

	// ONLY flush the memory if it does not support VK_MEMORY_PROPERTY_HOST_COHERENT_BIT.
	if ((appManager.dynamicUniformBufferData.memPropFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) { vk::FlushMappedMemoryRanges(appManager.device, 1, &mapMemRange); }
}

/// <summary>Initialises all Vulkan objects</summary>
void VulkanHelloAPI::initialize()
{
	// All the Vulkan objects are initialised here.
	// The vk::initVulkan() function is used to load the Vulkan library and definitions.

	// frameId is the index that will be used for synchronisation. It is going to be used mostly by
	// fences and semaphores to keep track of which one is currently free to work on.
	frameId = 0;

	// appManager holds all the object handles which need to be accessed "globally" such as the angle
	// of the rotation of the triangle that is going to be rendered on screen.
	appManager.angle = 45.0f;

	// Initialise all the pointers to Vulkan functions.
	vk::initVulkan();

	// Initialise all the Vulkan objects required to begin rendering.
	std::vector<std::string> layers = initLayers();
    std::vector<std::string> instanceExtensions = initInstanceExtensions();

    initApplicationAndInstance(appManager, instanceExtensions, layers);
    initPhysicalDevice(appManager);

    initSurface(appManager, surfaceData);

    initQueuesFamilies(appManager);

	std::vector<std::string> deviceExtensions = initDeviceExtensions();

    initLogicalDevice(appManager, deviceExtensions);
    initQueues(appManager);
    initSwapChain(appManager, surfaceData);
    initImagesAndViews(appManager);
    initCommandPoolAndBuffer(appManager);

    initShaders(appManager);
    initVertexBuffers(appManager);
    initUniformBuffers(appManager);
    initRenderPass(appManager);
    loadTexture(appManager);
    initDescriptorPoolAndSet(appManager);

    initFrameBuffers(appManager);
    initPipeline(appManager);

    initViewportAndScissor(appManager, surfaceData);
    initSemaphoreAndFence(appManager);

    recordCommandBuffer(appManager);

	float aspect = 0.0f;
	// The screen is rotated.
	if (surfaceData.width < surfaceData.height) { aspect = surfaceData.height / surfaceData.width; }
	else
	{
		aspect = surfaceData.width / surfaceData.height;
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

/// <summary>Ensures all created objects are cleaned up correctly and allocated memory is freed</summary>
void VulkanHelloAPI::deinitialize()
{
	// This function ensures that all the objects that were created are cleaned up correctly and nothing
	// is left "open" when the application is closed.

	// Wait for the device to have finished all operations before starting the clean up.
	debugAssertFunctionResult(vk::DeviceWaitIdle(appManager.device), "Device Wait for Idle");

	// Destroy the fence used to sync work between the CPU and GPU.
	vk::WaitForFences(appManager.device, static_cast<uint32_t>(appManager.frameFences.size()), appManager.frameFences.data(), true, uint64_t(-1));
	vk::ResetFences(appManager.device, static_cast<uint32_t>(appManager.frameFences.size()), appManager.frameFences.data());
	for (auto& fence : appManager.frameFences) { vk::DestroyFence(appManager.device, fence, nullptr); }

	// Destroy the semaphores used for image acquisition and rendering.
	for (auto& semaphore : appManager.acquireSemaphore) { vk::DestroySemaphore(appManager.device, semaphore, nullptr); }

	for (auto& semaphore : appManager.presentSemaphores) { vk::DestroySemaphore(appManager.device, semaphore, nullptr); }

	// Free the memory allocated for the descriptor sets.
	vk::FreeDescriptorSets(appManager.device, appManager.descriptorPool, 1, &appManager.staticDescSet);
	vk::FreeDescriptorSets(appManager.device, appManager.descriptorPool, 1, &appManager.dynamicDescSet);

	// Destroy both the descriptor layouts and descriptor pool.
	vk::DestroyDescriptorSetLayout(appManager.device, appManager.staticDescriptorSetLayout, nullptr);
	vk::DestroyDescriptorSetLayout(appManager.device, appManager.dynamicDescriptorSetLayout, nullptr);
	vk::DestroyDescriptorPool(appManager.device, appManager.descriptorPool, nullptr);

	// Destroy the uniform buffer and free the memory.
	vk::DestroyBuffer(appManager.device, appManager.dynamicUniformBufferData.buffer, nullptr);
	vk::FreeMemory(appManager.device, appManager.dynamicUniformBufferData.memory, nullptr);

	// Destroy the pipeline followed by the pipeline layout.
	vk::DestroyPipeline(appManager.device, appManager.pipeline, nullptr);
	vk::DestroyPipelineLayout(appManager.device, appManager.pipelineLayout, nullptr);

	// Destroy the texture image.
	vk::DestroyImage(appManager.device, appManager.texture.image, nullptr);

	// Destroy the texture image view.
	vk::DestroyImageView(appManager.device, appManager.texture.view, nullptr);

	// Free the memory allocated for the texture.
	vk::FreeMemory(appManager.device, appManager.texture.memory, nullptr);

	// Destroy the sampler.
	vk::DestroySampler(appManager.device, appManager.texture.sampler, nullptr);

	// Destroy then free the memory for the vertex buffer.
	vk::DestroyBuffer(appManager.device, appManager.vertexBuffer.buffer, nullptr);
	vk::FreeMemory(appManager.device, appManager.vertexBuffer.memory, nullptr);

	// Iterate through each of the framebuffers and destroy them.
	for (uint32_t i = 0; i < appManager.frameBuffers.size(); i++) { vk::DestroyFramebuffer(appManager.device, appManager.frameBuffers[i], nullptr); }

	// Destroy the two shader modules - vertex and fragment.
	vk::DestroyShaderModule(appManager.device, appManager.shaderStages[0].module, nullptr);
	vk::DestroyShaderModule(appManager.device, appManager.shaderStages[1].module, nullptr);

	// Destroy the render pass.
	vk::DestroyRenderPass(appManager.device, appManager.renderPass, nullptr);

	// Clean up the swapchain image views.
	for (auto& imagebuffers : appManager.swapChainImages) { vk::DestroyImageView(appManager.device, imagebuffers.view, nullptr); }

	// Free the allocated memory in the command buffers.
	vk::FreeCommandBuffers(appManager.device, appManager.commandPool, static_cast<uint32_t>(appManager.cmdBuffers.size()), appManager.cmdBuffers.data());

	// Destroy the command pool.
	vk::DestroyCommandPool(appManager.device, appManager.commandPool, nullptr);

	// Clean up the swapchain.
	vk::DestroySwapchainKHR(appManager.device, appManager.swapchain, nullptr);

	// Clean up the surface.
	vk::DestroySurfaceKHR(appManager.instance, appManager.surface, nullptr);

	// Destroy the logical device.
	vk::DestroyDevice(appManager.device, nullptr);
}
