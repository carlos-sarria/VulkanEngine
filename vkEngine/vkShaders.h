#ifndef VKSHADERS_H
#define VKSHADERS_H

#include "vkStructs.h"
#include "vkMemory.h"
#include "../FragShader.h"
#include "../VertShader.h"

/// <summary>Creates a shader module using pre-compiled SPIR-V shader source code</summary>
/// <param name="spvShader">Shader source code</param>
/// <param name="spvShaderSize">Size of the shader source code in bytes</param>
/// <param name="indx">Specifies which shader stage to define in appManager's shaderStages array</param>
/// <param name="shaderStage">Specifies the stage in the pipeline where the shader will exist</param>
inline void _createShaderModule(AppManager& appManager, const uint32_t* spvShader, size_t spvShaderSize, int indx, VkShaderStageFlagBits shaderStage)
{
    // This function will create a shader module and update the shader stage array. The shader module will hold
    // the data from the pre-compiled SPIR-V shader. A shader stage will also be associated with this shader module. This identifies in which stage of the pipeline this shader
    // will be used.

    // Populate a shader module creation info struct with a pointer to the shader source code and the size of the shader in bytes.
    VkShaderModuleCreateInfo shaderModuleInfo = {};
    shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.flags = 0;
    shaderModuleInfo.pCode = spvShader;
    shaderModuleInfo.codeSize = spvShaderSize;
    shaderModuleInfo.pNext = nullptr;

    // Set the stage of the pipeline that the shader module will be associated with.
    // The shader source code entry point ("main") is also set here.
    appManager.shaderStages[indx].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    appManager.shaderStages[indx].flags = 0;
    appManager.shaderStages[indx].pName = "main";
    appManager.shaderStages[indx].pNext = nullptr;
    appManager.shaderStages[indx].stage = shaderStage;
    appManager.shaderStages[indx].pSpecializationInfo = nullptr;

    // Create a shader module and add it to the shader stage corresponding to the VkShaderStageFlagBits stage.
    debugAssertFunctionResult(vk::CreateShaderModule(appManager.device, &shaderModuleInfo, nullptr, &(appManager.shaderStages[indx].module)), "Shader Module Creation");
}

/// <summary>Creates the vertex and fragment shader modules and loads in compiled SPIR-V code</summary>
inline void _initShaders(AppManager& appManager)
{
    // In Vulkan, shaders are in SPIR-V format which is a byte-code format rather than a human-readable one.
    // SPIR-V can be used for both graphical and compute operations.
    // This function loads the compiled source code (see vertshader.h and fragshader.h) and creates shader modules that are going
    // to be used by the pipeline later on.

    _createShaderModule(appManager, spv_VertShader_bin, sizeof(spv_VertShader_bin), 0, VK_SHADER_STAGE_VERTEX_BIT);
    _createShaderModule(appManager, spv_FragShader_bin, sizeof(spv_FragShader_bin), 1, VK_SHADER_STAGE_FRAGMENT_BIT);
}

