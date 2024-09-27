#ifndef VKMEMORY_H
#define VKMEMORY_H

#include "vkStructs.h"

// Check type of memory using the device memory properties.
inline bool _getMemoryTypeFromProperties(const VkPhysicalDeviceMemoryProperties& memory_properties, uint32_t typeBits, VkFlags requirements_mask, uint32_t* typeIndex)
{
    // Search memory types to find first index with those properties.
    for (uint32_t i = 0; i < memory_properties.memoryTypeCount; i++)
    {
        if ((typeBits & 1) == 1)
        {
            // Type is available, does it match user properties?
            if ((memory_properties.memoryTypes[i].propertyFlags & requirements_mask) == requirements_mask)
            {
                *typeIndex = i;
                return true;
            }
        }
        typeBits >>= 1;
    }
    // No memory types matched, return failure.
    return false;
}

/// <summary>Creates a buffer, allocates it memory, maps the memory and copies the data into the buffer</summary>
/// <param name="inBuffer">Vkbuffer handle in which the newly-created buffer object is returned</param>
/// <param name="inData">Data to be copied into the buffer</param>
/// <param name="inUsage">Usage flag which determines what type of buffer will be created</param>
inline void _createBuffer(AppManager& appManager, BufferData& inBuffer, const uint8_t* inData, const VkBufferUsageFlags& inUsage)
{
    // This is a generic function which is used to create buffers.
    // It is responsible for creating a buffer object, allocating the memory, mapping this memory, and
    // copying the data into the buffer. The usage flag that determines the type of buffer that is going to be used
    // is passed when this function is called.

    // Declare and populate a buffer creation info struct.
    // This tells the API the size of the buffer and how it is going to be used. Additionally, it specifies whether the
    // buffer is going to be accessed by multiple queue families at the same time and if so, what those queue families are.
    VkBufferCreateInfo bufferInfo = {};
    bufferInfo.flags = 0;
    bufferInfo.pNext = nullptr;
    bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
    bufferInfo.size = inBuffer.size;
    bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    bufferInfo.usage = inUsage;
    bufferInfo.pQueueFamilyIndices = nullptr;
    bufferInfo.queueFamilyIndexCount = 0;

    // Create the buffer object itself.
    debugAssertFunctionResult(vk::CreateBuffer(appManager.device, &bufferInfo, nullptr, &inBuffer.buffer), "Buffer Creation");

    // Define a struct to hold the memory requirements for the buffer.
    VkMemoryRequirements memoryRequirments;

    // Extract the memory requirements for the buffer.
    vk::GetBufferMemoryRequirements(appManager.device, inBuffer.buffer, &memoryRequirments);

    // Populate an allocation info struct with the memory requirement size.
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.pNext = nullptr;
    allocateInfo.memoryTypeIndex = 0;
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirments.size;

    // Check if the memory that is going to be used supports the necessary flags for the usage of the buffer.
    // In this case it needs to be "Host Coherent" in order to be able to map it. If it is not, find a compatible one.
    bool pass = _getMemoryTypeFromProperties(appManager.deviceMemoryProperties, memoryRequirments.memoryTypeBits,
                                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &(allocateInfo.memoryTypeIndex));
    if (pass)
    {
        // This pointer will be used to pass the data into the buffer.
        uint8_t* pData;

        // Allocate the memory necessary for the data.
        debugAssertFunctionResult(vk::AllocateMemory(appManager.device, &allocateInfo, nullptr, &(inBuffer.memory)), "Allocate Buffer Memory");

        // Save the data in the buffer struct.
        inBuffer.bufferInfo.range = memoryRequirments.size;
        inBuffer.bufferInfo.offset = 0;
        inBuffer.bufferInfo.buffer = inBuffer.buffer;

        VkMemoryPropertyFlags flags = appManager.deviceMemoryProperties.memoryTypes[allocateInfo.memoryTypeIndex].propertyFlags;
        inBuffer.memPropFlags = flags;

        if (inData != nullptr)
        {
            // Map data to the memory.
            // inBuffer.memory is the device memory handle.
            // memoryRequirments.size is the size of the memory to be mapped, in this case it is the entire buffer.
            // &pData is an output variable and will contain a pointer to the mapped data.
            debugAssertFunctionResult(vk::MapMemory(appManager.device, inBuffer.memory, 0, inBuffer.size, 0, reinterpret_cast<void**>(&pData)), "Map Buffer Memory");

            // Copy the data into the pointer mapped to the memory.
            memcpy(pData, inData, inBuffer.size);

            VkMappedMemoryRange mapMemRange = {
                VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
                nullptr,
                inBuffer.memory,
                0,
                inBuffer.size,
            };

            // ONLY flush the memory if it does not support VK_MEMORY_PROPERTY_HOST_COHERENT_BIT.
            if (!(flags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT)) { vk::FlushMappedMemoryRanges(appManager.device, 1, &mapMemRange); }
        }

        // Associate the allocated memory with the previously created buffer.
        // Mapping and binding do not need to occur in a particular order. This step could just as well be performed before mapping
        // and populating.
        debugAssertFunctionResult(vk::BindBufferMemory(appManager.device, inBuffer.buffer, inBuffer.memory, 0), "Bind Buffer Memory");
    }
}


#endif // VKMEMORY_H
