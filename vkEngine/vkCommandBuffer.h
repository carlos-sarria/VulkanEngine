#ifndef VKCOMMANDBUFFER_H
#define VKCOMMANDBUFFER_H

#include "vkStructs.h"

/// <summary>Creates a command pool and then allocates out of it a number of command buffers equal to the number of swapchain images</summary>
inline void _initCommandPoolAndBuffer(AppManager& appManager)
{
    // This function creates a command pool to reserve memory for the command buffers are created to execute commands.
    // After the command pool is created, command buffers are allocated from it. A number of command buffers equal to
    // the number of images in the swapchain are needed, assuming the command buffers are used for rendering.

    // Populate a command pool info struct with the queue family that will be used and the intended usage behaviour of command buffers
    // that can be allocated out of it.
    VkCommandPoolCreateInfo commandPoolInfo = {};
    commandPoolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    commandPoolInfo.pNext = nullptr;
    commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    commandPoolInfo.queueFamilyIndex = appManager.graphicsQueueFamilyIndex;

    // Create the actual command pool.
    debugAssertFunctionResult(vk::CreateCommandPool(appManager.device, &commandPoolInfo, nullptr, &appManager.commandPool), "Command Pool Creation");

    // Resize the vector to have a number of elements equal to the number of swapchain images.
    appManager.cmdBuffers.resize(appManager.swapChainImages.size());

    // Populate a command buffer info struct with a reference to the command pool from which the memory for the command buffer is taken.
    // Notice the "level" parameter which ensures these will be primary command buffers.
    VkCommandBufferAllocateInfo commandBufferAllocateInfo = {};
    commandBufferAllocateInfo.pNext = nullptr;
    commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    commandBufferAllocateInfo.commandPool = appManager.commandPool;
    commandBufferAllocateInfo.commandBufferCount = static_cast<uint32_t>(appManager.cmdBuffers.size());
    commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;

    // Allocate the command buffers from the command pool.
    debugAssertFunctionResult(vk::AllocateCommandBuffers(appManager.device, &commandBufferAllocateInfo, appManager.cmdBuffers.data()), "Command Buffer Creation");
}

/// <summary>Records rendering commands to the command buffers</summary>
inline void _recordCommandBuffer(AppManager& appManager)
{
    // Concept: Command Buffers
    // Command buffers are containers that contain GPU commands. They are passed to the queues to be executed on the device.
    // Each command buffer when executed performs a different task. For instance, the command buffer required to render an object is
    // recorded before the rendering. When the rendering stage of the application is reached, the command buffer is submitted to execute its tasks.

    // This function will record a set of commands in the command buffers which will render a basic triangle on screen.

    // State the clear values for rendering.
    // This is the colour value that the framebuffer is cleared to at the start of the render pass.
    // The framebuffer is cleared because, during render pass creation, the loadOp parameter was set to VK_LOAD_OP_CLEAR. Remember
    // that this is crucial as it can reduce system memory bandwidth and reduce power consumption, particularly on PowerVR platforms.
    VkClearValue clearColor = { 0.00f, 0.70f, 0.67f, 1.0f };

    // This is a constant offset which specifies where the vertex data starts in the vertex
    // buffer. In this case the data just starts at the beginning of the buffer.
    const VkDeviceSize vertexOffsets[1] = { 0 };

    // Iterate through each created command buffer to record to it.
    for (size_t i = 0; i < appManager.cmdBuffers.size(); ++i)
    {
        // Reset the buffer to its initial state.
        debugAssertFunctionResult(vk::ResetCommandBuffer(appManager.cmdBuffers[i], 0), "Command Buffer Reset");

        // Begin the command buffer.
        VkCommandBufferBeginInfo cmd_begin_info = {};
        cmd_begin_info.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        cmd_begin_info.pNext = nullptr;
        cmd_begin_info.flags = 0;
        cmd_begin_info.pInheritanceInfo = nullptr;

        debugAssertFunctionResult(vk::BeginCommandBuffer(appManager.cmdBuffers[i], &cmd_begin_info), "Command Buffer Recording Started.");

        // Start recording commands.
        // In Vulkan, commands are recorded by calling vkCmd... functions.
        // Set the viewport and scissor to previously defined values.
        vk::CmdSetViewport(appManager.cmdBuffers[i], 0, 1, &appManager.viewport);

        vk::CmdSetScissor(appManager.cmdBuffers[i], 0, 1, &appManager.scissor);

        // Begin the render pass.
        // The render pass and framebuffer instances are passed here, along with the clear colour value and the extents of
        // the rendering area. VK_SUBPASS_CONTENTS_INLINE means that the subpass commands will be recorded here. The alternative is to
        // record them in isolation in a secondary command buffer and then record them here with vkCmdExecuteCommands.
        VkRenderPassBeginInfo renderPassInfo = {};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.pNext = nullptr;
        renderPassInfo.renderPass = appManager.renderPass;
        renderPassInfo.framebuffer = appManager.frameBuffers[i];
        renderPassInfo.clearValueCount = 1;
        renderPassInfo.pClearValues = &clearColor;
        renderPassInfo.renderArea.extent = appManager.swapchainExtent;
        renderPassInfo.renderArea.offset.x = 0;
        renderPassInfo.renderArea.offset.y = 0;

        vk::CmdBeginRenderPass(appManager.cmdBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // Bind the pipeline to the command buffer.
        vk::CmdBindPipeline(appManager.cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, appManager.pipeline);

        // A single large uniform buffer object is being used to hold all of the transformation matrices
        // associated with the swapchain images. It is for this reason that only a single descriptor set is
        // required for all of the frames.
        const VkDescriptorSet descriptorSet[] = { appManager.staticDescSet, appManager.dynamicDescSet };

        // An offset is used to select each slice of the uniform buffer object that contains the transformation
        // matrix related to each swapchain image.
        // Calculate the offset into the uniform buffer object for the current slice.
        uint32_t offset = static_cast<uint32_t>(appManager.dynamicUniformBufferData.bufferInfo.range * i);

        // Bind the descriptor sets. The &offset parameter is the offset into the dynamic uniform buffer which is
        // contained within the dynamic descriptor set.
        vk::CmdBindDescriptorSets(appManager.cmdBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, appManager.pipelineLayout, 0, NUM_DESCRIPTOR_SETS, descriptorSet, 1, &offset);

        // Bind the vertex buffer.
        vk::CmdBindVertexBuffers(appManager.cmdBuffers[i], 0, 1, &appManager.vertexBuffer.buffer, vertexOffsets);

        // Draw three vertices.
        vk::CmdDraw(appManager.cmdBuffers[i], appManager.vertexCount, 1, 0, 0);

        // End the render pass.
        vk::CmdEndRenderPass(appManager.cmdBuffers[i]);

        // End the command buffer recording process.
        debugAssertFunctionResult(vk::EndCommandBuffer(appManager.cmdBuffers[i]), "Command Buffer Recording Ended.");

        // At this point the command buffer is ready to be submitted to a queue with all of the recorded operations executed
        // asynchronously after that. A command buffer can, and if possible should, be executed multiple times, unless
        // it is allocated with the VK_COMMAND_BUFFER_ONE_TIME_USE bit.
        // The command buffers recorded here will be reused across the lifetime of the application.
    }
}


#endif // VKCOMMANDBUFFER_H