/// <summary>Creates the uniform buffers used throughout the demo</summary>
inline void _initUniformBuffers(AppManager& appManager)
{
    // This function creates a dynamic uniform buffer which will hold several transformation matrices. Each of these matrices is associated with a
    // swapchain image created earlier.

    // Vulkan requires that when updating a descriptor of type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, the
    // offset specified is an integer multiple of the minimum required alignment in bytes for the physical device. This also applied to any dynamic alignments used.
    size_t minimumUboAlignment = static_cast<size_t>(appManager.deviceProperties.limits.minUniformBufferOffsetAlignment);

    // The dynamic buffers will be used as uniform buffers. These are later used with a descriptor of type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC and VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER.
    VkBufferUsageFlags usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;

    {
        // Using the minimum uniform buffer offset alignment, the minimum buffer slice size is calculated based on the size of the intended data, or more specifically
        // the size of the smallest chunk of data which may be mapped or updated as a whole.
        // In this case the size of the intended data is the size of a 4 by 4 matrix.
        size_t bufferDataSizePerSwapchain = sizeof(float) * 4 * 4;
        bufferDataSizePerSwapchain = static_cast<uint32_t>(_getAlignedDataSize(bufferDataSizePerSwapchain, minimumUboAlignment));

        // Calculate the size of the dynamic uniform buffer.
        // This buffer will be updated on each frame and must therefore be multi-buffered to avoid issues with using partially updated data, or updating data already in use.
        // Rather than allocating multiple (swapchain) buffers, a larger buffer is allocated and a slice of this buffer will be used per swapchain. This works as
        // long as the buffer is created taking into account the minimum uniform buffer offset alignment.
        appManager.dynamicUniformBufferData.size = bufferDataSizePerSwapchain * appManager.swapChainImages.size();

        // Create the buffer, allocate the device memory, and attach the memory to the newly created buffer object.
        _createBuffer(appManager, appManager.dynamicUniformBufferData, nullptr, usageFlags);
        appManager.dynamicUniformBufferData.bufferInfo.range = bufferDataSizePerSwapchain;

        // Note that only memory created with the memory property flag VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT can be mapped.
        // vkMapMemory retrieves a host virtual address pointer to a region of a mappable memory object.
        debugAssertFunctionResult(vk::MapMemory(appManager.device, appManager.dynamicUniformBufferData.memory, 0, appManager.dynamicUniformBufferData.size, 0,
                                                &appManager.dynamicUniformBufferData.mappedData),
                                  "Could not map the uniform buffer.");
    }
}
/// <summary>Creates a dynamic uniform buffer and allocates its memory</summary>
/// <param name="inBuffer">Vkbuffer handle in which the newly-created buffer object is returned</param>
inline void _createDynamicUniformBuffer(AppManager& appManager, BufferData& inBuffer)
{
    // This function is used to create a dynamic uniform buffer.

    // Concept: Dynamic Uniform Buffers
    // Dynamic uniform buffers are buffers that contain the data for multiple single uniform buffers (usually each associated with a frame) and use offsets to access this data.
    // This minimises the amount of descriptor sets required, and may help optimise write operations.

    // Query the physical device properties, such as the API version of the device, and the device name.
    VkPhysicalDeviceProperties deviceProperties;
    vk::GetPhysicalDeviceProperties(appManager.physicalDevice, &deviceProperties);

    // Check the limit of the dynamic buffers the physical device supports by consulting the device properties.
    if (deviceProperties.limits.maxDescriptorSetUniformBuffersDynamic > 1)
    {
        // Get the alignment of the uniform buffer.
        size_t uboAlignment = (size_t)deviceProperties.limits.minUniformBufferOffsetAlignment;

        // Calculate the size of each offset so that it aligns correctly with the device property alignment.
        appManager.offset = static_cast<uint32_t>(((sizeof(float) * 4) / uboAlignment) * uboAlignment + (((sizeof(float) * 4) % uboAlignment) > 0 ? uboAlignment : 0));

        // Calculate the full size of the buffer.
        inBuffer.size = appManager.swapChainImages.size() * appManager.offset;

        // Create a Buffer Creation info. This will tell the API what the purpose of the buffer is and how to use it.
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.flags = 0;
        bufferInfo.pNext = nullptr;
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = inBuffer.size;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        bufferInfo.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
        bufferInfo.pQueueFamilyIndices = nullptr;
        bufferInfo.queueFamilyIndexCount = 0;

        // Create a buffer.
        debugAssertFunctionResult(vk::CreateBuffer(appManager.device, &bufferInfo, nullptr, &inBuffer.buffer), "Buffer Creation");

        // The memory requirements for the buffer.
        VkMemoryRequirements memoryRequirments;

        // Extract the memory requirements for the buffer.
        vk::GetBufferMemoryRequirements(appManager.device, inBuffer.buffer, &memoryRequirments);

        // Create an allocation info struct which defines the parameters of memory allocation.
        // Pass the memory requirements size.
        VkMemoryAllocateInfo allocateInfo = {};
        allocateInfo.pNext = nullptr;
        allocateInfo.memoryTypeIndex = 0;
        allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocateInfo.allocationSize = memoryRequirments.size;

        // Check the memory that is going to used is compatible with the operation of this application.
        // If it is not, find the compatible one.
        bool pass = _getMemoryTypeFromProperties(appManager.deviceMemoryProperties, memoryRequirments.memoryTypeBits,
                                                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &(allocateInfo.memoryTypeIndex));

        if (pass)
        {
            // Allocate the memory for the buffer.
            debugAssertFunctionResult(vk::AllocateMemory(appManager.device, &allocateInfo, nullptr, &(inBuffer.memory)), "Dynamic Buffer Memory Allocation");
            inBuffer.memPropFlags = appManager.deviceMemoryProperties.memoryTypes[allocateInfo.memoryTypeIndex].propertyFlags;

            // Save the data to the buffer struct.
            inBuffer.bufferInfo.range = memoryRequirments.size / appManager.swapChainImages.size();
            inBuffer.bufferInfo.offset = 0;
            inBuffer.bufferInfo.buffer = inBuffer.buffer;
        }
    }
}

#endif // VKSHADERS_H
