#ifndef VKDESCRIPTOR_H
#define VKDESCRIPTOR_H

#include "vkStructs.h"

/// <summary>Creates a static and dynamic descriptor set</summary>
inline void _initDescriptorPoolAndSet(AppManager& appManager)
{
    // Concept: Descriptors and Descriptor Sets
    // In Vulkan, to pass data to shaders, descriptor sets are required. Descriptors (as the name implies) are used to describe the data that is going to be passed. They hold
    // information that helps with binding data to shaders, and additionally describes any information Vulkan needs to know before executing the shader. Descriptors are not
    // passed individually (and are not visible to the application) but instead bundled in sets, known as descriptor sets.

    // The process of creating a descriptor set has three steps:

    // 1) Start by creating a descriptor pool that is used to allocate descriptor sets.
    // 2) Create a descriptor layout that defines how the descriptor set is laid out. This includes information on the binding points and the type of data passed to the shader. The
    // descriptor set layouts are used to create pipeline layouts. Pipeline layouts are essentially a list of all of the descriptor set layouts. They form a complete description of the set of
    // resources that can be accessed by the pipeline. They will be mentioned again when creating the graphics pipeline.
    // 3) Finally, the descriptor set is allocated from the previously created descriptor pool. The descriptor sets themselves hold the data, in the form of a pointer, that is passed to
    // the shader. This can include textures, uniform buffers, and so on.

    // These steps are demonstrated below.

    // This is the size of the descriptor pool. This establishes how many descriptors are needed and their type.
    VkDescriptorPoolSize descriptorPoolSize[2];

     int numTextures = appManager.textures.size(), numDescriptors = numTextures+1;

    descriptorPoolSize[0].descriptorCount = 1;
    descriptorPoolSize[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;

    descriptorPoolSize[1].descriptorCount = 1;
    descriptorPoolSize[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;

    // This is the creation info struct for the descriptor pool.
    // This specifies the size of the pool
    // and the maximum number of descriptor sets that can be allocated out of it.
    // The VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT used here indicates that the descriptor
    // sets can return their allocated memory individually rather than all together.
    VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
    descriptorPoolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    descriptorPoolInfo.pNext = nullptr;
    descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    descriptorPoolInfo.poolSizeCount = numDescriptors;
    descriptorPoolInfo.pPoolSizes = descriptorPoolSize;
    descriptorPoolInfo.maxSets = numDescriptors;

    // Create the descriptor pool.
    debugAssertFunctionResult(vk::CreateDescriptorPool(appManager.device, &descriptorPoolInfo, nullptr, &appManager.descriptorPool), "Descriptor Pool Creation");
    {
        // Populate a descriptor layout binding struct. This defines the type of data that will be passed to the shader and the binding location in the shader stages.
        VkDescriptorSetLayoutBinding descriptorLayoutBinding;
        descriptorLayoutBinding.descriptorCount = 1;
        descriptorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorLayoutBinding.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
        descriptorLayoutBinding.binding = 0;
        descriptorLayoutBinding.pImmutableSamplers = nullptr;

        // Populate an info struct for the creation of the descriptor set layout. The number of bindings previously created is passed in here.
        VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
        descriptorLayoutInfo.flags = 0;
        descriptorLayoutInfo.pNext = nullptr;
        descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorLayoutInfo.bindingCount = 1;
        descriptorLayoutInfo.pBindings = &descriptorLayoutBinding;

        // Create the descriptor set layout for the descriptor set which provides access to the texture data.
        debugAssertFunctionResult(
            vk::CreateDescriptorSetLayout(appManager.device, &descriptorLayoutInfo, nullptr, &appManager.staticDescriptorSetLayout), "Descriptor Set Layout Creation");
    }

    // The process is then repeated for the descriptor set layout of the uniform buffer descriptor set.
    {
        VkDescriptorSetLayoutBinding descriptorLayoutBinding;
        descriptorLayoutBinding.descriptorCount = 1;
        descriptorLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
        descriptorLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
        descriptorLayoutBinding.binding = 0;
        descriptorLayoutBinding.pImmutableSamplers = nullptr;

        // Create the descriptor set layout using the array of VkDescriptorSetLayoutBindings.
        VkDescriptorSetLayoutCreateInfo descriptorLayoutInfo = {};
        descriptorLayoutInfo.flags = 0;
        descriptorLayoutInfo.pNext = nullptr;
        descriptorLayoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
        descriptorLayoutInfo.bindingCount = 1;
        descriptorLayoutInfo.pBindings = &descriptorLayoutBinding;

        // Create the descriptor set layout for the uniform buffer descriptor set.
        debugAssertFunctionResult(
            vk::CreateDescriptorSetLayout(appManager.device, &descriptorLayoutInfo, nullptr, &appManager.dynamicDescriptorSetLayout), "Descriptor Set Layout Creation");
    }

    // Allocate the uniform buffer descriptor set from the descriptor pool.
    // This struct simply points to the layout of the uniform buffer descriptor set and also the descriptor pool created earlier.
    VkDescriptorSetAllocateInfo descriptorAllocateInfo = {};
    descriptorAllocateInfo.descriptorPool = appManager.descriptorPool;
    descriptorAllocateInfo.descriptorSetCount = 1;
    descriptorAllocateInfo.pNext = nullptr;
    descriptorAllocateInfo.pSetLayouts = &appManager.dynamicDescriptorSetLayout;
    descriptorAllocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;

    debugAssertFunctionResult(vk::AllocateDescriptorSets(appManager.device, &descriptorAllocateInfo, &appManager.dynamicDescSet), "Descriptor Set Creation");

    // This information references the texture sampler that will be passed to the shaders by way of
    // the descriptor set. The sampler determines how the pixel data of the texture image will be
    // sampled and how it will be passed to the fragment shader. It also contains the actual image
    // object (via its image view) and the image layout.
    // This image layout is optimised for read-only access by shaders. The image was transitioned to
    // this layout using a memory barrier in initTexture().

    // Update the descriptor sets with the actual objects, in this case the texture image and the uniform buffer.
    // These structs specify which descriptor sets are going to be updated and hold a pointer to the actual objects.
    VkWriteDescriptorSet *descriptorSetWrite;
    descriptorSetWrite = (VkWriteDescriptorSet *)malloc(sizeof(VkWriteDescriptorSet)*numDescriptors);

    int count = 0;

    // Allocate the texture image descriptor set.
    // The allocation struct variable is updated to point to the layout of the texture image descriptor set.
    std::vector<VkDescriptorImageInfo> descriptorImageInfo;
    descriptorAllocateInfo.pSetLayouts = &appManager.staticDescriptorSetLayout; // Same layout as the binding for the shaders/texture is always the same: 0

    for(; count<numTextures; count++)
    {
        appManager.staticDescSet.emplace_back();
        debugAssertFunctionResult(vk::AllocateDescriptorSets(appManager.device, &descriptorAllocateInfo, &appManager.staticDescSet[count]), "Descriptor Set Creation");

        descriptorImageInfo.emplace_back();
        descriptorImageInfo[count].sampler = appManager.textures[count].sampler;
        descriptorImageInfo[count].imageView =  appManager.textures[count].view;
        descriptorImageInfo[count].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

        descriptorSetWrite[count].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        descriptorSetWrite[count].pNext = nullptr;
        descriptorSetWrite[count].dstSet = appManager.staticDescSet[count];
        descriptorSetWrite[count].descriptorCount = 1;
        descriptorSetWrite[count].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        descriptorSetWrite[count].pImageInfo = &descriptorImageInfo[count];
        descriptorSetWrite[count].dstArrayElement = 0;
        descriptorSetWrite[count].dstBinding = 0;
        descriptorSetWrite[count].pBufferInfo = nullptr;
        descriptorSetWrite[count].pTexelBufferView = nullptr;
    }

    descriptorSetWrite[count].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    descriptorSetWrite[count].pNext = nullptr;
    descriptorSetWrite[count].dstSet = appManager.dynamicDescSet;
    descriptorSetWrite[count].descriptorCount = 1;
    descriptorSetWrite[count].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
    descriptorSetWrite[count].pBufferInfo = &appManager.dynamicUniformBufferData.bufferInfo; // Pass uniform buffer to this function.
    descriptorSetWrite[count].dstArrayElement = 0;
    descriptorSetWrite[count].dstBinding = 0;

    vk::UpdateDescriptorSets(appManager.device, numDescriptors, descriptorSetWrite, 0, nullptr);

    free(descriptorSetWrite);
}


#endif // VKDESCRIPTOR_H
