#ifndef VKTEXTURES_H
#define VKTEXTURES_H

#include "vkStructs.h"
#include "vkMemory.h"

/// <summary>Generates a checkered texture on-the-fly</summary>
void generateTexture(AppManager& appManager)
{
    // This function will generate a checkered texture on the fly to be used on the triangle that is going
    // to be rendered and rotated on screen.
    for (uint16_t x = 0; x < appManager.texture.textureDimensions.width; ++x)
    {
        for (uint16_t y = 0; y < appManager.texture.textureDimensions.height; ++y)
        {
            float g = 0.3f;
            if (x % 128 < 64 && y % 128 < 64) { g = 1; }
            if (x % 128 >= 64 && y % 128 >= 64) { g = 1; }

            uint8_t* pixel = (static_cast<uint8_t*>(appManager.texture.data.data())) + (x * appManager.texture.textureDimensions.height * 4) + (y * 4);
            pixel[0] = static_cast<uint8_t>(100 * g);
            pixel[1] = static_cast<uint8_t>(80 * g);
            pixel[2] = static_cast<uint8_t>(70 * g);
            pixel[3] = 255;
        }
    }
}

/// <summary>Initialises the images of a previously created swapchain and creates an associated image view for each image</summary>
static void initImagesAndViews(AppManager& appManager)
{
    // Concept: Images and Views
    // Images in Vulkan are the object representation of data. It can take many forms such as attachments, textures, and so on.
    // Views are a snapshot of the image's parameters. It describes how to access the image and which parts to access.
    // It helps to distinguish the type of image that is being working with.

    // Image objects are used to hold the swapchain images. When the swapchain was created, the
    // images were automatically created alongside it. This function creates an image view for each swapchain image.

    // This vector is used as a temporary vector to hold the retrieved images.
    uint32_t swapchainImageCount;
    std::vector<VkImage> images;

    // Get the number of the images which are held by the swapchain. This is set in InitSwapchain function and is the minimum number of images supported.
    debugAssertFunctionResult(vk::GetSwapchainImagesKHR(appManager.device, appManager.swapchain, &swapchainImageCount, nullptr), "SwapChain Images - Get Count");

    // Resize the temporary images vector to hold the number of images.
    images.resize(swapchainImageCount);

    // Resize the application's permanent swapchain images vector to be able to hold the number of images.
    appManager.swapChainImages.resize(swapchainImageCount);

    // Get all of the images from the swapchain and save them in a temporary vector.
    debugAssertFunctionResult(vk::GetSwapchainImagesKHR(appManager.device, appManager.swapchain, &swapchainImageCount, images.data()), "SwapChain Images - Allocate Data");

    // Iterate over each image in order to create an image view for each one.
    for (uint32_t i = 0; i < swapchainImageCount; ++i)
    {
        // Copy over the images to the permanent vector.
        appManager.swapChainImages[i].image = images[i];

        // Create the image view object itself, referencing one of the retrieved swapchain images.
        VkImageViewCreateInfo image_view_info = {};
        image_view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        image_view_info.pNext = nullptr;
        image_view_info.flags = 0;
        image_view_info.image = appManager.swapChainImages[i].image;
        image_view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        image_view_info.format = appManager.surfaceFormat.format;

        image_view_info.components.r = VK_COMPONENT_SWIZZLE_R;
        image_view_info.components.g = VK_COMPONENT_SWIZZLE_G;
        image_view_info.components.b = VK_COMPONENT_SWIZZLE_B;
        image_view_info.components.a = VK_COMPONENT_SWIZZLE_A;

        image_view_info.subresourceRange.layerCount = 1;
        image_view_info.subresourceRange.levelCount = 1;
        image_view_info.subresourceRange.baseArrayLayer = 0;
        image_view_info.subresourceRange.baseMipLevel = 0;
        image_view_info.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

        debugAssertFunctionResult(vk::CreateImageView(appManager.device, &image_view_info, nullptr, &appManager.swapChainImages[i].view), "SwapChain Images View Creation");
    }
}

