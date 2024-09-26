/*!*********************************************************************************************************************
\File         VulkanHelloAPI.h
\Title        VulkanHello API Header
\Author       PowerVR by Imagination, Developer Technology Team.
\Copyright    Copyright(c) Imagination Technologies Limited.
\brief        Header file for VulkanHelloAPI class. It also contains helper functions and structs.
***********************************************************************************************************************/
#pragma once
#include <windows.h>

struct SurfaceData
{
	float width, height;
	HINSTANCE connection;
	HWND window;
};
