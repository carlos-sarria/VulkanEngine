/*!*********************************************************************************************************************
\File         EngineExample.h
\Title        VulkanHello API Header
\Author       PowerVR by Imagination, Developer Technology Team.
\Copyright    Copyright(c) Imagination Technologies Limited.
\brief        Header file for EngineExample class. It also contains helper functions and structs.
***********************************************************************************************************************/
#pragma once
#include <string>
#include <array>
#include <limits>
#include <sstream>

#define VK_USE_PLATFORM_WIN32_KHR
#include "vkEngine.h"

// Constants used throughout the example.
#define FENCE_TIMEOUT std::numeric_limits<uint64_t>::max()
#define NUM_DESCRIPTOR_SETS 2

const float TORAD = PI / 180.0f;

class EngineExample
{
private:
	// Custom structs that encapsulates related data to help keep track of
    // the multiple aspects of different Vulkan objects
	std::array<std::array<float, 4>, 4> viewProj = std::array<std::array<float, 4>, 4>();

public:

    vkEngine eng;

	// Changes the rotation of the per-frame uniform buffer.
	void applyRotation(int idx = 0);

	// Initialises all the needed Vulkan objects, but calling all the Init__ methods.
    void initialize(const char* appName);

	// Cleans up everything when the application is finished with.
    void deinitialize() { eng.closeDown(); }

	// Execute the command buffer and present the result to the surface.
	void drawFrame();
};
