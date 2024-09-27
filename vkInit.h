#ifndef VKINIT_H
#define VKINIT_H

#include "vkStructs.h"
#include "vk_getProcAddrs.h"

/// <summary>Creates a Vulkan instance</summary>
/// <param name="extensionNames">Vector of the names of the required instance-level extensions</param>
/// <param name="layerNames">Vector of the names of the required validation layers</param>
inline void _initApplicationAndInstance(AppManager& appManager, std::vector<std::string>& extensionNames, std::vector<std::string>& layerNames)
{
    // This is where the Vulkan instance is created. Vulkan does not have a global state like OpenGL, so a
    // handle is required to access its functions. The instance is the primary access to the API.

    // Declare and populate the application info.
    // When creating objects in Vulkan using "vkCreate..." functions, a creation struct must be defined. This struct contains information describing the properties of the
    // object which is going to be created. In this case, applicationInfo contains properties such as the chosen name of the application and the version of Vulkan used.
    VkApplicationInfo applicationInfo = {};
    applicationInfo.pNext = nullptr;
    applicationInfo.pApplicationName = "Vulkan Hello API Sample";
    applicationInfo.applicationVersion = 1;
    applicationInfo.engineVersion = 1;
    applicationInfo.pEngineName = "Vulkan Hello API Sample";
    applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    applicationInfo.apiVersion = VK_API_VERSION_1_0;

    // Declare an instance creation info struct.
    // instanceInfo specifies the parameters of a newly created Vulkan instance. The
    // application info struct populated above is referenced here along with the instance layers and extensions.
    VkInstanceCreateInfo instanceInfo = {};
    instanceInfo.pNext = nullptr;
    instanceInfo.flags = 0;
    instanceInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    instanceInfo.pApplicationInfo = &applicationInfo;

    // Assign the number and names of the instance layers to be enabled.
    appManager.instanceLayerNames.resize(layerNames.size());
    for (uint32_t i = 0; i < layerNames.size(); ++i) { appManager.instanceLayerNames[i] = layerNames[i].c_str(); }

    instanceInfo.enabledLayerCount = static_cast<uint32_t>(appManager.instanceLayerNames.size());
    instanceInfo.ppEnabledLayerNames = appManager.instanceLayerNames.data();

    // Assign the number and names of the instance extensions to be enabled.
    appManager.instanceExtensionNames.resize(extensionNames.size());
    for (uint32_t i = 0; i < extensionNames.size(); ++i) { appManager.instanceExtensionNames[i] = extensionNames[i].c_str(); }

    instanceInfo.enabledExtensionCount = static_cast<uint32_t>(appManager.instanceExtensionNames.size());
    instanceInfo.ppEnabledExtensionNames = appManager.instanceExtensionNames.data();

    // If the enabled instance layers contain the Khronos validation layer, then attempt to extend the functionality of
    // that layer by using the validation feature extension. This can be used to enable/disable features of the
    // validation layers. In this instance, the aim is to add best practice checking to ensure that Vulkan's best
    // practices are being followed.

    // Find out if validation features are supported
    bool validationFeaturesSupported = false;
    for (auto const& layerName : appManager.instanceLayerNames)
    {
        if (!strcmp(layerName, "VK_LAYER_KHRONOS_validation"))
        {
            std::vector<VkExtensionProperties> validationLayerInstanceExtensions;
            uint32_t extensionCount;
            debugAssertFunctionResult(vk::EnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &extensionCount, nullptr), "Enumerate instance extension properties");

            validationLayerInstanceExtensions.resize(extensionCount);
            debugAssertFunctionResult(vk::EnumerateInstanceExtensionProperties("VK_LAYER_KHRONOS_validation", &extensionCount, validationLayerInstanceExtensions.data()),
                                      "Enumerate instance extension properties");

            // Check each of the extensions provided by the Khronos validation layer and ensure one of them is the validation features.
            for (auto it = validationLayerInstanceExtensions.begin(); !validationFeaturesSupported && it != validationLayerInstanceExtensions.end(); it++)
            { validationFeaturesSupported = !strcmp(it->extensionName, "VK_EXT_validation_features"); }
            break;
        }
    }

    // If the validation features are availble, then enable the best practices feature
    if (validationFeaturesSupported)
    {
        VkValidationFeatureEnableEXT enabledValidationFeatures = VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT;
        VkValidationFeaturesEXT validationFeatures = {};
        validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
        validationFeatures.enabledValidationFeatureCount = 1;
        validationFeatures.pEnabledValidationFeatures = &enabledValidationFeatures;

        // The validation features are added by being attached to the vulkan instance.
        instanceInfo.pNext = &validationFeatures;
    }

    // Create a Vulkan application instance using the instanceInfo struct defined above.
    // The handle to this new instance is stored in appManager.instance for access elsewhere.
    debugAssertFunctionResult(vk::CreateInstance(&instanceInfo, nullptr, &appManager.instance), "Create Instance");

    // The pointers to the functions which depend on the Vulkan instance need to be initialised. GetInstanceProcAddr is used to find the correct function
    // pointer associated with this instance. This is not necessary but it is a best practice. It provides a way to bypass the Vulkan loader and grants a
    // small performance boost.
    if (!vk::initVulkanInstance(appManager.instance)) { Log(true, "Could not initialise the instance function pointers."); }
}