/// <summary>Creates a texture image (VkImage) and maps it into GPU memory</summary>
static void loadTexture(AppManager& appManager)
{
    // In Vulkan, uploading an image requires multiple steps:

    // 1) Creating the texture.
    //		a) Create the texture definition ("VkImage" object).
    //		b) Determine its memory requirements and create the backing memory object ("VkDeviceMemory" object).
    //		c) Bind the memory to the image.

    // 2) Uploading the data into the texture.
    //		a) Create a staging buffer.
    //		b) Determine its memory requirements and create the backing memory object ("VkDeviceMemory" object).
    //		c) Map the staging buffer and copy the image data into it.
    //		d) Perform a copy from the staging buffer to the image using the vkCmdCopyBufferToImage command to transfer the data. This requires a command buffer and related objects.

    // A texture (sampled image) is stored in the GPU in an implementation-defined way, which may be completely different
    // to the layout of the texture on the disk/CPU-side.
    // For this reason, it is not possible to map its memory and write the data directly for that image.
    // Using the vkCmdCopyBufferToImage command in the second (uploading) step guarantees the correct
    // translation/swizzling of the texture data.

    // These steps are demonstrated below.

    // Set the width and height of the texture image.
    appManager.texture.textureDimensions.height = 256;
    appManager.texture.textureDimensions.width = 256;
    appManager.texture.data.resize(appManager.texture.textureDimensions.width * appManager.texture.textureDimensions.height * 4);

    // This function generates a texture pattern on-the-fly into a block of CPU-side memory: appManager.texture.data.
    generateTexture(appManager);

    // The BufferData struct has been defined in this application to hold the necessary data for the staging buffer.
    BufferData stagingBufferData;
    stagingBufferData.size = appManager.texture.data.size();

    // Use the buffer creation function to generate a staging buffer. The VK_BUFFER_USAGE_TRANSFER_SRC_BIT flag is passed to specify that the buffer
    // is going to be used as the source buffer of a transfer command.
    createBuffer(appManager, stagingBufferData, appManager.texture.data.data(), VK_BUFFER_USAGE_TRANSFER_SRC_BIT);

    // Create the image object.
    // The format is set to the most common format, R8G8B8_UNORM, 8-bits per channel, unsigned, and normalised.
    // Additionally, the dimensions of the image, the number of mipmap levels, the intended usage of the image, the number of samples per texel,
    // and whether this image can be accessed concurrently by multiple queue families are all also set here.
    // Some of the other parameters specified include the tiling and the initialLayout.
    // The tiling parameter determines the layout of texel blocks in memory. This should be set
    // to VK_IMAGE_TILING_OPTIMAL for images used as textures in shaders.
    // The initialLayout parameter is set to VK_IMAGE_LAYOUT_UNDEFINED but the layout will be transitioned
    // later using a barrier.
    VkImageCreateInfo imageInfo = {};
    imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
    imageInfo.imageType = VK_IMAGE_TYPE_2D;
    imageInfo.flags = 0;
    imageInfo.pNext = nullptr;
    imageInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
    imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    imageInfo.usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
    imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
    imageInfo.extent = { appManager.texture.textureDimensions.width, appManager.texture.textureDimensions.height, 1 };
    imageInfo.mipLevels = 1;
    imageInfo.arrayLayers = 1;

    debugAssertFunctionResult(vk::CreateImage(appManager.device, &imageInfo, nullptr, &appManager.texture.image), "Texture Image Creation");

    // Get the memory allocation requirements for the image.
    // These are used to allocate memory for the image that has just been created.
    VkMemoryRequirements memoryRequirments;
    vk::GetImageMemoryRequirements(appManager.device, appManager.texture.image, &memoryRequirments);

    // Populate a memory allocation info struct with the memory requirements size for the image.
    VkMemoryAllocateInfo allocateInfo = {};
    allocateInfo.pNext = nullptr;
    allocateInfo.memoryTypeIndex = 0;
    allocateInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
    allocateInfo.allocationSize = memoryRequirments.size;

    // This helper function queries available memory types to find memory with the features that are suitable for a sampled
    // image. Device Local memory is the preferred choice.
    getMemoryTypeFromProperties(appManager.deviceMemoryProperties, memoryRequirments.memoryTypeBits, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &(allocateInfo.memoryTypeIndex));

    // Use all of this information to allocate memory with the correct features for the image and bind the memory to the texture buffer.
    debugAssertFunctionResult(vk::AllocateMemory(appManager.device, &allocateInfo, nullptr, &appManager.texture.memory), "Texture Image Memory Allocation");
    debugAssertFunctionResult(vk::BindImageMemory(appManager.device, appManager.texture.image, appManager.texture.memory, 0), "Texture Image Memory Binding");

    // Specify the region which should be copied from the texture. In this case it is the entire image, so
    // the texture width and height are passed as extents.
    VkBufferImageCopy copyRegion = {};
    copyRegion.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    copyRegion.imageSubresource.mipLevel = 0;
    copyRegion.imageSubresource.baseArrayLayer = 0;
    copyRegion.imageSubresource.layerCount = 1;
    copyRegion.imageExtent.width = static_cast<uint32_t>(appManager.texture.textureDimensions.width);
    copyRegion.imageExtent.height = static_cast<uint32_t>(appManager.texture.textureDimensions.height);
    copyRegion.imageExtent.depth = 1;
    copyRegion.bufferOffset = 0;

    // Allocate a command buffer from the command pool. This command buffer will be used to execute the copy operation.
    // The allocation info struct below specifies that a single primary command buffer needs
    // to be allocated. Primary command buffers can be contrasted with secondary command buffers
    // which cannot be submitted directly to queues but instead are executed as part of a primary command
    // buffer.
    // The command pool referenced here was created in initCommandPoolAndBuffer().
    VkCommandBuffer commandBuffer;

    VkCommandBufferAllocateInfo commandAllocateInfo = {};
    commandAllocateInfo.commandPool = appManager.commandPool;
    commandAllocateInfo.pNext = nullptr;
    commandAllocateInfo.commandBufferCount = 1;
    commandAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    commandAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;

    debugAssertFunctionResult(vk::AllocateCommandBuffers(appManager.device, &commandAllocateInfo, &commandBuffer), "Allocate Command Buffers");

    // Begin recording the copy commands into the command buffer.
    VkCommandBufferBeginInfo commandBufferBeginInfo = {};
    commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    commandBufferBeginInfo.flags = 0;
    commandBufferBeginInfo.pNext = nullptr;
    commandBufferBeginInfo.pInheritanceInfo = nullptr;

    debugAssertFunctionResult(vk::BeginCommandBuffer(commandBuffer, &commandBufferBeginInfo), "Begin Image Copy to Staging Buffer Command Buffer Recording");

    // Specify the sub resource range of the image. In the case of this image, the parameters are default, with one mipmap level and layer,
    // because the image is very simple.
    VkImageSubresourceRange subResourceRange = {};
    subResourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    subResourceRange.baseMipLevel = 0;
    subResourceRange.levelCount = 1;
    subResourceRange.layerCount = 1;

    // A memory barrier needs to be created to make sure that the image layout is set up for a copy operation.
    // The barrier will transition the image layout from VK_IMAGE_LAYOUT_UNDEFINED to
    // VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL. This new layout is optimal for images which are the destination
    // of a transfer command.
    VkImageMemoryBarrier copyMemoryBarrier = {};
    copyMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    copyMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    copyMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    copyMemoryBarrier.image = appManager.texture.image;
    copyMemoryBarrier.subresourceRange = subResourceRange;
    copyMemoryBarrier.srcAccessMask = 0;
    copyMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

    // Use the pipeline barrier defined above.
    vk::CmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &copyMemoryBarrier);

    // Copy the staging buffer data to the image that was just created.
    vk::CmdCopyBufferToImage(commandBuffer, stagingBufferData.buffer, appManager.texture.image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

    // Create a barrier to make sure that the image layout is shader read-only.
    // This barrier will transition the image layout from VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL to
    // VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL.
    VkImageMemoryBarrier layoutMemoryBarrier = {};
    layoutMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    layoutMemoryBarrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
    layoutMemoryBarrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    layoutMemoryBarrier.image = appManager.texture.image;
    layoutMemoryBarrier.subresourceRange = subResourceRange;
    layoutMemoryBarrier.srcAccessMask = 0;
    layoutMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    // Use a pipeline barrier to change the image layout to be optimised for reading by the shader.
    vk::CmdPipelineBarrier(commandBuffer, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, VK_PIPELINE_STAGE_ALL_COMMANDS_BIT, 0, 0, nullptr, 0, nullptr, 1, &layoutMemoryBarrier);

    // End the recording of the command buffer.
    debugAssertFunctionResult(vk::EndCommandBuffer(commandBuffer), "End Image Copy to Staging Buffer Command Buffer Recording");

    // Create a fence object which will signal when all of the commands in this command buffer have been completed.
    VkFence copyFence;
    VkFenceCreateInfo copyFenceInfo = {};
    copyFenceInfo.flags = 0;
    copyFenceInfo.pNext = nullptr;
    copyFenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

    debugAssertFunctionResult(vk::CreateFence(appManager.device, &copyFenceInfo, nullptr, &copyFence), "Image Copy to Staging Buffer Fence Creation");

    // Finally, submit the command buffer to the graphics queue to get the GPU to perform the copy operations.
    // When submitting command buffers, it is possible to set wait and signal semaphores to control synchronisation. These
    // are not used here but they will be used later during rendering.
    VkSubmitInfo submitInfo = {};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.pNext = nullptr;
    submitInfo.pWaitDstStageMask = nullptr;
    submitInfo.waitSemaphoreCount = 0;
    submitInfo.pWaitSemaphores = nullptr;
    submitInfo.signalSemaphoreCount = 0;
    submitInfo.pSignalSemaphores = nullptr;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;

    debugAssertFunctionResult(vk::QueueSubmit(appManager.graphicQueue, 1, &submitInfo, copyFence), "Submit Image Copy to Staging Buffer Command Buffer");

    // Wait for the fence to be signalled. This ensures the command buffer has finished executing.
    debugAssertFunctionResult(vk::WaitForFences(appManager.device, 1, &copyFence, VK_TRUE, FENCE_TIMEOUT), "Image Copy to Staging Buffer Fence Signal");

    // After the image is complete and all the texture data has been copied, an image view needs to be created to make sure
    // that the API can understand what the image is. For example, information can be provided on the format or view type.
    // The image parameters used here are the same as for the swapchain images created earlier.
    VkImageViewCreateInfo imageViewInfo = {};
    imageViewInfo.flags = 0;
    imageViewInfo.pNext = nullptr;
    imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    imageViewInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
    imageViewInfo.image = appManager.texture.image;
    imageViewInfo.subresourceRange.layerCount = 1;
    imageViewInfo.subresourceRange.levelCount = 1;
    imageViewInfo.subresourceRange.baseArrayLayer = 0;
    imageViewInfo.subresourceRange.baseMipLevel = 0;
    imageViewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    imageViewInfo.components.r = VK_COMPONENT_SWIZZLE_R;
    imageViewInfo.components.g = VK_COMPONENT_SWIZZLE_G;
    imageViewInfo.components.b = VK_COMPONENT_SWIZZLE_B;
    imageViewInfo.components.a = VK_COMPONENT_SWIZZLE_A;

    debugAssertFunctionResult(vk::CreateImageView(appManager.device, &imageViewInfo, nullptr, &appManager.texture.view), "Texture Image View Creation");

    // Create a texture sampler.
    // The sampler will be needed to sample the texture data and pass
    // it to the fragment shader during the execution of the rendering phase.
    // The parameters specified below define any filtering or transformations which are applied before
    // passing the colour data to the fragment shader.
    // In this case, anisotropic filtering is turned off and if the fragment shader samples outside of the image co-ordinates
    // it will return the colour at the nearest edge of the image.
    VkSamplerCreateInfo samplerInfo = {};
    samplerInfo.flags = 0;
    samplerInfo.pNext = nullptr;
    samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
    samplerInfo.magFilter = VK_FILTER_LINEAR;
    samplerInfo.minFilter = VK_FILTER_LINEAR;
    samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.addressModeW = VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
    samplerInfo.anisotropyEnable = VK_FALSE;
    samplerInfo.maxAnisotropy = 1.0f;
    samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_TRANSPARENT_BLACK;
    samplerInfo.unnormalizedCoordinates = VK_FALSE;
    samplerInfo.compareEnable = VK_FALSE;
    samplerInfo.compareOp = VK_COMPARE_OP_ALWAYS;
    samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
    samplerInfo.mipLodBias = 0.0f;
    samplerInfo.minLod = 0.0f;
    samplerInfo.maxLod = 5.0f;

    debugAssertFunctionResult(vk::CreateSampler(appManager.device, &samplerInfo, nullptr, &appManager.texture.sampler), "Texture Sampler Creation");

    // Clean up all the temporary data created for this operation.
    vk::DestroyFence(appManager.device, copyFence, nullptr);
    vk::FreeCommandBuffers(appManager.device, appManager.commandPool, 1, &commandBuffer);
    vk::FreeMemory(appManager.device, stagingBufferData.memory, nullptr);
    vk::DestroyBuffer(appManager.device, stagingBufferData.buffer, nullptr);
}

#endif // VKTEXTURES_H
