#ifndef VKQUEUES_H
#define VKQUEUES_H

#include "vkStructs.h"

/// <summary>Finds the indices of compatible graphics and present queues and returns them</summary>
/// <param name="graphicsfamilyindex">Compatible graphics queue index that is returned</param>
/// <param name="presentfamilyindex">Compatible present queue index that is returned</param>
static void getCompatibleQueueFamilies(AppManager& appManager, uint32_t& graphicsfamilyindex, uint32_t& presentfamilyindex)
{
    // This function iterates through all the queue families available on the selected device and selects a graphics queue
    // family and a present queue family by selecting their associated indices. It also checks that the present queue family
    // supports presenting.
    int i = 0;
    bool presentFound = false, graphicsFound = false;
    for (const auto& queueFamily : appManager.queueFamilyProperties)
    {
        // Check for graphical operations support.
        if ((queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) && !graphicsFound)
        {
            graphicsfamilyindex = i;
            graphicsFound = true;
        }

        // Check if the queue family supports presenting to our surface.
        VkBool32 compatible = VK_FALSE;
        debugAssertFunctionResult(vk::GetPhysicalDeviceSurfaceSupportKHR(appManager.physicalDevice, i, appManager.surface, &compatible), "Querying Physical Device Surface Support");
        if (compatible && !presentFound)
        {
            presentfamilyindex = i;
            presentFound = true;
        }

        if (graphicsFound && presentFound) break;
        i++;
    }
}


/// <summary>Queries the physical device for supported queue families</summary>
static void initQueuesFamilies(AppManager& appManager)
{
    // Concept: Queues and Queues Families
    // Queues are needed by Vulkan to execute commands on, such as drawing or memory transfers.
    // Queue families are in their simplest form a collection of queues that share properties related to the type of commands allowed to execute.
    // Queue families make sure that the collection of queues being used is compatible with the operations that the developer wants to execute.

    // This function queries the physical device for supported queue families and then identifies two queue families which support rendering and presenting.
    // These could be the same if one queue family supports both operations but this will be dealt with later.

    // This will hold the number of queue families available.
    uint32_t queueFamiliesCount;

    // Get the number of queue families the physical device supports.
    vk::GetPhysicalDeviceQueueFamilyProperties(appManager.physicalDevice, &queueFamiliesCount, nullptr);

    // Resize the vector to fit the number of supported queue families.
    appManager.queueFamilyProperties.resize(queueFamiliesCount);

    // Load the queue families data from the physical device to the list.
    vk::GetPhysicalDeviceQueueFamilyProperties(appManager.physicalDevice, &queueFamiliesCount, &appManager.queueFamilyProperties[0]);

    // Get the indices of compatible queue families.
    getCompatibleQueueFamilies(appManager, appManager.graphicsQueueFamilyIndex, appManager.presentQueueFamilyIndex);
}


/// <summary>Creates a rendering and a present queue for executing commands</summary>
static void initQueues(AppManager& appManager)
{
    // The queues that will be used for executing commands on needs to be retrieved.
    // Two queues are needed: one for rendering and the other to present the rendering on the surface.
    // Some devices support both operations on the same queue family.

    // Get the queue from the logical device created earlier and save it for later.
    vk::GetDeviceQueue(appManager.device, appManager.graphicsQueueFamilyIndex, 0, &appManager.graphicQueue);

    // If the queue family indices are the same, then the same queue is used to do both operations.
    // If not, another queue is retrieved for presenting.
    if (appManager.graphicsQueueFamilyIndex == appManager.presentQueueFamilyIndex) { appManager.presentQueue = appManager.graphicQueue; }
    else
    {
        vk::GetDeviceQueue(appManager.device, appManager.presentQueueFamilyIndex, 0, &appManager.presentQueue);
    }
}


#endif // VKQUEUES_H
