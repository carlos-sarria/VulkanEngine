/*!*********************************************************************************************************************
\File         VulkanHelloAPI.cpp
\Title        Vulkan HelloAPI
\Author       PowerVR by Imagination, Developer Technology Team.
\Copyright    Copyright(c) Imagination Technologies Limited.
\brief        Build an introductory Vulkan application to show the process of getting started with Vulkan.
***********************************************************************************************************************/
#include "VulkanEngine.h"
#include "VertShader.h"
#include "FragShader.h"

#include "vkLayers.h"
#include "vkExtensions.h"
#include "vkInit.h"
#include "vkQueues.h"
#include "vkSwapChain.h"
#include "vkMemory.h"
#include "vkTextures.h"

/// <summary>Creates the vertex and fragment shader modules and loads in compiled SPIR-V code</summary>
void VulkanHelloAPI::initShaders()
{
	// In Vulkan, shaders are in SPIR-V format which is a byte-code format rather than a human-readable one.
	// SPIR-V can be used for both graphical and compute operations.
	// This function loads the compiled source code (see vertshader.h and fragshader.h) and creates shader modules that are going
	// to be used by the pipeline later on.

	createShaderModule(spv_VertShader_bin, sizeof(spv_VertShader_bin), 0, VK_SHADER_STAGE_VERTEX_BIT);

	createShaderModule(spv_FragShader_bin, sizeof(spv_FragShader_bin), 1, VK_SHADER_STAGE_FRAGMENT_BIT);
}

/// <summary>Creates a render pass object and defines its properties.</summary>
void VulkanHelloAPI::initRenderPass()
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

/// <summary>Creates the uniform buffers used throughout the demo</summary>
void VulkanHelloAPI::initUniformBuffers()
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
		bufferDataSizePerSwapchain = static_cast<uint32_t>(getAlignedDataSize(bufferDataSizePerSwapchain, minimumUboAlignment));

		// Calculate the size of the dynamic uniform buffer.
		// This buffer will be updated on each frame and must therefore be multi-buffered to avoid issues with using partially updated data, or updating data already in use.
		// Rather than allocating multiple (swapchain) buffers, a larger buffer is allocated and a slice of this buffer will be used per swapchain. This works as
		// long as the buffer is created taking into account the minimum uniform buffer offset alignment.
		appManager.dynamicUniformBufferData.size = bufferDataSizePerSwapchain * appManager.swapChainImages.size();

		// Create the buffer, allocate the device memory, and attach the memory to the newly created buffer object.
        createBuffer(appManager, appManager.dynamicUniformBufferData, nullptr, usageFlags);
		appManager.dynamicUniformBufferData.bufferInfo.range = bufferDataSizePerSwapchain;

		// Note that only memory created with the memory property flag VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT can be mapped.
		// vkMapMemory retrieves a host virtual address pointer to a region of a mappable memory object.
		debugAssertFunctionResult(vk::MapMemory(appManager.device, appManager.dynamicUniformBufferData.memory, 0, appManager.dynamicUniformBufferData.size, 0,
									  &appManager.dynamicUniformBufferData.mappedData),
			"Could not map the uniform buffer.");
	}
}

