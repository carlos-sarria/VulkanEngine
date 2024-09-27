#ifndef VKRENDERPASS_H
#define VKRENDERPASS_H

#include "vkStructs.h"

/// <summary>Creates a render pass object and defines its properties.</summary>
inline void _initRenderPass(AppManager& appManager)
{
    // Concept: Render passes
    // In Vulkan, a render pass is a collection of data that describes a set of framebuffer
    // attachments that are needed for rendering. A render pass is composed of subpasses that
    // order the data. A render pass collects all the colour, depth, and stencil attachments,
    // making sure to explicitly define them so that the driver does not have to work them out for itself.

    // This function creates a render pass object using the descriptions of a colour attachment and a subpass.

    // Create a description of the colour attachment that will be added to the render pass.
    // This will tell the render pass what to do with the image (framebuffer) before, during, and after rendering.
    // In this case the contents of the image will be cleared at the start of the subpass and stored at the
    // end.
    // Additionally, this description tells Vulkan that only one sample per pixel will be allowed for this image and the pixel layout will
    // be transitioned to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR during the render pass. This layout is used
    // when an image is going to be presented to a surface.
    VkAttachmentDescription colorAttachmentDescription = {};
    colorAttachmentDescription.format = appManager.surfaceFormat.format;
    colorAttachmentDescription.flags = 0;
    colorAttachmentDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    colorAttachmentDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
    colorAttachmentDescription.samples = VK_SAMPLE_COUNT_1_BIT;
    colorAttachmentDescription.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
    colorAttachmentDescription.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
    colorAttachmentDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
    colorAttachmentDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

    // Create a colour attachment reference.
    // This tells the implementation that the first attachment at index 0 of this render pass will be a colour attachment.
    VkAttachmentReference colorAttachmentReference = {};
    colorAttachmentReference.attachment = 0;
    colorAttachmentReference.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

    // Declare and populate a struct which contains a description of the subpass.
    // In this case the subpass only has a single colour attachment and will support a graphics pipeline.
    VkSubpassDescription subpassDescription = {};
    subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
    subpassDescription.flags = 0;
    subpassDescription.colorAttachmentCount = 1;
    subpassDescription.pColorAttachments = &colorAttachmentReference;
    subpassDescription.pDepthStencilAttachment = nullptr;
    subpassDescription.pInputAttachments = nullptr;
    subpassDescription.inputAttachmentCount = 0;
    subpassDescription.preserveAttachmentCount = 0;
    subpassDescription.pPreserveAttachments = nullptr;
    subpassDescription.pResolveAttachments = nullptr;

    VkSubpassDependency subpassDependencies[2];
    subpassDependencies[0] = {};
    subpassDependencies[0].srcSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[0].dstSubpass = 0;
    subpassDependencies[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[0].srcAccessMask = 0;
    subpassDependencies[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    subpassDependencies[1] = {};
    subpassDependencies[1].srcSubpass = 0;
    subpassDependencies[1].dstSubpass = VK_SUBPASS_EXTERNAL;
    subpassDependencies[1].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
    subpassDependencies[1].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
    subpassDependencies[1].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
    subpassDependencies[1].dstAccessMask = 0;
    subpassDependencies[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;

    // Populate a render pass creation info struct.
    // Again, this simply references the single colour attachment and subpass.
    VkRenderPassCreateInfo renderPassInfo = {};
    renderPassInfo.attachmentCount = 1;
    renderPassInfo.flags = 0;
    renderPassInfo.pNext = nullptr;
    renderPassInfo.subpassCount = 1;
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
    renderPassInfo.pAttachments = &colorAttachmentDescription;
    renderPassInfo.pSubpasses = &subpassDescription; // the subpass that was just created.
    renderPassInfo.pDependencies = subpassDependencies;
    renderPassInfo.dependencyCount = 2;

    // Depth or stencil buffers are not needed since this application is simply rendering a
    // triangle with no depth testing.

    // Create the render pass object itself.
    debugAssertFunctionResult(vk::CreateRenderPass(appManager.device, &renderPassInfo, nullptr, &appManager.renderPass), "Render pass Creation");
}


#endif // VKRENDERPASS_H