/// <summary>Finds a physical device which is compatible with the applications's requirements</summary>
/// <returns>Handle to a compatible physical device</returns>
inline VkPhysicalDevice _getCompatibleDevice(AppManager& appManager)
{
    // Iterate through the available physical devices to determine which one is compatible with the application's requirements.

    for (const auto& device : appManager.gpus)
    {
        VkPhysicalDeviceProperties deviceProperties;
        VkPhysicalDeviceFeatures deviceFeatures;
        vk::GetPhysicalDeviceProperties(device, &deviceProperties);
        vk::GetPhysicalDeviceFeatures(device, &deviceFeatures);

        // Return the first device which is either a discrete GPU or an integrated GPU.
        if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU || deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU)
        {
            Log(false, "Active Device is -- %s", deviceProperties.deviceName);
            return device;
        }
    }

    // If there is only one device, then return that one.
    if (appManager.gpus.size() == 1) { return appManager.gpus[0]; }

    // Return null if nothing is found.
    return nullptr;
}

/// <summary>Selects the physical device most compatible with application requirements</summary>
inline void _initPhysicalDevice(AppManager& appManager)
{
    // Concept: Physical Devices
    // A physical device needs to be chosen. A physical device represents a GPU used for operations.
    // All physical devices will be queried, and the device with the greatest compatibility with the application's needs will be used.

    // This function selects an available physical device which is most compatible with the application's requirements.

    // This will hold the number of GPUs available.
    uint32_t gpuCount;

    // Query for the number of GPUs available.
    debugAssertFunctionResult(vk::EnumeratePhysicalDevices(appManager.instance, &gpuCount, nullptr), "GPUS Enumeration - Get Count");

    // Resize the GPUs vector to match the number of GPUs available.
    appManager.gpus.resize(gpuCount);

    // Populate the vector with a list of GPUs available on the platform.
    debugAssertFunctionResult(vk::EnumeratePhysicalDevices(appManager.instance, &gpuCount, appManager.gpus.data()), "GPUS Enumeration - Allocate Data");

    // Log some properties for each of the available physical devices.
    Log(false, "%s", "------------Properties for Physical Devices--------------");
    for (const auto& device : appManager.gpus)
    {
        // General device properties like vendor and driver version.
        VkPhysicalDeviceProperties deviceProperties;
        vk::GetPhysicalDeviceProperties(device, &deviceProperties);

        Log(false, "Device Name: %s", deviceProperties.deviceName);
        Log(false, "Device ID: 0x%X", deviceProperties.deviceID);
        Log(false, "Device Driver Version: 0x%X", deviceProperties.driverVersion);
        Log(false, "%s", "--------------------------------------");

        // Features are more in-depth information that is not needed right now so these are not outputted.
        VkPhysicalDeviceFeatures deviceFeatures;
        vk::GetPhysicalDeviceFeatures(device, &deviceFeatures);
    }

    // Get the device compatible with the needs of the application using a custom helper function.
    // The physical device is also queried for its memory properties which will be used later when allocating memory for buffers.
    appManager.physicalDevice = _getCompatibleDevice(appManager);
    vk::GetPhysicalDeviceMemoryProperties(appManager.physicalDevice, &appManager.deviceMemoryProperties);

    // Get the compatible device's properties.
    // These properties will be used later when creating the surface and swapchain objects.
    vk::GetPhysicalDeviceProperties(appManager.physicalDevice, &appManager.deviceProperties);
}

