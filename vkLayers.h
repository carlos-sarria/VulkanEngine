#ifndef VKLAYERS_H
#define VKLAYERS_H

#include "vkStructs.h"

const static uint32_t NumInstanceLayers = 3;

const std::string InstanceLayers[NumInstanceLayers] = {
#ifdef DEBUG
    // Khronos Validation is a layer which encompasses all of the functionality that used to be contained in VK_LAYER_GOOGLE_threading,
    // VK_LAYER_LUNARG_parameter_validation, VK_LAYER_LUNARG_object_tracker, VK_LAYER_LUNARG_core_validation, and VK_LAYER_GOOGLE_unique_objects
    // Validation layer functionality can be extended when creating the instance to allow for best practices
    "VK_LAYER_KHRONOS_validation",
    // Standard Validation is a (now deprecated) meta-layer managed by the LunarG Loader.
    // Using Standard Validation will cause the loader to load a standard set of validation layers in an optimal order:
    // * VK_LAYER_GOOGLE_threading.
    // * VK_LAYER_LUNARG_parameter_validation.
    // * VK_LAYER_LUNARG_object_tracker.
    // * VK_LAYER_LUNARG_core_validation.
    // * VK_LAYER_GOOGLE_unique_objects.
    "VK_LAYER_LUNARG_standard_validation",
    // PerfDoc is a Vulkan layer built to identify best practices and Vulkan API usage, specifically for Imagination GPUs
    "VK_LAYER_IMG_powervr_perf_doc"
#else
    ""
#endif
};

/// <summary>Checks whether validation layers that are supported have been enabled by the application</summary>
/// <param name="layerProperties">Vector of supported layer properties</param>
/// <param name="layersToEnable">Array of the names of layers which the application wants to enable</param>
/// <param name="layerCount">Size of layersToEnable</param>
/// <returns>Vector of the names of validation layer names which are supported and required by the application.</returns>
inline std::vector<std::string> _filterLayers(const std::vector<VkLayerProperties>& layerProperties, const std::string* layersToEnable, uint32_t layerCount)
{
    // For each layer supported by a particular device, check whether the application has chosen to enable it. If the chosen layer to enable exists in the list
    // of layers to enable, then add the layer to a list of layers to return to the application.

    std::vector<std::string> outLayers;
    for (const auto& layerProperty : layerProperties)
    {
        for (uint32_t j = 0; j < layerCount; ++j)
        {
            if (!strcmp(layersToEnable[j].c_str(), layerProperty.layerName)) { outLayers.emplace_back(layersToEnable[j]); }
        }
    }
    return outLayers;
}



