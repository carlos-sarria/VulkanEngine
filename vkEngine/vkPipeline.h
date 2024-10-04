#ifndef VKPIPELINE_H
#define VKPIPELINE_H

#include "vkStructs.h"

/// <summary>Creates the graphics pipeline</summary>
inline void _initPipeline(AppManager& appManager)
{
    // Concept: Pipelines
    // A pipeline is a collection of stages in the rendering or compute process. Each stage processes data and passes it on to the next stage.
    // In Vulkan, there are two types of pipelines: graphics and compute.
    // The graphics pipeline is used for rendering operations, and the compute pipeline allows the application to perform computational work such as physics calculations.
    // With Vulkan, the pipeline is stored in one object that is immutable; therefore each object that needs to be rendered will potentially use a different pipeline.
    // The pipeline in Vulkan needs to be prepared before the its use. This helps with increasing the performance of the application.

    // There are a lot of parameters to be populated in the graphics pipeline. Each of the structs below will configure a different aspect of the pipeline and will be referenced
    // by the final pipeline creation struct.

    // This is the description of the vertex buffers that will be bound, in this case it is just one.
    // The stride variable set here is the distance, in bytes, between consecutive vertices. The input rate
    // specifies at what rate vertex attributes are pulled from the vertex buffer. It can be set to: per instance or per vertex.
    VkVertexInputBindingDescription vertexInputBindingDescription = {};
    vertexInputBindingDescription.binding = 0;
    vertexInputBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
    vertexInputBindingDescription.stride = sizeof(Vertex);

    // This is the description of the vertex attributes for the vertex input.
    // The location variable sets which vertex attribute to use. In this case there are two attributes: one for
    // position co-ordinates and one for the texture co-ordinates.
    // The offset variable specifies at what memory location within each vertex the attribute is found, and the format
    // parameter describes how the data is stored in each attribute.
    VkVertexInputAttributeDescription vertexInputAttributeDescription[2];
    vertexInputAttributeDescription[0].binding = 0;
    vertexInputAttributeDescription[0].format = VK_FORMAT_R32G32B32A32_SFLOAT;
    vertexInputAttributeDescription[0].location = 0;
    vertexInputAttributeDescription[0].offset = 0;

    vertexInputAttributeDescription[1].binding = 0;
    vertexInputAttributeDescription[1].format = VK_FORMAT_R32G32_SFLOAT;
    vertexInputAttributeDescription[1].location = 1;
    vertexInputAttributeDescription[1].offset = 4 * sizeof(float);

    // Combine the vertex bindings and the vertex attributes into the vertex input. This sums up all of the information about the vertices.
    VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.pVertexBindingDescriptions = &vertexInputBindingDescription;
    vertexInputInfo.vertexAttributeDescriptionCount = sizeof(vertexInputAttributeDescription) / sizeof(vertexInputAttributeDescription[0]);
    vertexInputInfo.pVertexAttributeDescriptions = vertexInputAttributeDescription;

    // Declare and populate the input assembly info struct.
    // This describes how the pipeline should handle the incoming vertex data. In
    // this case the pipeline will form triangles from the incoming vertices.
    // Additionally, an index buffer is not being used so primitive restart is not required.
    VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo = {};
    inputAssemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssemblyInfo.flags = 0;
    inputAssemblyInfo.pNext = nullptr;
    inputAssemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssemblyInfo.primitiveRestartEnable = VK_FALSE;

    // Define the rasterizer.
    // Here the rasterizer is set to fill the polygons with fragments, cull back faces, define the front face
    // by the clockwise winding direction and not use any depth bias.
    VkPipelineRasterizationStateCreateInfo rasterizationInfo = {};
    rasterizationInfo.pNext = nullptr;
    rasterizationInfo.flags = 0;
    rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizationInfo.cullMode = VK_CULL_MODE_NONE; // VK_CULL_MODE_BACK_BIT;
    rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
    rasterizationInfo.lineWidth = 1.0f;
    rasterizationInfo.depthBiasClamp = 0.0f;
    rasterizationInfo.depthBiasConstantFactor = 0.0f;
    rasterizationInfo.depthBiasEnable = VK_FALSE;
    rasterizationInfo.depthBiasSlopeFactor = 0.0f;
    rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;

    // This colour blend attachment state will be used by the colour blend info.
    // Only a single colour blend attachment is required because the render pass only has
    // one attachment.
    // No blending is needed so existing fragment values will be overwritten with incoming ones.
    VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
    colorBlendAttachment.colorWriteMask = 0xf;
    colorBlendAttachment.blendEnable = VK_FALSE;
    colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
    colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
    colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
    colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;

    // Populate the colour blend info struct required by the pipeline.
    VkPipelineColorBlendStateCreateInfo colorBlendInfo = {};
    colorBlendInfo.flags = 0;
    colorBlendInfo.pNext = nullptr;
    colorBlendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlendInfo.logicOp = VK_LOGIC_OP_COPY;
    colorBlendInfo.logicOpEnable = VK_FALSE;
    colorBlendInfo.attachmentCount = 1;
    colorBlendInfo.pAttachments = &colorBlendAttachment;
    colorBlendInfo.blendConstants[0] = 0.0f;
    colorBlendInfo.blendConstants[1] = 0.0f;
    colorBlendInfo.blendConstants[2] = 0.0f;
    colorBlendInfo.blendConstants[3] = 0.0f;

    // Populate the multisampling info struct. Multisampling is not needed.
    VkPipelineMultisampleStateCreateInfo multisamplingInfo = {};
    multisamplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisamplingInfo.pNext = nullptr;
    multisamplingInfo.flags = 0;
    multisamplingInfo.pSampleMask = nullptr;
    multisamplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
    multisamplingInfo.sampleShadingEnable = VK_FALSE;
    multisamplingInfo.alphaToCoverageEnable = VK_FALSE;
    multisamplingInfo.alphaToOneEnable = VK_FALSE;
    multisamplingInfo.minSampleShading = 0.0f;

    // Create a list of dynamic states that will be used.
    // memset is used to initialise the block of memory pointed to by dynamicState to 0.
    VkDynamicState dynamicState[16]; // Number of elements in enum VkDynamicState
    memset(dynamicState, 0, sizeof(dynamicState));

    // Declare and populate the dynamic state info struct.
    VkPipelineDynamicStateCreateInfo dynamicStateInfo = {};
    dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicStateInfo.dynamicStateCount = 0;
    dynamicStateInfo.pNext = nullptr;
    dynamicStateInfo.pDynamicStates = dynamicState;

    // Populate a viewport state creation struct.
    VkPipelineViewportStateCreateInfo viewportInfo = {};
    viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportInfo.pNext = nullptr;
    viewportInfo.flags = 0;

    // Add the viewport and scissor as dynamic states in the dynamic state info struct.
    viewportInfo.viewportCount = 1;
    dynamicState[dynamicStateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_VIEWPORT;
    viewportInfo.pViewports = &appManager.viewport;

    viewportInfo.scissorCount = 1;
    dynamicState[dynamicStateInfo.dynamicStateCount++] = VK_DYNAMIC_STATE_SCISSOR;
    viewportInfo.pScissors = &appManager.scissor;

    // Create a list of the descriptor set layouts.
    // This were created earlier in initDescriptorPoolAndSet().
    VkDescriptorSetLayout descriptorSetLayout[] = { appManager.staticDescriptorSetLayout, appManager.dynamicDescriptorSetLayout };

    // Create the pipeline layout from the descriptor set layouts.
    VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    pipelineLayoutInfo.setLayoutCount = 2; // The count of the descriptors is already known.
    pipelineLayoutInfo.pSetLayouts = descriptorSetLayout; // Add them to the pipeline layout info struct.
    pipelineLayoutInfo.pushConstantRangeCount = 0;
    pipelineLayoutInfo.pPushConstantRanges = nullptr;

    debugAssertFunctionResult(vk::CreatePipelineLayout(appManager.device, &pipelineLayoutInfo, nullptr, &appManager.pipelineLayout), "Pipeline Layout Creation");

    // Create the pipeline by putting all of these elements together.
    VkGraphicsPipelineCreateInfo pipelineInfo;
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.pNext = nullptr;
    pipelineInfo.layout = appManager.pipelineLayout;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
    pipelineInfo.basePipelineIndex = 0;
    pipelineInfo.flags = 0;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssemblyInfo;
    pipelineInfo.pRasterizationState = &rasterizationInfo;
    pipelineInfo.pColorBlendState = &colorBlendInfo;
    pipelineInfo.pTessellationState = nullptr;
    pipelineInfo.pMultisampleState = &multisamplingInfo;
    pipelineInfo.pDynamicState = &dynamicStateInfo;
    pipelineInfo.pViewportState = &viewportInfo;
    pipelineInfo.pDepthStencilState = nullptr;
    pipelineInfo.pStages = appManager.shaderStages;
    pipelineInfo.stageCount = 2;
    pipelineInfo.renderPass = appManager.renderPass;
    pipelineInfo.subpass = 0;

    debugAssertFunctionResult(vk::CreateGraphicsPipelines(appManager.device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &appManager.pipeline), "Pipeline Creation");
}


#endif // VKPIPELINE_H
