#ifndef VKCLOSEDOWN_H
#define VKCLOSEDOWN_H

#include "vkStructs.h"

inline void _closeDown(AppManager& appManager)
{
    // This function ensures that all the objects that were created are cleaned up correctly and nothing
    // is left "open" when the application is closed.

    // Wait for the device to have finished all operations before starting the clean up.
    debugAssertFunctionResult(vk::DeviceWaitIdle(appManager.device), "Device Wait for Idle");

    // Destroy the fence used to sync work between the CPU and GPU.
    vk::WaitForFences(appManager.device, static_cast<uint32_t>(appManager.frameFences.size()), appManager.frameFences.data(), true, uint64_t(-1));
    vk::ResetFences(appManager.device, static_cast<uint32_t>(appManager.frameFences.size()), appManager.frameFences.data());
    for (auto& fence : appManager.frameFences) { vk::DestroyFence(appManager.device, fence, nullptr); }

    // Destroy the semaphores used for image acquisition and rendering.
    for (auto& semaphore : appManager.acquireSemaphore) { vk::DestroySemaphore(appManager.device, semaphore, nullptr); }

    for (auto& semaphore : appManager.presentSemaphores) { vk::DestroySemaphore(appManager.device, semaphore, nullptr); }

    // Free the memory allocated for the descriptor sets.
    for(int i=0; i<appManager.textures.size(); i++)
        vk::FreeDescriptorSets(appManager.device, appManager.descriptorPool, 1, &appManager.staticDescSet[i]);
    vk::FreeDescriptorSets(appManager.device, appManager.descriptorPool, 1, &appManager.dynamicDescSet);

    // Destroy both the descriptor layouts and descriptor pool.
    vk::DestroyDescriptorSetLayout(appManager.device, appManager.staticDescriptorSetLayout, nullptr);
    vk::DestroyDescriptorSetLayout(appManager.device, appManager.dynamicDescriptorSetLayout, nullptr);
    vk::DestroyDescriptorPool(appManager.device, appManager.descriptorPool, nullptr);

    // Destroy the uniform buffer and free the memory.
    vk::DestroyBuffer(appManager.device, appManager.dynamicUniformBufferData.buffer, nullptr);
    vk::FreeMemory(appManager.device, appManager.dynamicUniformBufferData.memory, nullptr);

    // Destroy the pipeline followed by the pipeline layout.
    vk::DestroyPipeline(appManager.device, appManager.pipeline, nullptr);
    vk::DestroyPipelineLayout(appManager.device, appManager.pipelineLayout, nullptr);

    for(auto &texture : appManager.textures)
    {
        // Destroy the texture image.
        vk::DestroyImage(appManager.device, texture.image, nullptr);

        // Destroy the texture image view.
        vk::DestroyImageView(appManager.device, texture.view, nullptr);

        // Free the memory allocated for the texture.
        vk::FreeMemory(appManager.device, texture.memory, nullptr);

        // Destroy the sampler.
        vk::DestroySampler(appManager.device, texture.sampler, nullptr);
    }

    // Destroy then free the memory for the vertex buffer.
    for (Mesh m : appManager.meshes)
    {
        vk::DestroyBuffer(appManager.device, m.vertexBuffer.buffer, nullptr);
        vk::FreeMemory(appManager.device, m.vertexBuffer.memory, nullptr);
        vk::DestroyBuffer(appManager.device, m.indexBuffer.buffer, nullptr);
        vk::FreeMemory(appManager.device, m.indexBuffer.memory, nullptr);
    }

    // Iterate through each of the framebuffers and destroy them.
    for (uint32_t i = 0; i < appManager.frameBuffers.size(); i++) { vk::DestroyFramebuffer(appManager.device, appManager.frameBuffers[i], nullptr); }

    // Destroy the two shader modules - vertex and fragment.
    vk::DestroyShaderModule(appManager.device, appManager.shaderStages[0].module, nullptr);
    vk::DestroyShaderModule(appManager.device, appManager.shaderStages[1].module, nullptr);

    // Destroy the render pass.
    vk::DestroyRenderPass(appManager.device, appManager.renderPass, nullptr);

    // Clean up the swapchain image views.
    for (auto& imagebuffers : appManager.swapChainImages) { vk::DestroyImageView(appManager.device, imagebuffers.view, nullptr); vk::DestroyImageView(appManager.device, imagebuffers.depth_view, nullptr);}

    // Free the depth buffer memory
    //vk::FreeMemory(appManager.device, appManager.depth_memory, nullptr);

    // Free the allocated memory in the command buffers.
    vk::FreeCommandBuffers(appManager.device, appManager.commandPool, static_cast<uint32_t>(appManager.cmdBuffers.size()), appManager.cmdBuffers.data());

    // Destroy the command pool.
    vk::DestroyCommandPool(appManager.device, appManager.commandPool, nullptr);

    // Clean up the swapchain.
    vk::DestroySwapchainKHR(appManager.device, appManager.swapchain, nullptr);

    // Clean up the surface.
    vk::DestroySurfaceKHR(appManager.instance, appManager.surface, nullptr);

    // Destroy the logical device.
    vk::DestroyDevice(appManager.device, nullptr);
}

#endif // VKCLOSEDOWN_H