/// <summary>Finds the names of the required validation layers</summary>
/// <returns>Vector of the names of required validation layers which are to be activated</returns>
inline std::vector<std::string> _initLayers()
{
    // Due to the (intentionally) limited overhead in Vulkan, error checking is virtually non-existent.
    // Validation layers provide some error checking functionality but they will first need to be initialised.

    // Concept: Validation Layers
    // Validation layers help in tracking API objects and calls, making sure there are no validity errors in the code.
    // They are initialised by the Vulkan loader when vk::CreateInstance is called.

    // This vector will store the supported instance layers that will be returned.
    std::vector<std::string> layerNames;

// This ensures validation layers will only be enabled during
// debugging, reducing the overhead of the final release version.
#ifdef PVR_DEBUG
    // Create a vector to hold the layer properties.
    std::vector<VkLayerProperties> outLayers;
    uint32_t numItems = 0;
    // Enumerate all the layer properties to find the total number of items to add to the vector created above.
    debugAssertFunctionResult(vk::EnumerateInstanceLayerProperties(&numItems, nullptr), "Fetching Layer count");

    // Resize the vector to hold the result from vk::EnumerateInstanceLayerProperties.
    outLayers.resize(numItems);

    // Enumerate once more, this time pass the vector and fetch the layer properties themselves to store them in the vector.
    debugAssertFunctionResult(vk::EnumerateInstanceLayerProperties(&numItems, outLayers.data()), "Fetching Layer Data");

    // Log the supported layers on this system.
    Log(false, "---------- Supported Layers ----------");
    for (auto&& layer : outLayers) { Log(false, ">> %s", layer.layerName); }
    Log(false, "--------------------------------------");

    layerNames = filterLayers(outLayers, InstanceLayers, NumInstanceLayers);

    bool requestedStdValidation = false;
    bool supportsStdValidation = false;
    bool supportsKhronosValidation = false;
    uint32_t stdValidationRequiredIndex = -1;

    for (const auto& InstanceLayer : InstanceLayers)
    {
        if (!strcmp(InstanceLayer.c_str(), "VK_LAYER_LUNARG_standard_validation"))
        {
            requestedStdValidation = true;
            break;
        }
    }

    // This code is to cover cases where VK_LAYER_LUNARG_standard_validation is requested but is not supported. This is where on some platforms the
    // component layers enabled via VK_LAYER_LUNARG_standard_validation may still be supported, even though VK_LAYER_LUNARG_standard_validation itself is not.
    if (requestedStdValidation)
        for (const auto& SupportedInstanceLayer : layerNames)
        {
            if (!strcmp(SupportedInstanceLayer.c_str(), "VK_LAYER_LUNARG_standard_validation")) { supportsStdValidation = true; }
            if (!strcmp(SupportedInstanceLayer.c_str(), "VK_LAYER_KHRONOS_validation")) { supportsKhronosValidation = true; }
        }

    // This code is to cover cases where VK_LAYER_LUNARG_standard_validation is requested but is not supported, where on some platforms the
    // component layers enabled via VK_LAYER_LUNARG_standard_validation may still be supported even though VK_LAYER_LUNARG_standard_validation is not.
    // Only perform the expansion if VK_LAYER_LUNARG_standard_validation is requested and not supported and the newer equivalent layer VK_LAYER_KHRONOS_validation is also not supported
    if (requestedStdValidation && !supportsStdValidation && !supportsKhronosValidation)
    {
        for (auto it = outLayers.begin(); !supportsStdValidation && it != outLayers.end(); ++it)
        { supportsStdValidation = !strcmp(it->layerName, "VK_LAYER_LUNARG_standard_validation"); }
        if (!supportsStdValidation)
        {
            for (uint32_t i = 0; stdValidationRequiredIndex == static_cast<uint32_t>(-1) && i < outLayers.size(); ++i)
            {
                if (!strcmp(InstanceLayers[i].c_str(), "VK_LAYER_LUNARG_standard_validation")) { stdValidationRequiredIndex = i; }
            }

            for (uint32_t j = 0; j < NumInstanceLayers; ++j)
            {
                if (stdValidationRequiredIndex == j && !supportsStdValidation)
                {
                    const char* stdValComponents[] = { "VK_LAYER_GOOGLE_threading", "VK_LAYER_LUNARG_parameter_validation", "VK_LAYER_LUNARG_object_tracker",
                                                      "VK_LAYER_LUNARG_core_validation", "VK_LAYER_GOOGLE_unique_objects" };
                    for (auto& stdValComponent : stdValComponents)
                    {
                        for (auto& outLayer : outLayers)
                        {
                            if (!strcmp(stdValComponent, outLayer.layerName))
                            {
                                layerNames.emplace_back(stdValComponent);
                                break;
                            }
                        }
                    }
                }
            }

            // Filter the layers again. This time checking for support for the component layers enabled via VK_LAYER_LUNARG_standard_validation.
            layerNames = filterLayers(outLayers, layerNames.data(), static_cast<uint32_t>(layerNames.size()));
        }
    }

    Log(false, "---------- Supported Layers to be enabled ----------");
    for (auto&& layer : layerNames) { Log(false, ">> %s", layer.c_str()); }
    Log(false, "--------------------------------------");
#endif

    return layerNames;
}

#endif // VKLAYERS_H