/// <summary>Creates a Vulkan logical device</summary>
/// <param name="deviceExtensions">Vector of the names of the required device-level extensions</param>
inline void _initLogicalDevice(AppManager& appManager, std::vector<std::string>& deviceExtensions)
{
    // A logical device is required to start using the API. This function creates a logical device
    // and a graphics queue to execute commands on.

    // Concept: Logical Devices
    // A logical device is an application view of the physical device that will be used. The logical device is
    // used to load the device extensions and create the rest of the Vulkan API objects.

    // There are priorities for queues (range: 0 - 1). Each queue in the same device is assigned a priority with higher priority queues
    // potentially being given more processing time than lower priority ones.
    // In this case there is only one, so it does not matter.
    float queuePriorities[1] = { 0.0f };

    // Populate the device queue creation info struct with the previously found compatible queue family
    // and number of queues to be created. Again, only one queue is needed.
    VkDeviceQueueCreateInfo deviceQueueInfo = {};
    deviceQueueInfo.pNext = nullptr;
    deviceQueueInfo.flags = 0;
    deviceQueueInfo.queueFamilyIndex = appManager.graphicsQueueFamilyIndex;
    deviceQueueInfo.pQueuePriorities = queuePriorities;
    deviceQueueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    deviceQueueInfo.queueCount = 1;

    // Declare and populate the logical device creation info struct. This will be used to create the logical device and its associated queues.
    // The device extensions that were looked up earlier are specified here. They will be initialised when the logical device
    // is created. Additionally, the physical device is queried for its supported features so the logical device can enable them.
    VkDeviceCreateInfo deviceInfo;
    deviceInfo.flags = 0;
    deviceInfo.pNext = nullptr;
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.enabledLayerCount = 0;
    deviceInfo.ppEnabledLayerNames = nullptr;

    appManager.deviceExtensionNames.resize(deviceExtensions.size());
    for (uint32_t i = 0; i < deviceExtensions.size(); ++i) { appManager.deviceExtensionNames[i] = deviceExtensions[i].c_str(); }

    deviceInfo.enabledExtensionCount = static_cast<uint32_t>(appManager.deviceExtensionNames.size());
    deviceInfo.ppEnabledExtensionNames = appManager.deviceExtensionNames.data();
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &deviceQueueInfo;
    VkPhysicalDeviceFeatures features;
    vk::GetPhysicalDeviceFeatures(appManager.physicalDevice, &features);
    features.robustBufferAccess = false;
    deviceInfo.pEnabledFeatures = &features;

    // Create the logical device using the deviceInfo struct defined above.
    debugAssertFunctionResult(vk::CreateDevice(appManager.physicalDevice, &deviceInfo, nullptr, &appManager.device), "Logic Device Creation");

    // Initialise the function pointers that require the device address. This is the same process as for the instance function pointers.
    if (!vk::initVulkanDevice(appManager.device)) { Log(true, "Could not initialise the device function pointers."); }
}

#endif // VKINIT_H
