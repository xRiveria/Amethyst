#pragma once
#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_Pipeline.h"
#include "../RHI_Shader.h"
#include "../RHI_BlendState.h"
#include "../RHI_InputLayout.h"
#include "../RHI_DescriptorSetLayout.h"
#include "../RHI_RasterizerState.h"

namespace Amethyst
{
	/*
		A pipeline refers to a succession of fixed stages through which data input flows; each stage processes the incoming data and hands it over to the next stage. 
		The final product will be either a 2D rasterized drawing image of the 2D/3D scene (the graphics pipeline) or updated resources (buffers or images) with computational logic and 
		calculations (the compute pipeline). 

		Both types take in commands through command buffers and processes them for their respective purposes.
	*/
	
	RHI_Pipeline::RHI_Pipeline(const RHI_Device* rhi_Device, RHI_PipelineState& rhi_PipelineState, RHI_DescriptorSetLayout* rhi_DescriptorSetLayout)
	{
		m_RHI_Device = rhi_Device;
		m_PipelineState = rhi_PipelineState;

		/*  Pipeline Layout
		* 
			Access to descriptor sets from a pipeline is accomplished through a pipeline layout. Zero or more descriptor set layouts and zero or more push constant 
			ranges are combined to form a pipeline layout object describing the complete set of resources that can be accessed by a pipeline. The pipeline layout 
			represents a sequence of descriptor sets with each having a specific layout. This sequence of layouts is used to determine the interface between 
			shader stages and shader resources.
		*/

		{
			std::array<void*, 1> layouts = { rhi_DescriptorSetLayout->RetrieveResource() };

			// Validate Descriptor Set Layouts
			for (void* layout : layouts)
			{
				AMETHYST_ASSERT(layout != nullptr);
			}

			/* Push Constants
					
				Push Constants are a way to quickly provide a small number of uniform data to shaders. It should be much faster than UBOs but a huge limitation is the size 
				of data (128 bytes, or vendor dependant). As they are faster than descriptors, they are convinient for data that changes between draw calls, like transformation matrices.
				
				From the shader perspective, they are declared through a "layout (push_constant)" qualifier and a block of uniform data. You can only read values from a push constant block.
				Thus, they are constant from the shader's perspective.
			*/

			// Pipeline Layout
			VkPipelineLayoutCreateInfo pipelineLayoutCreateInfo = {};
			pipelineLayoutCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			pipelineLayoutCreateInfo.pushConstantRangeCount = 0;
			pipelineLayoutCreateInfo.setLayoutCount = static_cast<uint32_t>(layouts.size());
			pipelineLayoutCreateInfo.pSetLayouts = reinterpret_cast<VkDescriptorSetLayout*>(layouts.data());

			// Create
			if (!VulkanUtility::Error::CheckResult(vkCreatePipelineLayout(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, &pipelineLayoutCreateInfo, nullptr, reinterpret_cast<VkPipelineLayout*>(&m_PipelineLayout))))
			{
				return;
			}

			// Name
			VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkPipelineLayout>(m_PipelineLayout), m_PipelineState.m_PassName);
		}

