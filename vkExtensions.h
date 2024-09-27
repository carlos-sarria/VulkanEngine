#ifndef VKEXTENSIONS_H
#define VKEXTENSIONS_H

#include "vkStructs.h"

/// <summary>Selects required instance-level extensions</summary>
/// <returns>Vector of the names of required instance-level extensions</returns>
inline std::vector<std::string> _initInstanceExtensions()
{
    // Concept: Extensions
    // Extensions extend the API's functionality; they may add additional features or commands. They can be used for a variety of purposes,
    // such as providing compatibility for specific hardware. Instance-level extensions are extensions with global-functionality; they affect
    // both the instance-level and device-level commands. Device-level extensions specifically affect the device they are bound to.
    // Surface and swapchain functionality are both found in extensions as Vulkan does not make assumptions about the type of application, as not all applications are graphical;
    // for example - compute applications. For this reason they are both considered extensions that add functionality to the core API. The surface extension is an instance-level
    // extension and is added to the instanceExtensionNames vector, while the swapchain is a device-level one and is added to deviceExtensionNames.

    // This function selects the two instance-level extensions which are required by this application.

    // This vector will store a list of supported instance extensions that will be returned. The general surface extension is added to this vector first.
    std::vector<std::string> extensionNames;

    extensionNames.emplace_back(VK_KHR_SURFACE_EXTENSION_NAME);

// An additional surface extension needs to be loaded. This extension is platform-specific so needs to be selected based on the
// platform the example is going to be deployed to.
// Preprocessor directives are used here to select the correct platform.
#ifdef VK_USE_PLATFORM_WIN32_KHR
    extensionNames.emplace_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_XLIB_KHR
    extensionNames.emplace_back(VK_KHR_XLIB_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_XCB_KHR
    extensionNames.emplace_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_ANDROID_KHR
    extensionNames.emplace_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_WAYLAND_KHR
    extensionNames.emplace_back(VK_KHR_WAYLAND_SURFACE_EXTENSION_NAME);
#endif
#ifdef VK_USE_PLATFORM_MACOS_MVK
    extensionNames.emplace_back(VK_MVK_MACOS_SURFACE_EXTENSION_NAME);
#endif
#ifdef USE_PLATFORM_NULLWS
    extensionNames.emplace_back(VK_KHR_DISPLAY_EXTENSION_NAME);
#endif

    return extensionNames;
}

/// <summary>Selects required device-level extensions</summary>
/// <returns>Vector of the names of required device-level extensions</returns>
inline std::vector<std::string> _initDeviceExtensions()
{
    // The VK_KHR_swapchain extension is device-level. The device-level extension names are stored in a
    // separate vector from the instance-level extension names.
    std::vector<std::string> extensionNames;
    extensionNames.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
    return extensionNames;
}


#endif // VKEXTENSIONS_H