/// <summary>Defines the vertices of a simple triangle which can be passed to the vertex shader to be rendered on screen</summary>
void VulkanHelloAPI::initVertexBuffers()
{
	// This function defines the vertices of a simple triangle and creates a vertex buffer to hold this data.

	// Calculate the size of the vertex buffer to be passed to the vertex shader.
	appManager.vertexBuffer.size = sizeof(Vertex) * 3;

	// Set the values for the triangle's vertices.
	Vertex triangle[3];
	triangle[0] = { -0.5f, -0.288f, 0.0f, 1.0f, 0.0f, 0.0f };
	triangle[1] = { .5f, -.288f, 0.0f, 1.0f, 1.0f, 0.0f };
	triangle[2] = { 0.0f, .577f, 0.0f, 1.0f, 0.5f, 1.0f };

	// Create the buffer that will hold the data and be passed to the shaders.
    createBuffer(appManager, appManager.vertexBuffer, reinterpret_cast<uint8_t*>(triangle), VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}


/// <summary>Creates a static and dynamic descriptor set</summary>
void VulkanHelloAPI::initDescriptorPoolAndSet()
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
	descriptorPoolInfo.poolSizeCount = 2;
	descriptorPoolInfo.pPoolSizes = descriptorPoolSize;
	descriptorPoolInfo.maxSets = 2;

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

	// Allocate the texture image descriptor set.
	// The allocation struct variable is updated to point to the layout of the texture image descriptor set.
	descriptorAllocateInfo.pSetLayouts = &appManager.staticDescriptorSetLayout;
	debugAssertFunctionResult(vk::AllocateDescriptorSets(appManager.device, &descriptorAllocateInfo, &appManager.staticDescSet), "Descriptor Set Creation");

	// This information references the texture sampler that will be passed to the shaders by way of
	// the descriptor set. The sampler determines how the pixel data of the texture image will be
	// sampled and how it will be passed to the fragment shader. It also contains the actual image
	// object (via its image view) and the image layout.
	// This image layout is optimised for read-only access by shaders. The image was transitioned to
	// this layout using a memory barrier in initTexture().
	VkDescriptorImageInfo descriptorImageInfo = {};
	descriptorImageInfo.sampler = appManager.texture.sampler;
	descriptorImageInfo.imageView = appManager.texture.view;
	descriptorImageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

	// Update the descriptor sets with the actual objects, in this case the texture image and the uniform buffer.
	// These structs specify which descriptor sets are going to be updated and hold a pointer to the actual objects.
	VkWriteDescriptorSet descriptorSetWrite[2] = {};

	descriptorSetWrite[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorSetWrite[0].pNext = nullptr;
	descriptorSetWrite[0].dstSet = appManager.staticDescSet;
	descriptorSetWrite[0].descriptorCount = 1;
	descriptorSetWrite[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorSetWrite[0].pImageInfo = &descriptorImageInfo; // Pass image object
	descriptorSetWrite[0].dstArrayElement = 0;
	descriptorSetWrite[0].dstBinding = 0;
	descriptorSetWrite[0].pBufferInfo = nullptr;
	descriptorSetWrite[0].pTexelBufferView = nullptr;

	descriptorSetWrite[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorSetWrite[1].pNext = nullptr;
	descriptorSetWrite[1].dstSet = appManager.dynamicDescSet;
	descriptorSetWrite[1].descriptorCount = 1;
	descriptorSetWrite[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorSetWrite[1].pBufferInfo = &appManager.dynamicUniformBufferData.bufferInfo; // Pass uniform buffer to this function.
	descriptorSetWrite[1].dstArrayElement = 0;
	descriptorSetWrite[1].dstBinding = 0;

	vk::UpdateDescriptorSets(appManager.device, 2, descriptorSetWrite, 0, nullptr);
}

/// <summary>Creates the graphics pipeline</summary>
void VulkanHelloAPI::initPipeline()
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
	rasterizationInfo.cullMode = VK_CULL_MODE_BACK_BIT;
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

/// <summary>Creates a number of framebuffer objects equal to the number of images in the swapchain</summary>
void VulkanHelloAPI::initFrameBuffers()
{
	// Concept: Framebuffers
	// In Vulkan, all the attachments used by the render pass are defined in framebuffers. Each frame in a framebuffer defines
	// the attachments related to it. This includes the textures (including the colour and depth / stencil attachments) and
	// the input attachments. This way of separating descriptions in render passes and definitions in framebuffers gives the option
	// of using different render passes with different framebuffers. However, the degree of flexibility with which this can be done is based on the
	// compatibility of the two.

	// This function creates a framebuffer for each swapchain image.

	// This is a placeholder handle for the attachment which will be stored in the VkFramebufferCreateInfo.
	VkImageView attachment = VK_NULL_HANDLE;

	// Populate a framebuffer info struct with the information that is needed to create the framebuffers. This includes its dimensions, its attachments, and the associated render
	// pass that will use the specified attachments. The attachment member will be a null variable for now.
	VkFramebufferCreateInfo frameBufferInfo = {};
	frameBufferInfo.flags = 0;
	frameBufferInfo.pNext = nullptr;
	frameBufferInfo.attachmentCount = 1;
	frameBufferInfo.height = appManager.swapchainExtent.height;
	frameBufferInfo.width = appManager.swapchainExtent.width;
	frameBufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
	frameBufferInfo.renderPass = appManager.renderPass;
	frameBufferInfo.pAttachments = &attachment;
	frameBufferInfo.layers = 1;

	// Resize the vector which will contain all of the framebuffers based on the number of images in the swap chain.
	appManager.frameBuffers.resize(appManager.swapChainImages.size());

	// Create as many framebuffer objects as swapchain images and assign each image to a framebuffer.
	// Note that, above, the pAttachments variable has been assigned to the address of the
	// local variable "attachment". Every time pAttachments is reassigned a new image is used during framebuffer creation.
	for (size_t i = 0; i < appManager.swapChainImages.size(); ++i)
	{
		attachment = appManager.swapChainImages[i].view;

		debugAssertFunctionResult(vk::CreateFramebuffer(appManager.device, &frameBufferInfo, nullptr, &appManager.frameBuffers[i]), "Swapchain Frame buffer creation");
	}
}

/// <summary>Creates a command pool and then allocates out of it a number of command buffers equal to the number of swapchain images</summary>
void VulkanHelloAPI::initCommandPoolAndBuffer()
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

/// <summary>Sets up the view port and also sets up the scissor</summary>
void VulkanHelloAPI::initViewportAndScissor()
{
	// The viewport is essentially the dimensions of the rendering area and
	// the scissor is a sub-section of this viewport which is actually stored.

	// Viewport and scissors are set dynamically with vkCmdSetViewport and vkCmdSetScissor.

	// This code sets up the values that will be used by these commands. In this example,
	// the extents of the scissor are the same as the viewport.

	// Set the viewport dimensions, depth, and starting coordinates.
	appManager.viewport.width = surfaceData.width;
	appManager.viewport.height = surfaceData.height;
	appManager.viewport.minDepth = 0.0f;
	appManager.viewport.maxDepth = 1.0f;
	appManager.viewport.x = 0;
	appManager.viewport.y = 0;

	// Set the extent to the dimensions of the surface and set the offset in both directions to 0.
	appManager.scissor.extent.width = static_cast<uint32_t>(surfaceData.width);
	appManager.scissor.extent.height = static_cast<uint32_t>(surfaceData.height);
	appManager.scissor.offset.x = 0;
	appManager.scissor.offset.y = 0;

	// The viewport and scissor are now ready to be set.
}

/// <summary>Creates a number of fences and semaphores which synchronise work on the CPU and GPU</summary>
void VulkanHelloAPI::initSemaphoreAndFence()
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


/// <summary>Creates a dynamic uniform buffer and allocates its memory</summary>
/// <param name="inBuffer">Vkbuffer handle in which the newly-created buffer object is returned</param>
void VulkanHelloAPI::createDynamicUniformBuffer(BufferData& inBuffer)
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
		bool pass = getMemoryTypeFromProperties(appManager.deviceMemoryProperties, memoryRequirments.memoryTypeBits,
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

/// <summary>Creates a shader module using pre-compiled SPIR-V shader source code</summary>
/// <param name="spvShader">Shader source code</param>
/// <param name="spvShaderSize">Size of the shader source code in bytes</param>
/// <param name="indx">Specifies which shader stage to define in appManager's shaderStages array</param>
/// <param name="shaderStage">Specifies the stage in the pipeline where the shader will exist</param>
void VulkanHelloAPI::createShaderModule(const uint32_t* spvShader, size_t spvShaderSize, int indx, VkShaderStageFlagBits shaderStage)
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

/// <summary>Records rendering commands to the command buffers</summary>
void VulkanHelloAPI::recordCommandBuffer()
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
		vk::CmdDraw(appManager.cmdBuffers[i], 3, 1, 0, 0);

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

/// <summary>Executes the recorded command buffers. The recorded operations will end up rendering and presenting the frame to the surface</summary>
void VulkanHelloAPI::drawFrame()
{
	// This is where the recorded command buffers are executed. The recorded operations will end up rendering
	// and presenting the frame to the surface.

	// currentBuffer will be used to point to the correct frame/command buffer/uniform buffer data.
	// It is going to be the general index of the data being worked on.
	uint32_t currentBuffer = 0;
	VkPipelineStageFlags pipe_stage_flags = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	// Acquire and get the index of the next available swapchain image.
	debugAssertFunctionResult(
		vk::AcquireNextImageKHR(appManager.device, appManager.swapchain, std::numeric_limits<uint64_t>::max(), appManager.acquireSemaphore[frameId], VK_NULL_HANDLE, &currentBuffer),
		"Draw - Acquire Image");

	// Wait for the fence to be signalled before starting to render the current frame, then reset it so it can be reused.
	debugAssertFunctionResult(vk::WaitForFences(appManager.device, 1, &appManager.frameFences[currentBuffer], true, FENCE_TIMEOUT), "Fence - Signalled");

	vk::ResetFences(appManager.device, 1, &appManager.frameFences[currentBuffer]);

	// Use a helper function with the current frame index to calculate the transformation matrix and write it into the correct
	// slice of the uniform buffer.
	applyRotation(currentBuffer);

	// Submit the command buffer to the queue to start rendering.
	// The command buffer is submitted to the graphics queue which was created earlier.
	// Notice the wait (acquire) and signal (present) semaphores, and the fence.
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.pNext = nullptr;
	submitInfo.pWaitDstStageMask = &pipe_stage_flags;
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = &appManager.acquireSemaphore[frameId];
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = &appManager.presentSemaphores[frameId];
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &appManager.cmdBuffers[currentBuffer];

	debugAssertFunctionResult(vk::QueueSubmit(appManager.graphicQueue, 1, &submitInfo, appManager.frameFences[currentBuffer]), "Draw - Submit to Graphic Queue");

	// Queue the rendered image for presentation to the surface.
	// The currentBuffer is again used to select the correct swapchain images to present. A wait
	// semaphore is also set here which will be signalled when the command buffer has
	// finished execution.
	VkPresentInfoKHR presentInfo;
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.pNext = nullptr;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = &appManager.swapchain;
	presentInfo.pImageIndices = &currentBuffer;
	presentInfo.pWaitSemaphores = &appManager.presentSemaphores[frameId];
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pResults = nullptr;

	debugAssertFunctionResult(vk::QueuePresentKHR(appManager.presentQueue, &presentInfo), "Draw - Submit to Present Queue");

	// Update the frameId to get the next suitable one.
	frameId = (frameId + 1) % appManager.swapChainImages.size();
}



/// <summary>Calculate a rotation matrix which provides a rotation around the z axis using the given angle</summary>
/// <param name="angle">The angle through which the rotation will be applied</param>
/// <param name="outRotationMatrix">The output rotation matrix. This matrix must have dimensions 4x4</param>
void rotateAroundZ(float angle, std::array<std::array<float, 4>, 4>& outRotationMatrix)
{
	const float c = cos(angle);
	const float s = sin(angle);

	// Rotation around z axis (0, 0, 1)
	outRotationMatrix[0][0] = c;
	outRotationMatrix[0][1] = s;
	outRotationMatrix[0][2] = 0;
	outRotationMatrix[0][3] = 0;

	outRotationMatrix[1][0] = -s;
	outRotationMatrix[1][1] = c;
	outRotationMatrix[1][2] = 0;
	outRotationMatrix[1][3] = 0;

	outRotationMatrix[2][0] = 0;
	outRotationMatrix[2][1] = 0;
	outRotationMatrix[2][2] = 1;
	outRotationMatrix[2][3] = 0;

	outRotationMatrix[3][0] = 0;
	outRotationMatrix[3][1] = 0;
	outRotationMatrix[3][2] = 0;
	outRotationMatrix[3][3] = 1;
}

/// <summary>Multiple two matrices together.</summary>
/// <param name="first">The first matrix</param>
/// <param name="second">The second matrix</param>
/// <param name="outMatrix">The output matrix</param>
void multiplyMatrices(std::array<std::array<float, 4>, 4>& first, std::array<std::array<float, 4>, 4>& second, std::array<std::array<float, 4>, 4>& outMatrix)
{
	for (uint32_t i = 0; i < 4; i++)
	{
		for (uint32_t j = 0; j < 4; j++)
		{
			for (uint32_t k = 0; k < 4; k++) { outMatrix[i][j] += first[i][k] * second[k][j]; }
		}
	}
}

/// <summary>Updates the dynamic uniform buffer with the new rotation value</summary>
/// <param name="idx">Index which selects the correct area of the buffer</param>
void VulkanHelloAPI::applyRotation(int idx)
{
	// This is called on every frame to update the dynamic uniform buffer with the new rotation
	// value.

	// An offset is used to point to the correct slice of the buffer that corresponds to the current
	// frame. The current frame is specified by the parameter, idx.
	// This memory is mapped persistently so it does not need to be mapped again on every frame. The pointer to this
	// consistently mapped memory is the variable appManager.dynamicUniformBufferData.mappedData.

	// Calculate the offset.
	VkDeviceSize offset = (appManager.offset * idx);

	// Update the angle of rotation and calculate the transformation matrix using the fixed projection
	// matrix and a freshly-calculated rotation matrix.
	appManager.angle += 0.02f;

	auto rotation = std::array<std::array<float, 4>, 4>();
	rotateAroundZ(appManager.angle, rotation);

	auto mvp = std::array<std::array<float, 4>, 4>();
	multiplyMatrices(rotation, viewProj, mvp);

	// Copy the matrix to the mapped memory using the offset calculated above.
	memcpy(static_cast<unsigned char*>(appManager.dynamicUniformBufferData.mappedData) + appManager.dynamicUniformBufferData.bufferInfo.range * idx, &mvp, sizeof(mvp));

	VkMappedMemoryRange mapMemRange = {
		VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE,
		nullptr,
		appManager.dynamicUniformBufferData.memory,
		offset,
		appManager.dynamicUniformBufferData.bufferInfo.range,
	};

	// ONLY flush the memory if it does not support VK_MEMORY_PROPERTY_HOST_COHERENT_BIT.
	if ((appManager.dynamicUniformBufferData.memPropFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) == 0) { vk::FlushMappedMemoryRanges(appManager.device, 1, &mapMemRange); }
}

/// <summary>Initialises all Vulkan objects</summary>
void VulkanHelloAPI::initialize()
{
	// All the Vulkan objects are initialised here.
	// The vk::initVulkan() function is used to load the Vulkan library and definitions.

	// frameId is the index that will be used for synchronisation. It is going to be used mostly by
	// fences and semaphores to keep track of which one is currently free to work on.
	frameId = 0;

	// appManager holds all the object handles which need to be accessed "globally" such as the angle
	// of the rotation of the triangle that is going to be rendered on screen.
	appManager.angle = 45.0f;

	// Initialise all the pointers to Vulkan functions.
	vk::initVulkan();

	// Initialise all the Vulkan objects required to begin rendering.
	std::vector<std::string> layers = initLayers();
    std::vector<std::string> instanceExtensions = initInstanceExtensions();

    initApplicationAndInstance(appManager, instanceExtensions, layers);
    initPhysicalDevice(appManager);

    initSurface(appManager, surfaceData);

    initQueuesFamilies(appManager);

	std::vector<std::string> deviceExtensions = initDeviceExtensions();

    initLogicalDevice(appManager, deviceExtensions);
    initQueues(appManager);
    initSwapChain(appManager, surfaceData);
    initImagesAndViews(appManager);
	initCommandPoolAndBuffer();

	initShaders();
	initVertexBuffers();
	initUniformBuffers();
	initRenderPass();
    loadTexture(appManager);
	initDescriptorPoolAndSet();

	initFrameBuffers();
	initPipeline();

	initViewportAndScissor();
	initSemaphoreAndFence();

	float aspect = 0.0f;
	// The screen is rotated.
	if (surfaceData.width < surfaceData.height) { aspect = surfaceData.height / surfaceData.width; }
	else
	{
		aspect = surfaceData.width / surfaceData.height;
	}

	float left = aspect;
	float right = -aspect;
	float bottom = 1.0;
	float top = -1.0f;

	viewProj[0][0] = 2.0f / (right - left);
	viewProj[1][1] = 2.0f / (top - bottom);
	viewProj[2][2] = -1.0f;
	viewProj[3][0] = -(right + left) / (right - left);
	viewProj[3][1] = -(top + bottom) / (top - bottom);
	viewProj[3][3] = 1.0f;
}

/// <summary>Ensures all created objects are cleaned up correctly and allocated memory is freed</summary>
void VulkanHelloAPI::deinitialize()
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
	vk::FreeDescriptorSets(appManager.device, appManager.descriptorPool, 1, &appManager.staticDescSet);
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

	// Destroy the texture image.
	vk::DestroyImage(appManager.device, appManager.texture.image, nullptr);

	// Destroy the texture image view.
	vk::DestroyImageView(appManager.device, appManager.texture.view, nullptr);

	// Free the memory allocated for the texture.
	vk::FreeMemory(appManager.device, appManager.texture.memory, nullptr);

	// Destroy the sampler.
	vk::DestroySampler(appManager.device, appManager.texture.sampler, nullptr);

	// Destroy then free the memory for the vertex buffer.
	vk::DestroyBuffer(appManager.device, appManager.vertexBuffer.buffer, nullptr);
	vk::FreeMemory(appManager.device, appManager.vertexBuffer.memory, nullptr);

	// Iterate through each of the framebuffers and destroy them.
	for (uint32_t i = 0; i < appManager.frameBuffers.size(); i++) { vk::DestroyFramebuffer(appManager.device, appManager.frameBuffers[i], nullptr); }

	// Destroy the two shader modules - vertex and fragment.
	vk::DestroyShaderModule(appManager.device, appManager.shaderStages[0].module, nullptr);
	vk::DestroyShaderModule(appManager.device, appManager.shaderStages[1].module, nullptr);

	// Destroy the render pass.
	vk::DestroyRenderPass(appManager.device, appManager.renderPass, nullptr);

	// Clean up the swapchain image views.
	for (auto& imagebuffers : appManager.swapChainImages) { vk::DestroyImageView(appManager.device, imagebuffers.view, nullptr); }

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