		if (m_PipelineState.IsComputePipeline())
		{
			// Shader
			VkPipelineShaderStageCreateInfo shaderStageCreateInfo = {};
			{
				shaderStageCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageCreateInfo.stage = VK_SHADER_STAGE_COMPUTE_BIT;
				shaderStageCreateInfo.module = static_cast<VkShaderModule>(m_PipelineState.m_ComputeShader->RetrieveResource());
				shaderStageCreateInfo.pName = m_PipelineState.m_ComputeShader->RetrieveEntryPoint();

				// Validate Shader Stage
				AMETHYST_ASSERT(shaderStageCreateInfo.module != nullptr);
				AMETHYST_ASSERT(shaderStageCreateInfo.pName  != nullptr);
			}

			// Pipeline
			{
				VkComputePipelineCreateInfo pipelineCreateInfo = {};
				pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
				pipelineCreateInfo.layout = static_cast<VkPipelineLayout>(m_PipelineLayout);
				pipelineCreateInfo.stage = shaderStageCreateInfo;

				// Pipeline Creation
				VkPipeline* pipeline = reinterpret_cast<VkPipeline*>(&m_Pipeline);
				if (!VulkanUtility::Error::CheckResult(vkCreateComputePipelines(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, nullptr, 1, &pipelineCreateInfo, nullptr, pipeline)))
				{
					return;
				}

				// Name
				VulkanUtility::Debug::SetVulkanObjectName(*pipeline, m_PipelineState.m_PassName);
			}
		}
		else if (m_PipelineState.IsGraphicsPipeline() || m_PipelineState.IsDummyPipeline())
		{
			if (m_PipelineState.IsGraphicsPipeline())
			{
				m_PipelineState.CreateFramebuffer(rhi_Device);
			}

			//======================================================================================================================

			/* Dynamic States
	
				Creating a graphics pipeline requires us to provide lots of parameters. What's more, once set, these parameters cannot be changed. Such an approach 
				was taken to improve the performance of our application and present a stable and predictable environment to the driver. Unfortunately, it is also unfeasible 
				as developers will have to create many pipeline objects with almost identical states that differ only in small details.

				Thus, dynamic states were introduced. They allow us to control some of the pipeline's parameters dynamically by recording specific functions in command buffers. 
				To do that, we need to specify which parts of the pipeline are dynamic. This is done by specifying pipeline dynamic states.

				For example, dynamic viewports can be useful if your presentation surface is resized/changes resolution often, perhaps due to some windowed tool. 
				The downside is naturally that your driver may not be able to apply optimizations for this.
			*/

			// Viewport & Scissor
			std::vector<VkDynamicState> dynamicStates;
			VkPipelineDynamicStateCreateInfo dynamicStateCreateInfo	  = {};
			VkViewport vkViewport										  = {};
			VkRect2D vkScissor											  = {};
			VkPipelineViewportStateCreateInfo viewportStateCreateInfo     = {};

			{
				/*
					The viewport defines a part of an attachment (image) to which the clip's space will be mapped. For example, it is the viewing region of your browser (below your bookmarks).
					The scissor test allows us to additionally confine a drawing to the specified rectangle within the specified viewport dimensions. The scissor test is always enabled.
					We can only set up various values for its parameters. This can be done statically during pipeline creation, or dynamically, which is done with a function call in a command buffer.
					
					If no viewport has been provided, we assume that it is dynamic. Thus, we must use vkCmdSetViewport before any drawing commands.
				*/

				if (!m_PipelineState.m_Viewport.IsDefined())
				{
					dynamicStates.emplace_back(VK_DYNAMIC_STATE_VIEWPORT);
				}

				if (m_PipelineState.m_IsScissorDynamic) // We must use vkCmdSetScissor before any drawing commands.
				{
					dynamicStates.emplace_back(VK_DYNAMIC_STATE_SCISSOR);
				}

				//Dynamic States
				dynamicStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
				dynamicStateCreateInfo.pNext = nullptr;
				dynamicStateCreateInfo.flags = 0;
				dynamicStateCreateInfo.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
				dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

				// Viewport - See: http://math.hws.edu/graphicsbook/c2/coordinate-transformation-2d.png
				vkViewport.x	    = m_PipelineState.m_Viewport.m_X;
				vkViewport.y	    = m_PipelineState.m_Viewport.m_Y;
				vkViewport.width    = m_PipelineState.m_Viewport.m_Width;
				vkViewport.height   = m_PipelineState.m_Viewport.m_Height;
				vkViewport.minDepth = m_PipelineState.m_Viewport.m_DepthMinimum;
				vkViewport.maxDepth = m_PipelineState.m_Viewport.m_DepthMaximum;

				// Scissor
				if (!m_PipelineState.m_Scissor.IsDefined())
				{
					vkScissor.offset = { 0, 0 };
					vkScissor.extent.width = static_cast<uint32_t>(vkViewport.width);
					vkScissor.extent.height = static_cast<uint32_t>(vkViewport.height);
				}
				else
				{
					vkScissor.offset = { static_cast<int32_t>(m_PipelineState.m_Scissor.m_Left), static_cast<int32_t>(m_PipelineState.m_Scissor.m_Top) };
					vkScissor.extent.width = static_cast<uint32_t>(m_PipelineState.m_Scissor.Width());
					vkScissor.extent.height = static_cast<uint32_t>(m_PipelineState.m_Scissor.Height());
				}

				// Viewport State
				viewportStateCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
				viewportStateCreateInfo.viewportCount = 1;
				viewportStateCreateInfo.pViewports = &vkViewport;
				viewportStateCreateInfo.scissorCount = 1;
				viewportStateCreateInfo.pScissors = &vkScissor;
			}

			//======================================================================================================================

			// Shader Stages to be included in the graphics pipeline.
			std::vector<VkPipelineShaderStageCreateInfo> shaderStageCreateInfos;

			//Vertex Shader
			if (m_PipelineState.m_VertexShader)
			{
				VkPipelineShaderStageCreateInfo shaderStageVertexCreateInfo = {};
				shaderStageVertexCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStageVertexCreateInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
				shaderStageVertexCreateInfo.module = static_cast<VkShaderModule>(m_PipelineState.m_VertexShader->RetrieveResource()); // Our shader module (code).
				shaderStageVertexCreateInfo.pName = m_PipelineState.m_VertexShader->RetrieveEntryPoint(); // Specifies the entry point of the shader for this stage.

				// Validates our shader stage by making sure that our shader module and entry points are present.
				AMETHYST_ASSERT(shaderStageVertexCreateInfo.module != nullptr);
				AMETHYST_ASSERT(shaderStageVertexCreateInfo.pName != nullptr);

				shaderStageCreateInfos.push_back(shaderStageVertexCreateInfo);
			}
			else // If our graphics pipeline does not have a vertex shader/invalid...
			{
				AMETHYST_ERROR("The vertex shader is invalid.");
				return;
			}

			// Pixel Shader
			if (m_PipelineState.m_PixelShader)
			{
				VkPipelineShaderStageCreateInfo shaderStagePixelCreateInfo = {};
				shaderStagePixelCreateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
				shaderStagePixelCreateInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
				shaderStagePixelCreateInfo.module = static_cast<VkShaderModule>(m_PipelineState.m_PixelShader->RetrieveResource());
				shaderStagePixelCreateInfo.pName = m_PipelineState.m_PixelShader->RetrieveEntryPoint();

				// Validate our shader stage by making sure that our shader module and entry points are present.
				AMETHYST_ASSERT(shaderStagePixelCreateInfo.module != nullptr);
				AMETHYST_ASSERT(shaderStagePixelCreateInfo.pName != nullptr);

				shaderStageCreateInfos.push_back(shaderStagePixelCreateInfo);
			}

			//======================================================================================================================

			// Binding Description
			VkVertexInputBindingDescription bindingDescriptionInfo = {};
			bindingDescriptionInfo.binding = 0; // Binding number.
			bindingDescriptionInfo.inputRate = VK_VERTEX_INPUT_RATE_VERTEX; // The rate which vertex attributes are pulled from buffers. Can be vertex/instance based (for instanced rendering).
			bindingDescriptionInfo.stride = m_PipelineState.m_VertexBufferStride; // Distance in bytes between two consecutive elements within the buffer.

			// Vertex Attributes Descriptions - 1 for each vertex attribute we have. 
			std::vector<VkVertexInputAttributeDescription> vertexAttributeDescriptionInfos = {};

			if (m_PipelineState.m_VertexShader)
			{
				if (RHI_InputLayout* inputLayout = m_PipelineState.m_VertexShader->RetrieveInputLayout().get())
				{
					// Reserve allocates but does not construct. Push back will increment the vector list (and construct) if the allocated size is not enough. Resize initializes (basically does both).
					vertexAttributeDescriptionInfos.reserve(inputLayout->RetrieveAttributeDescriptions().size());

					for (const VertexAttribute& description : inputLayout->RetrieveAttributeDescriptions())
					{
						vertexAttributeDescriptionInfos.push_back
						({
							description.m_Location,					// Location - Shader input location number for the attribute.
							description.m_Binding,					// Binding  - Binding number which this attribute takes it data from.
							VulkanFormat[description.m_Format],		// Format   - Size and type of the vertex attribute data.
							description.m_Offset					// Offset   - Byte offset of this attribute relative to the start of an element in the vertex input binding.
						});
					}
				}
			}

			// Vertex Input State
			VkPipelineVertexInputStateCreateInfo vertexInputStateInfo = {};
			{
				vertexInputStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
				vertexInputStateInfo.vertexBindingDescriptionCount = 1;
				vertexInputStateInfo.pVertexBindingDescriptions = &bindingDescriptionInfo;
				vertexInputStateInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(vertexAttributeDescriptionInfos.size());
				vertexInputStateInfo.pVertexAttributeDescriptions = vertexAttributeDescriptionInfos.data();
			}

			//======================================================================================================================

			// Input Assembly - How the input attributes are assembled. 
			VkPipelineInputAssemblyStateCreateInfo inputAssemblyStateInfo = {};
			{
				inputAssemblyStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
				inputAssemblyStateInfo.topology = VulkanPrimitiveTopology[m_PipelineState.m_PrimitiveTopology];
				//Primitive Restart controls whether a special vertex index value is treated as restarting the assembly of primitives. This enable only applies to indexed draws. 
				inputAssemblyStateInfo.primitiveRestartEnable = VK_FALSE; // Restarting the assembly discards the most recent index values if those elements formed an incomplete primitive, and restarts the assembly.
			}

			//======================================================================================================================
			///
			// Rasterizer State - Controls the conversion of your shapes into a raster image (a series of pixels, dots or lines) to be colored by the fragment shader.
			VkPipelineRasterizationStateCreateInfo rasterizerStateInfo = {};

			// Specifying our depth clipping state.
			VkPipelineRasterizationDepthClipStateCreateInfoEXT rasterizerStateDepthClip = {};
			{
				rasterizerStateDepthClip.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_DEPTH_CLIP_STATE_CREATE_INFO_EXT;
				rasterizerStateDepthClip.pNext = nullptr;
				rasterizerStateDepthClip.flags = 0;
				rasterizerStateDepthClip.depthClipEnable = m_PipelineState.m_RasterizerState->RetrieveDepthClippingEnabled();

				rasterizerStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
				rasterizerStateInfo.pNext = &rasterizerStateDepthClip;
				rasterizerStateInfo.depthClampEnable = VK_FALSE; // If set to true, fragments that are beyond the near and far planes are clamped to them as opposed to discarding them. This is useful for shadow maps.
				rasterizerStateInfo.rasterizerDiscardEnable = VK_FALSE; // If this is set to true, the geometry will never pass the rasterizer state, disabling output to the framebuffer.
				rasterizerStateInfo.polygonMode = VulkanPolygonMode[m_PipelineState.m_RasterizerState->RetrieveFillMode()]; // Filled/Line
				rasterizerStateInfo.lineWidth = m_RHI_Device->RetrieveContextRHI()->m_PhysicalDeviceFeatures.features.wideLines ? m_PipelineState.m_RasterizerState->RetrieveLineWidth() : 1.0f;
				rasterizerStateInfo.cullMode = VulkanCullMode[m_PipelineState.m_RasterizerState->RetrieveCullMode()]; // Selects cull mode (front/back).
				rasterizerStateInfo.frontFace = VK_FRONT_FACE_CLOCKWISE; // Specifies the vertex order for faces to be considered front-facing. Can be clockwise or anticlockwise.

				/* Depth Bias
				
					Alters depth values by adding a constant value or biasing them based on a fragment's slope. Sometimes used for shadow mapping.

					Polygons that are coplanar in 3D space can be made to appear as if they are not coplanar by adding a Z-Bias (or depth bias) to each one.
					This is a technique commonly used to ensure that shadows in a scene are displayed properly. For instance, a shadow on a wall will likely have the same 
					depth value as the wall. If an application renders a wall first and then a shadow, the shadow might not be visible, or depth artifacts might be visible.

					An application can help ensure that coplanar polgyons are rendered properly by adding a bias to the Z-values that the system uses when rendering the 
					sets of coplanar polygons. Polygons with a larger Z value will be drawn in front of polygons with a smaller Z value.

					One of the artifacts with shadow buffer based shadows is shadow acne, or a surface shadowing itself due to minor differences between the depth 
					computation in a shader, and the depth of the same surface in the shadow buffer. One way to alleviate this is to use DepthBias and SlopeScaledDepthBias 
					when rendering a shadow buffer. The idea is to push surfaces out enough while rendering a shadow buffer so that the comparison result 
					(between the Shadow-Z buffer and the Shader-Z) is consistent across the surface, and avoid local self-shadowing.

					However, using DepthBias and SlopeScaledDepthBias can introduce new rendering problems when a polygon viewed at an extremelys sharp angle causes the bias 
					equation to generate a very a large Z value. This in effect pushes the polygon extremely far away from the original surface in the shadow map. One way to help 
					alleviate this problem is to use DepthBiasClamp which provides an upper bound (positive or negative) on the magnitude of the Z bias calculated. 
				*/

				rasterizerStateInfo.depthBiasEnable = m_PipelineState.m_RasterizerState->RetrieveDepthBias() != 0.0f ? VK_TRUE : VK_FALSE; // Whether to bias fragment depth values.
				rasterizerStateInfo.depthBiasConstantFactor = Math::Utilities::Floor(m_PipelineState.m_RasterizerState->RetrieveDepthBias() * (float)(1 << 24)); // Scalar factor controlling the constant depth value added to each fragment.
				rasterizerStateInfo.depthBiasClamp = m_PipelineState.m_RasterizerState->RetrieveDepthBiasClamp(); // The maximum (or minimum) depth bias of a fragment.
				rasterizerStateInfo.depthBiasSlopeFactor = m_PipelineState.m_RasterizerState->RetrieveDepthBiasSlopeScaled(); // Scalar factor applied to a fragment's slope in depth bias calculations.
			}

			//======================================================================================================================

			// Multisampling
			VkPipelineMultisampleStateCreateInfo multisamplingStateInfo = {};
			{
				multisamplingStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
				
				/* Sample Shading
				
					Sample Shading can be used to specify a minimum number of unique samples to process for each fragment. Simply setting rasterizationSamples may have 
					limitations which may impact the quality of the output image in more detailed scenes. For example, there may be issues with shader aliasing, as 
					MSAA only smoothens out the edges of the geometry but not the interior filling.

					This may lead to a situation whereby you get a smooth polygon rendered on the screen but the applied texture will still look aliased if it contains 
					highly contrasting colors. One way to approach this is Sample shading, which will improve the image quality at the cost of performance.

					See: https://vulkan-tutorial.com/images/sample_shading.png
				*/

				multisamplingStateInfo.sampleShadingEnable = VK_FALSE;
				//multisamplingStateInfo.minSampleShading = 0.2f; // The minimum fraction of sample shading - max([(minSampleShading x rasterizationSamples], 1)
				multisamplingStateInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT; //Specifies the number of samples used in rasterization. Currently, it is set to one sample per pixel. We can set this to a maximum of 64.
			}

			//======================================================================================================================

			// Blend State
			VkPipelineColorBlendStateCreateInfo colorBlendStateInfo = {};
			std::vector<VkPipelineColorBlendAttachmentState> blendStateAttachments; 
			{
				// Blend State Attachments
				{
					// Same blend state across everything. All must be identical unless independant blending is enabled. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkPipelineColorBlendStateCreateInfo.html
					VkPipelineColorBlendAttachmentState blendStateAttachmentInfo = {};
					// The write mask specifies whether the final color values R, G, B and A are written to the framebuffer attachment. Otherwise, the value in memory is unmodified.
					blendStateAttachmentInfo.colorWriteMask		 = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
					blendStateAttachmentInfo.blendEnable		 = m_PipelineState.m_BlendState->RetrieveBlendingEnabled() ? VK_TRUE : VK_FALSE;
					blendStateAttachmentInfo.srcColorBlendFactor = VulkanBlendFactor[m_PipelineState.m_BlendState->RetrieveSourceBlendFunction()];
					blendStateAttachmentInfo.dstColorBlendFactor = VulkanBlendFactor[m_PipelineState.m_BlendState->RetrieveDestinationBlendFunction()];
					blendStateAttachmentInfo.colorBlendOp		 = VulkanBlendOperation[m_PipelineState.m_BlendState->RetrieveBlendOperation()];
					blendStateAttachmentInfo.srcAlphaBlendFactor = VulkanBlendFactor[m_PipelineState.m_BlendState->RetrieveSourceBlendFunctionAlpha()];
					blendStateAttachmentInfo.dstAlphaBlendFactor = VulkanBlendFactor[m_PipelineState.m_BlendState->RetrieveDestinationBlendFunctionAlpha()];
					blendStateAttachmentInfo.alphaBlendOp		 = VulkanBlendOperation[m_PipelineState.m_BlendState->RetrieveBlendOperationAlpha()];

					// Swapchain
					if (m_PipelineState.m_RenderTargetSwapchain)
					{
						blendStateAttachments.push_back(blendStateAttachmentInfo);
					}

					// Render Target(s)
					for (int8_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
					{
						if (m_PipelineState.m_RenderTargetColorTextures[i] != nullptr)
						{
							blendStateAttachments.push_back(blendStateAttachmentInfo);
						}
					}
				}

				colorBlendStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
				colorBlendStateInfo.logicOpEnable = VK_FALSE; // Enables a logical operation between the fragment's color values and existing value in the framebuffer attachment. If enabled, blending is disabled completely, and the operation selected below will apply.
				colorBlendStateInfo.logicOp = VK_LOGIC_OP_COPY; // Copy as it is. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#framebuffer-logicop
				colorBlendStateInfo.attachmentCount = static_cast<uint32_t>(blendStateAttachments.size()); // The number of VkPipelineColorBlendAttachmentState elements in pAttachments.
				colorBlendStateInfo.pAttachments = blendStateAttachments.data(); // Pointer to an array of per target attachment states.

				// Our blend constants for the R, G, B and A components respectively, depending on the blend function. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#framebuffer-blendfactors
				// Default to 1.0f as per Vulkan conventions, but can be changed for blend functions such as VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR. Ignored otherwise.
				colorBlendStateInfo.blendConstants[0] = m_PipelineState.m_BlendState->RetrieveBlendFactor(); 
				colorBlendStateInfo.blendConstants[1] = m_PipelineState.m_BlendState->RetrieveBlendFactor();
				colorBlendStateInfo.blendConstants[2] = m_PipelineState.m_BlendState->RetrieveBlendFactor();
				colorBlendStateInfo.blendConstants[3] = m_PipelineState.m_BlendState->RetrieveBlendFactor();
			}

			//======================================================================================================================

			// Depth/Stencil State
			VkPipelineDepthStencilStateCreateInfo depthStencilStateInfo = {};
			{
				depthStencilStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
				depthStencilStateInfo.depthTestEnable = m_PipelineState.m_DepthStencilState->RetrieveDepthTestingEnabled();
				depthStencilStateInfo.depthWriteEnable = m_PipelineState.m_DepthStencilState->RetrieveDepthWritingEnabled();
				depthStencilStateInfo.depthCompareOp = VulkanCompareOperator[m_PipelineState.m_DepthStencilState->RetrieveDepthComparisonFunction()];
				depthStencilStateInfo.stencilTestEnable = m_PipelineState.m_DepthStencilState->RetrieveStencilTestingEnabled();

				/* Stencil Testing: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#fragops-stencil
				 
					Stencil testing compares the stencil attachment value in the depth/stencil attachment at each of the sample's framebuffer coordinates and sample index i against a stencil reference value.

					The stencil test is controlled by one of two sets of stencil related state, the front stencil state and the back stencil state (VkStencilOpState). Stencil tests 
					and writes use the back stencil state when processing fragments generated by back-facing polygons, and the front stencil state when processing 
					fragmenrs generated by front-facing polygons or any other primitives.
					
					The stencil reference and attachment values are independantly combined with the compare mask using a logical AND operation to create masked reference 
					and attachment values. 
				*/

				depthStencilStateInfo.front.compareOp = VulkanCompareOperator[m_PipelineState.m_DepthStencilState->RetrieveStencilComparisonFunction()];
				depthStencilStateInfo.front.failOp = VulkanStencilOperation[m_PipelineState.m_DepthStencilState->RetrieveStencilFailOperation()];
				depthStencilStateInfo.front.depthFailOp = VulkanStencilOperation[m_PipelineState.m_DepthStencilState->RetrieveStencilDepthFailOperation()];
				depthStencilStateInfo.front.passOp = VulkanStencilOperation[m_PipelineState.m_DepthStencilState->RetrieveStencilPassOperation()];
				depthStencilStateInfo.front.compareMask = m_PipelineState.m_DepthStencilState->RetrieveStencilReadMask(); // Selects the bits of the unsigned integer stencil values participating in the stencil test. 
				depthStencilStateInfo.front.writeMask = m_PipelineState.m_DepthStencilState->RetrieveStencilWriteMask();  // Selects the bits of the unsigned integer stencil values updated by the stencil test in the stencil framebuffer attachment.
				depthStencilStateInfo.front.reference = 1; // Integer reference value used in the unsigned stencil comparison.
				depthStencilStateInfo.back = depthStencilStateInfo.front;
			}

			//======================================================================================================================

			// Finally, our graphics pipeline itself.
			VkGraphicsPipelineCreateInfo pipelineCreateInfo = {};
			{
				// Culmulate everything so far.
				pipelineCreateInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
				pipelineCreateInfo.stageCount = static_cast<uint32_t>(shaderStageCreateInfos.size());
				pipelineCreateInfo.pStages = shaderStageCreateInfos.data();
				pipelineCreateInfo.pVertexInputState = &vertexInputStateInfo;
				pipelineCreateInfo.pDynamicState = dynamicStates.empty() ? nullptr : &dynamicStateCreateInfo;
				pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
				pipelineCreateInfo.pRasterizationState = &rasterizerStateInfo;
				pipelineCreateInfo.pMultisampleState = &multisamplingStateInfo;
				pipelineCreateInfo.pColorBlendState = &colorBlendStateInfo;
				pipelineCreateInfo.pDepthStencilState = &depthStencilStateInfo;
				pipelineCreateInfo.layout = static_cast<VkPipelineLayout>(m_PipelineLayout);
				pipelineCreateInfo.renderPass = static_cast<VkRenderPass>(m_PipelineState.RetrieveRenderPass());

				// Creation
				VkPipeline* pipeline = reinterpret_cast<VkPipeline*>(&m_Pipeline);
				if (!VulkanUtility::Error::CheckResult(vkCreateGraphicsPipelines(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, nullptr, 1, &pipelineCreateInfo, nullptr, pipeline)))
				{
					return;
				}

				// Name
				VulkanUtility::Debug::SetVulkanObjectName(*pipeline, m_PipelineState.m_PassName);
			}
		}
	}

	RHI_Pipeline::~RHI_Pipeline()
	{
		// Wait in case it's still in use by the GPU.
		m_RHI_Device->Queue_WaitAll();

		vkDestroyPipeline(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkPipeline>(m_Pipeline), nullptr);
		m_Pipeline = nullptr;

		vkDestroyPipelineLayout(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkPipelineLayout>(m_PipelineLayout), nullptr);
		m_PipelineLayout = nullptr;
	}
};