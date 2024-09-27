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

#include "MainWindows.h"

#include "vkEngine.h"

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

public:

	// Changes the rotation of the per-frame uniform buffer.
	void applyRotation(int idx = 0);

	// Initialises all the needed Vulkan objects, but calling all the Init__ methods.
	void initialize();

	// Cleans up everything when the application is finished with.
    void deinitialize() { eng.closeDown(); }

	// Execute the command buffer and present the result to the surface.
	void drawFrame();

    vkEngine eng;

	// Used for debugging mostly; to show the VKResult return from the Vulkan function calls.
	VkResult lastRes;

	// Keeps track of the frame for synchronisation purposes.
	int frameId;
};
