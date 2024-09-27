#ifndef VKFENCES_H
#define VKFENCES_H

#include "vkStructs.h"

/// <summary>Creates a number of fences and semaphores which synchronise work on the CPU and GPU</summary>
inline void _initSemaphoreAndFence(AppManager& appManager)
{
    // Concept: Fences and Semaphores
    // Fences and semaphores are used to synchronise work on the CPU and GPU that share the same resources.
    // Fences are GPU to CPU syncs. They are signalled by the GPU, and can only be waited on by the CPU. They need to be reset manually.
    // Semaphores are GPU to GPU syncs, specifically used to sync queue submissions on the same or different queue. Again, they are signalled by
    // the GPU but are waited on by the GPU. They are reset after they are waited on.

    // This function creates two sets of semaphores and a single fence for each swapchain image.

    // The first semaphore will wait until the image has been acquired successfully from the
    // swapchain before signalling, the second semaphore will wait until the render has finished
    // on the image, and finally the fence will wait until the commands in the command
    // buffer have finished executing.

    // The semaphores are created with default parameters, but the fence is created with the flags parameter set to
    // VK_FENCE_CREATE_SIGNALED_BIT. This is because of the specific way this example is structured. The
    // application waits for this fence to be signalled before starting to draw the frame, however, on the first
    // frame there is no previous frame to trigger the fence, so it must be created in a signalled state.

    // All of the objects created here are stored in std::vectors. The individual semaphores and fences
    // will be accessed later with an index relating to the frame that is currently being rendered.
    for (uint32_t i = 0; i < appManager.swapChainImages.size(); ++i)
    {
        VkSemaphore acquireSemaphore;
        VkSemaphore renderSemaphore;

        VkFence frameFence;

        VkSemaphoreCreateInfo acquireSemaphoreInfo = {};
        acquireSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        acquireSemaphoreInfo.pNext = nullptr;
        acquireSemaphoreInfo.flags = 0;

        debugAssertFunctionResult(vk::CreateSemaphore(appManager.device, &acquireSemaphoreInfo, nullptr, &acquireSemaphore), "Acquire Semaphore creation");

        appManager.acquireSemaphore.emplace_back(acquireSemaphore);

        VkSemaphoreCreateInfo renderSemaphoreInfo = {};
        renderSemaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
        renderSemaphoreInfo.pNext = nullptr;
        renderSemaphoreInfo.flags = 0;

        debugAssertFunctionResult(vk::CreateSemaphore(appManager.device, &renderSemaphoreInfo, nullptr, &renderSemaphore), "Render Semaphore creation");

        appManager.presentSemaphores.emplace_back(renderSemaphore);

        VkFenceCreateInfo FenceInfo;
        FenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        FenceInfo.pNext = nullptr;
        FenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // Start the fence as signalled.

        debugAssertFunctionResult(vk::CreateFence(appManager.device, &FenceInfo, nullptr, &frameFence), "Fence Creation");

        appManager.frameFences.emplace_back(frameFence);
    }
}

#endif // VKFENCES_H
