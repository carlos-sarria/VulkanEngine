/*!*********************************************************************************************************************
\File         VulkanHelloAPI.h
\Title        VulkanHello API Header
\Author       PowerVR by Imagination, Developer Technology Team.
\Copyright    Copyright(c) Imagination Technologies Limited.
\brief        Header file for VulkanHelloAPI class. It also contains helper functions and structs.
***********************************************************************************************************************/
#pragma once
#include <string>
#include <array>
#include <limits>
#include <sstream>

#include "vk_getProcAddrs.h"
#include "MainWindows.h"

#include "vkStructs.h"

// Constants used throughout the example.
#define FENCE_TIMEOUT std::numeric_limits<uint64_t>::max()
#define NUM_DESCRIPTOR_SETS 2

const float PI = 3.14159265359f;
const float TORAD = PI / 180.0f;

class VulkanHelloAPI
{
private:
	// Custom structs that encapsulates related data to help keep track of
    // the multiple aspects of different Vulkan objects
	std::array<std::array<float, 4>, 4> viewProj = std::array<std::array<float, 4>, 4>();



	void initUniformBuffers();

public:


	// Create vertex buffers to draw the primitive.
	void initVertexBuffers();

	// Create a descriptor pool and allocate descriptor sets for the buffers.
	void initDescriptorPoolAndSet();

	// Compile and convert the shaders that will be used.
	void initShaders();

	// Create the pipeline to use for the rendering.
	void initPipeline();

	// Create the render pass to use for rendering the triangle.
	void initRenderPass();

	// Create the frame buffers for rendering.
	void initFrameBuffers();

	// Create the command buffer to be sent to the GPU from the command pool.
	void initCommandPoolAndBuffer();

	// Initialise the viewport and scissor for the rendering.
	void initViewportAndScissor();

	// Create the semaphore to deal with the command queue.
	void initSemaphoreAndFence();


	// Generic method for creating a dynamic uniform buffer.
	void createDynamicUniformBuffer(BufferData& inBuffer);

	// Generic method for creating a shader module.
	void createShaderModule(const uint32_t* spvShader, size_t spvShaderSize, int indx, VkShaderStageFlagBits shaderStage);

	// Changes the rotation of the per-frame uniform buffer.
	void applyRotation(int idx = 0);

	// Initialises all the needed Vulkan objects, but calling all the Init__ methods.
	void initialize();

	// Cleans up everything when the application is finished with.
	void deinitialize();

	// Record the command buffer for rendering the example.
	void recordCommandBuffer();

	// Execute the command buffer and present the result to the surface.
	void drawFrame();

	// Holds all the Vulkan handles that global access is required for.
	AppManager appManager;

	// Used for debugging mostly; to show the VKResult return from the Vulkan function calls.
	VkResult lastRes;

	// Keeps track of the frame for synchronisation purposes.
	int frameId;

	// Surface data needed to distinguish between the different platforms.
	SurfaceData surfaceData;
};
