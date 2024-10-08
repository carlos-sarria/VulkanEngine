#ifndef VKSHADERS_H
#define VKSHADERS_H

#include "vkStructs.h"
#include "vkMemory.h"

/// <summary>Creates a shader module using pre-compiled SPIR-V shader source code</summary>
/// <param name="spvShader">Shader source code</param>
/// <param name="spvShaderSize">Size of the shader source code in bytes</param>
/// <param name="indx">Specifies which shader stage to define in appManager's shaderStages array</param>
/// <param name="shaderStage">Specifies the stage in the pipeline where the shader will exist</param>
inline void _createShaderModule(AppManager& appManager, const char* fileName, int indx, VkShaderStageFlagBits shaderStage)
{
    // This function will create a shader module and update the shader stage array. The shader module will hold
    // the data from the pre-compiled SPIR-V shader. A shader stage will also be associated with this shader module. This identifies in which stage of the pipeline this shader
    // will be used.
    FILE* shaderFile;

    // opening the file in read mode
    shaderFile = fopen(fileName, "rb");
    if(!shaderFile)
    {
        Log(true, (std::string("Failed load shader: ") + fileName).c_str());
        exit(1);
    }

    // File size
    fseek(shaderFile, 0L, SEEK_END);
    unsigned int fileSize = ftell(shaderFile);
    fseek(shaderFile, 0L, SEEK_SET);

    // File data
    uint32_t* fileData = (uint32_t*)malloc(fileSize);
    fread(fileData, sizeof(char), fileSize, shaderFile);

    // Populate a shader module creation info struct with a pointer to the shader source code and the size of the shader in bytes.
    VkShaderModuleCreateInfo shaderModuleInfo = {};
    shaderModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
    shaderModuleInfo.flags = 0;
    shaderModuleInfo.pCode = fileData;
    shaderModuleInfo.codeSize = fileSize;
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

    free(fileData);
    fclose(shaderFile);
}

/// <summary>Creates the vertex and fragment shader modules and loads in compiled SPIR-V code</summary>
inline void _initShaders(AppManager& appManager)
{
    // In Vulkan, shaders are in SPIR-V format which is a byte-code format rather than a human-readable one.
    // SPIR-V can be used for both graphical and compute operations.
    // This function loads the compiled source code (see vertshader.h and fragshader.h) and creates shader modules that are going
    // to be used by the pipeline later on.

    _createShaderModule(appManager, "..\\..\\vert.spv", 0, VK_SHADER_STAGE_VERTEX_BIT);
    _createShaderModule(appManager, "..\\..\\frag.spv", 1, VK_SHADER_STAGE_FRAGMENT_BIT);
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
        // In this case the size of the intended data is the size of a 4 by 4 matrix and a 3 floats light direction.
        size_t bufferDataSizePerSwapchain = sizeof(UBO);
        bufferDataSizePerSwapchain = static_cast<uint32_t>(_getAlignedDataSize(bufferDataSizePerSwapchain, minimumUboAlignment) * appManager.meshes.size());

        // Calculate the size of the dynamic uniform buffer.
        // This buffer will be updated on each frame and must therefore be multi-buffered to avoid issues with using partially updated data, or updating data already in use.
        // Rather than allocating multiple (swapchain) buffers, a larger buffer is allocated and a slice of this buffer will be used per swapchain. This works as
        // long as the buffer is created taking into account the minimum uniform buffer offset alignment.
        appManager.dynamicUniformBufferData.size = bufferDataSizePerSwapchain * appManager.swapChainImages.size() ;

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
#endif // VKSHADERS_H
