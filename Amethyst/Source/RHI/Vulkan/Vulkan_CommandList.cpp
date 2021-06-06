#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_CommandList.h"
#include "../RHI_Pipeline.h"
#include "../RHI_VertexBuffer.h"
#include "../RHI_IndexBuffer.h"
#include "../RHI_ConstantBuffer.h"
#include "../RHI_Sampler.h"
#include "../RHI_DescriptorSet.h"
#include "../RHI_DescriptorSetLayout.h"
#include "../RHI_DescriptorSetLayoutCache.h"
#include "../RHI_PipelineCache.h"
#include "../RHI_Semaphore.h"
#include "../RHI_Fence.h"
#include "../Rendering/Renderer.h"

namespace Amethyst
{
	RHI_CommandList::RHI_CommandList(uint32_t index, RHI_SwapChain* swapchain, Context* context)
	{
		m_SwapChain = swapchain;
		m_Renderer = context->RetrieveSubsystem<Renderer>();
		// Profiler
		m_RHI_Device = m_Renderer->RetrieveRHIDevice().get();
		m_PipelineCache = m_Renderer->RetrievePipelineCache();
		m_DescriptorSetLayoutCache = m_Renderer->RetrieveDescriptorLayoutCache();

		RHI_Context* rhi_Context = m_RHI_Device->RetrieveContextRHI();

		// Command Buffer
		VulkanUtility::CommandBuffer::CreateCommandBuffer(m_SwapChain->RetrieveCommandPool(), m_CommandBuffer, VK_COMMAND_BUFFER_LEVEL_PRIMARY);
		VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkCommandBuffer>(m_CommandBuffer), "Command Buffer");

		// Sync - Fence
		m_ProcessedFence = std::make_shared<RHI_Fence>(m_RHI_Device, "Processed Command Buffer");

		// Sync - Semaphore
		m_ProcessedSemaphore = std::make_shared<RHI_Semaphore>(m_RHI_Device, false, "Processed Command Buffer");

		// Query Pool - Only if profiler exists.
	}

	RHI_CommandList::~RHI_CommandList()
	{
		RHI_Context* rhi_Context = m_RHI_Device->RetrieveContextRHI();

		// Wait in case its still in use by the GPU.
		m_RHI_Device->Queue_WaitAll();

		// Command Buffer
		VulkanUtility::CommandBuffer::DestroyCommandBuffer(m_SwapChain->RetrieveCommandPool(), m_CommandBuffer);

		// Query Pool
		if (m_QueryPool) // - Swapchain
		{
			///=====
		}
	}

	bool RHI_CommandList::BeginCommandBuffer()
	{
		// If the command buffer is in use, wait for it to be complete.
		if (m_CommandListState == RHI_CommandListState::Submitted)
		{
			if (!WaitCommandBuffer())
			{
				AMETHYST_ERROR("Failed to wait for command buffer to complete.");
				return false;
			}
		}

		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Idle);

		// Retrieve Queries
		///=====

		// Specifies a command buffer begin operation. 
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT; // Specifies that each recording of the command buffer will only be submitted once, and that the command buffer will be reset and recorded again between each submission.

		if (!VulkanUtility::Error::CheckResult(vkBeginCommandBuffer(static_cast<VkCommandBuffer>(m_CommandBuffer), &beginInfo)))
		{
			return false;
		}

		/// 

		m_CommandListState = RHI_CommandListState::Recording;
		m_IsCommandBufferFlushed = false;

		return true;
	}

	bool RHI_CommandList::EndCommandBuffer()
	{
		// Validate command buffer state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		if (!VulkanUtility::Error::CheckResult(vkEndCommandBuffer(static_cast<VkCommandBuffer>(m_CommandBuffer))))
		{
			return false;
		}

		m_CommandListState == RHI_CommandListState::Ended;

		return true;
	}

	bool RHI_CommandList::SubmitCommandBuffer()
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Ended);

		// Ensure that the processed semaphore can be used.
		AMETHYST_ASSERT(m_ProcessedSemaphore->RetrieveState() == RHI_Semaphore_State::Idle);

		// Retrieve wait and signal semaphore.
		RHI_Semaphore* waitSemaphore = nullptr;
		RHI_Semaphore* signalSemaphore = nullptr;

		if (m_Pipeline)
		{
			if (RHI_PipelineState* state = m_Pipeline->RetrievePipelineState())
			{
				if (state->m_RenderTargetSwapchain)
				{
					// If the swapchain is not presenting (e.g. minimized window), don't submit any work.
					if (!state->m_RenderTargetSwapchain->IsPresentationEnabled())
					{
						m_CommandListState = RHI_CommandListState::Submitted;
						return true;
					}

					// Queue submission should wait on this semaphore (image must have been acquired).
					if (state->m_RenderTargetSwapchain->RetrieveImageAcquiredSemaphore()->RetrieveState() == RHI_Semaphore_State::Signaled)
					{
						waitSemaphore = state->m_RenderTargetSwapchain->RetrieveImageAcquiredSemaphore();
					}

					signalSemaphore = m_ProcessedSemaphore.get(); // Signal this semaphore once the command buffer has completed execution.
				}
			}
		}

		m_ProcessedFence->Reset(); // Fence to signal once the command buffer completes execution.

		if (!m_RHI_Device->Queue_Submit(
			RHI_Queue_Type::RHI_Queue_Graphics,		       // Queue
			VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT, // Wait Flags
			static_cast<VkCommandBuffer>(m_CommandBuffer), // Command Buffer
			waitSemaphore,								   // Wait Semaphore
			signalSemaphore,							   // Signal Semaphore
			m_ProcessedFence.get()						   // Signal Fence
		))
		{
			AMETHYST_ERROR("Failed to submit the command buffer.");
			return false;
		}

		m_CommandListState = RHI_CommandListState::Submitted;
		return true;
	}

	bool RHI_CommandList::ResetCommandBuffer()
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		std::lock_guard<std::mutex> guard(m_MutexReset);

		// Reset a command buffer to the initial state. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#commandbuffers-lifecycle
		// Side note, if a command buffer is in pending state (which occurs after queue submission), applications must not attempt to modify the command buffer in any way, as the device may be processing commands recorded into it.
		// VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT: Specifies that most or all memory resources currently owned by the command buffer should be returned to the parent command pool. If not set, the command buffer may hold onto memory resources and reuse them when recording commands. 
		if (!VulkanUtility::Error::CheckResult(vkResetCommandBuffer(static_cast<VkCommandBuffer>(m_CommandBuffer), VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT)))
		{
			return false;
		}

		m_CommandListState = RHI_CommandListState::Idle;

		return true;
	}

	bool RHI_CommandList::BeginRenderPass(RHI_PipelineState& pipelineState)
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		// Retrieve pipeline.
		{
			m_IsPipelineActive = false;

			// Update the descriptor cache with the pipeline state.
			m_DescriptorSetLayoutCache->SetPipelineState(pipelineState);

			// Retrieve (or create) a pipeline which matches the pipeline state.
			m_Pipeline = m_PipelineCache->RetrievePipeline(this, pipelineState, m_DescriptorSetLayoutCache->RetrieveCurrentDescriptorSetLayout());

			if (!m_Pipeline)
			{
				AMETHYST_ERROR("Failed to acquire appropriate pipeline.");
				return false;
			}

			// Keep a local pointer for convenience.
			m_PipelineState = &pipelineState;
		}

		// Start marker and profiler (if used).
		///

		// Shader resources.
		{
			// If the pipeline changed, resources have to be reset again.
			m_VertexBufferID = 0;
			m_IndexBufferID = 0;

			// Vulkan doesn't have a persistent state, so global resources have to be set.
			m_Renderer->SetGlobalSamplersAndConstantBuffers(this);
		}

		return true;
	}

	bool RHI_CommandList::EndRenderPass()
	{
		// Render Pass
		if (m_IsRenderPassActive)
		{
			// To record a command to end a render pass instance after recording the commands for the last subpass, call vkCmdEndRenderPass.
			vkCmdEndRenderPass(static_cast<VkCommandBuffer>(m_CommandBuffer)); // A command buffer in which to submit the end command to in order to end the render pass instance.
			m_IsRenderPassActive = false;
		}

		// Profiling
		///

		return true;
	}

	bool RHI_CommandList::OnDraw()
	{
		if (m_IsCommandBufferFlushed)
		{
			return false;
		}

		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		// Begin render pass.
		if (!m_IsRenderPassActive && !m_PipelineState->IsComputePipeline())
		{
			if (!Deferred_BeginRenderPass())
			{
				AMETHYST_ERROR("Failed to begin render pass.");
				return false;
			}
		}

		// Set pipeline.
		if (!m_IsPipelineActive)
		{
			if (!Deferred_BindPipeline())
			{
				AMETHYST_ERROR("Failed to begin render pass.");
				return false;
			}
		}

		// Bind descriptor set.
		return Deferred_BindDescriptorSet();
	}

	bool RHI_CommandList::Draw(const uint32_t vertexCount)
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		if (!OnDraw()) // Begins Deferred Render Pass, Binds Deferred Pipeline and Binds Deferred Descriptor Set.
		{
			return false;
		}

		vkCmdDraw(
			static_cast<VkCommandBuffer>(m_CommandBuffer), // Command Buffer - Buffer in which the command is recorded.
			vertexCount,								   // Vertex Count   - The number of vertices to draw.
			1,											   // Instance Count - The number of instances to draw.
			0,											   // First Vertex   - The index of the first vertex to draw.
			0											   // First Instance - Instance ID of the first instance to draw.
		);

		/// Profiling.

		return true;
	}

	bool RHI_CommandList::DrawIndexed(const uint32_t indexCount, const uint32_t indexOffset, const uint32_t vertexOffset)
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		// Ensure correct state before attempting to draw.

		if (!OnDraw()) // Begins Deferred Render Pass, Binds Deferred Pipeline and Binds Deferred Descriptor Set.
		{
			return false;
		}

		// Draws primitive with indexed vertices. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCmdDrawIndexed.html
		vkCmdDrawIndexed(
			static_cast<VkCommandBuffer>(m_CommandBuffer), // Command Buffer - Buffer into which the command is recorded.
			indexCount,									   // Index Count	 - The number of vertices to draw.
			1,											   // Instance Count - The number of instances to draw.
			indexCount,									   // First Index    - Base index within the index buffer.
			vertexOffset,								   // Vertex Offset	 - Value added to the vertex index before indexing into the vertex buffer.
			0											   // First Instance - Instance ID of the first instanc to draw.
		);

		/// Profiling.

		return true;
	}

	void RHI_CommandList::ClearPipelineStateRenderTargets(RHI_PipelineState& pipelineState)
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		if (m_IsRenderPassActive)
		{
			uint32_t attachmentCount = 0;
			std::array<VkClearAttachment, g_RHI_MaxRenderTargetCount + 1> attachments; // +1 for depth stencil.

			for (uint8_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
			{
				if (!m_PipelineState->m_RenderTargetColorTextures[i])
				{
					continue;
				}

				if (pipelineState.m_ClearValuesColor[i] != g_RHI_ColorLoadValue)
				{
					attachments[i].aspectMask = VK_IMAGE_ASPECT_COLOR_BIT; // Mask selecting the color/depth/and or stencil aspects of the attachment to be cleared.
					attachments[i].colorAttachment = attachmentCount++; // Only meaningful if VK_IMAGE_ASPECT_COLOR_BIT is set in aspectMask, in which case it is an index to the pColorAttachments array in the VkSubPass description structure of the current subpass which selects the color attachment to clear.
					attachments[i].clearValue.color.float32[0] = pipelineState.m_ClearValuesColor[i].m_X;
					attachments[i].clearValue.color.float32[1] = pipelineState.m_ClearValuesColor[i].m_Y;
					attachments[i].clearValue.color.float32[2] = pipelineState.m_ClearValuesColor[i].m_Z;
					attachments[i].clearValue.color.float32[3] = pipelineState.m_ClearValuesColor[i].m_W;
				}
			}

			bool clearDepth = pipelineState.m_ClearValueDepth != g_RHI_DepthLoadValue;
			bool clearStencil = pipelineState.m_ClearValueStencil != g_RHI_StencilLoadValue;

			if (clearDepth || clearStencil)
			{
				VkClearAttachment& attachment = attachments[attachmentCount++];

				if (clearDepth)
				{
					attachment.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
				}

				if (clearStencil)
				{
					attachment.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
				}

				attachment.clearValue.depthStencil.depth = pipelineState.m_ClearValueDepth;
				attachment.clearValue.depthStencil.stencil = pipelineState.m_ClearValueStencil;
			}

			// Structure specifying a clear rectangle. The layers counting from the base layer of the attachment image view are cleared.
			VkClearRect clearRectInfo = {};
			clearRectInfo.baseArrayLayer = 0; // The first layer to be cleared.
			clearRectInfo.layerCount = 1; // Number of layers to clear.
			// A two dimensional layer to be cleared.
			clearRectInfo.rect.extent.width = pipelineState.RetrieveRenderTargetWidth();
			clearRectInfo.rect.extent.height = pipelineState.RetrieveRenderTargetHeight();

			// vkCmdClearAttachments can clear multiple regions of each attachment used in the current subpass of a render pass instance. 
			// Clear regions within bound framebuffer attachments. This command must be called only inside a render pass instance, and implictly selects the images to clear based on the current framebuffer attachments and the command parameters.
			vkCmdClearAttachments(static_cast<VkCommandBuffer>(m_CommandBuffer), attachmentCount, attachments.data(), 1, &clearRectInfo);
		}

		else if (BeginRenderPass(pipelineState))
		{
			OnDraw(); // Start render pass.
			EndRenderPass(); // End render pass.
		}
	}

	void RHI_CommandList::ClearRenderTarget(
		RHI_Texture* texture,
		const uint32_t colorIndex			/*= 0*/,
		const uint32_t depthStencilIndex	/*= 0*/,
		const bool storage					/*= false*/,
		const Math::Vector4& clearColor		/*= g_RHI_ColorLoadValue*/,
		const float clearDepth				/*= g_RHI_DepthLoadValue*/,
		const uint32_t clearStencil			/*= g_RHI_StencilLoadValue*/
	)
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		if (m_IsRenderPassActive)
		{
			AMETHYST_ERROR("This must be only be called outside of a render pass instance.");
			return;
		}

		if (!texture || !texture->RetrieveResourceView())
		{
			AMETHYST_ERROR("Texture is null.");
			return;
		}

		// One of the required layouts for clear functions.
		texture->SetLayout(RHI_Image_Layout::Transfer_Destination_Optimal, this);

		VkImageSubresourceRange imageSubresourceRangeInfo = {};
		imageSubresourceRangeInfo.baseMipLevel = 0;
		imageSubresourceRangeInfo.levelCount = 1;
		imageSubresourceRangeInfo.baseArrayLayer = 0;
		imageSubresourceRangeInfo.layerCount = 1;

		if (texture->IsColorFormat())
		{
			VkClearColorValue _clearValue = { clearColor.m_X, clearColor.m_Y, clearColor.m_Z, clearColor.m_W };
			imageSubresourceRangeInfo.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;

			// Clear regions of a color image. The imageLayout here specifies the current layout of the image subresource ranges to be cleared and must be VK_IMAGE_LAYOUT_SHARED_PRESENT_KHR, VK_IMAGE_LAYOUT_GENERAL or VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL.
			vkCmdClearColorImage(static_cast<VkCommandBuffer>(m_CommandBuffer), static_cast<VkImage>(texture->RetrieveResource()), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &_clearValue, 1, &imageSubresourceRangeInfo);
		}
		else if (texture->IsDepthStencilFormat())
		{
			VkClearDepthStencilValue clearDepthStencilValue = { clearDepth, clearStencil };

			if (texture->IsDepthFormat())
			{
				imageSubresourceRangeInfo.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
			}

			if (texture->IsStencilFormat())
			{
				imageSubresourceRangeInfo.aspectMask |= VK_IMAGE_ASPECT_DEPTH_BIT;
			}
			
			// Fill regions of a combined depth/stencil image.
			vkCmdClearDepthStencilImage(static_cast<VkCommandBuffer>(m_CommandBuffer), static_cast<VkImage>(texture->RetrieveResource()), VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearDepthStencilValue, 1, &imageSubresourceRangeInfo);
		}
	}

	bool RHI_CommandList::Dispatch(uint32_t x, uint32_t y, uint32_t z, bool async /*= false*/)
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		// Ensure correct state before attempting to draw.
		if (!OnDraw())
		{
			return false;
		}

		// Dispatch compute work items. groupCountX, groupCountY, groupCountZ are the number of local workgroups to dispatch in the X, Y and Z dimensions.
		vkCmdDispatch(static_cast<VkCommandBuffer>(m_CommandBuffer), x, y, z);
		///Profiler.

		return true;
	}

	//=========================

	void RHI_CommandList::SetViewport(const RHI_Viewport& viewport) const
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		VkViewport vkViewport = {};
		vkViewport.x = viewport.m_X;
		vkViewport.y = viewport.m_Y;
		vkViewport.width = viewport.m_Width;
		vkViewport.height = viewport.m_Height;
		vkViewport.minDepth = viewport.m_DepthMinimum;
		vkViewport.maxDepth = viewport.m_DepthMaximum;

		// Record into command buffer.
		vkCmdSetViewport(
			static_cast<VkCommandBuffer>(m_CommandBuffer),		// Command Buffer: Buffer into which the command will be recorded.
			0,													// First Viewport: The index of the first viewport whose parameters are updated by the command.
			1,													// Viewport Count: The number of viewports whose parameters are updated by the command.
			&vkViewport											// An array of VkViewport structures specifying viewport parameters.
		);
	}

	void RHI_CommandList::SetScissorRectangle(const Math::Rectangle& scissorRectangle) const
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		VkRect2D vkScissor = {};
		vkScissor.offset.x = static_cast<int32_t>(scissorRectangle.m_Left);
		vkScissor.offset.y = static_cast<int32_t>(scissorRectangle.m_Top);
		vkScissor.extent.width = static_cast<uint32_t>(scissorRectangle.Width());
		vkScissor.extent.height = static_cast<uint32_t>(scissorRectangle.Height());

		// Record into command buffer.
		vkCmdSetScissor(
			static_cast<VkCommandBuffer>(m_CommandBuffer),		 // Command Buffer
			0,													 // First Scissor
			1,													 // Scissor Count
			&vkScissor											 // An array of VkScissor structures specifying scissor parameters.
		);
	}

	void RHI_CommandList::SetBufferVertex(const RHI_VertexBuffer* buffer, const uint64_t offset /*= 0*/)
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		if (m_VertexBufferID == buffer->RetrieveObjectID() && m_VertexBufferOffset == offset)
		{
			return;
		}

		VkBuffer vertexBuffers[] = { static_cast<VkBuffer>(buffer->RetrieveResource()) };
		VkDeviceSize offsets[] = { offset };

		// The vertex input binding is updated to start at the offset indicated by pOffsets[i] from the start of the buffer pBuffers[i].
		vkCmdBindVertexBuffers(
			static_cast<VkCommandBuffer>(m_CommandBuffer),	// Command buffer into which the command the recorded.
			0,											    // First Binding - Index of the first input binding whose state is updated by the command.
			1,												// Binding Count - Number of vertex input bindings whose state is updated by the command.
			vertexBuffers,									// Pointer to our Vertex Buffers.
			offsets											// Offsets for our Vertex Buffers.
		);

		/// Profiler
		m_VertexBufferID = buffer->RetrieveObjectID();
		m_VertexBufferOffset = offset;
	}

	void RHI_CommandList::SetBufferIndex(const RHI_IndexBuffer* buffer, const uint64_t offset /*= 0*/)
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		if (m_IndexBufferID == buffer->RetrieveObjectID() && m_IndexBufferOffset == offset)
		{
			return;
		}

		// Essentially an array of pointers into the vertex buffer, allowing us to reorder vertex data, and reuse existing data for multiple vertices.
		vkCmdBindIndexBuffer(
			static_cast<VkCommandBuffer>(m_CommandBuffer),						// Command buffer in which our command is recorded into.
			static_cast<VkBuffer>(buffer->RetrieveResource()),					// Buffer is the buffer being bound. 
			offset,																// Starting offset in bytes within buffer used in index buffer address calculations.
			buffer->Is16Bit() ? VK_INDEX_TYPE_UINT16 : VK_INDEX_TYPE_UINT32	    // Specifies whether the indices are treated as 16 bits or 32 bits. This depends on the number of entries in our vertex buffer. We can use 16 bit if we're using less than 65535 unique vertices.
		);

		/// Profiler
		m_IndexBufferID = buffer->RetrieveObjectID();
		m_IndexBufferOffset = offset;
	}

	bool RHI_CommandList::SetConstantBuffer(const uint32_t slot, const uint8_t scope, RHI_ConstantBuffer* constantBuffer) const
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		if (!m_DescriptorSetLayoutCache->RetrieveCurrentDescriptorSetLayout())
		{
			AMETHYST_WARNING("Descriptor layout not set, try setting constant buffer \"%s\" within a render pass.", constantBuffer->RetrieveObjectName().c_str());
			return false;
		}

		// Set ( will only happen if it's not already set.
		return m_DescriptorSetLayoutCache->SetConstantBuffer(slot, constantBuffer);
	}

	void RHI_CommandList::SetSampler(const uint32_t slot, RHI_Sampler* sampler) const
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		if (!m_DescriptorSetLayoutCache->RetrieveCurrentDescriptorSetLayout())
		{
			AMETHYST_WARNING("Descriptor layout not set, try setting sampler \"%s\" within a render pass.", sampler->RetrieveObjectName().c_str());
			return;
		}

		// Set (will only happen if it's not already set).
		m_DescriptorSetLayoutCache->SetSampler(slot, sampler);
	}

	void RHI_CommandList::SetTexture(const uint32_t slot, RHI_Texture* texture, const bool storage /*= false*/)
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		if (!m_DescriptorSetLayoutCache->RetrieveCurrentDescriptorSetLayout())
		{
			AMETHYST_WARNING("Descriptor layout not set, trying setting texture \"%s\" within a render pass.", texture->RetrieveObjectName().c_str());
			return;
		}

		// Null textures are not allowed, and gets replaced with a black texture here.
		if (!texture || !texture->RetrieveResourceView())
		{
			texture = m_Renderer->RetrieveDefaultTextureTransparent();
		}

		// If the image has an invalid layout (can happen for a few frames during staging), replace with black texture.
		if (texture->RetrieveLayout() == RHI_Image_Layout::Undefined || texture->RetrieveLayout() == RHI_Image_Layout::Preinitialized)
		{
			AMETHYST_WARNING("Can't set texture without a layout.");
			texture = m_Renderer->RetrieveDefaultTextureTransparent();
		}

		// Transition to appropriate layout if needed.
		{
			RHI_Image_Layout targetLayout = RHI_Image_Layout::Undefined;

			if (storage)
			{
				if (!texture->IsStorage())
				{
					AMETHYST_ERROR("Texture %s doesn't support storage.", texture->RetrieveObjectName().c_str());
				}
				else
				{
					// According to Section 13.1 of the Vulkan specification, storage textures have to be in a general layout. See: https://www.khronos.org/registry/vulkan/specs/1.1-extensions/html/vkspec.html#descriptorsets-storageimage
					if (texture->RetrieveLayout() != RHI_Image_Layout::General)
					{
						targetLayout = RHI_Image_Layout::General;
					}
				}
			}
			else
			{
				// Color
				if (texture->IsColorFormat() && texture->RetrieveLayout() != RHI_Image_Layout::Shader_Read_Only_Optimal)
				{
					targetLayout = RHI_Image_Layout::Shader_Read_Only_Optimal;
				}

				// Depth
				if (texture->IsDepthFormat() && texture->RetrieveLayout() != RHI_Image_Layout::Depth_Stencil_Read_Only_Optimal)
				{
					targetLayout = RHI_Image_Layout::Depth_Stencil_Read_Only_Optimal;
				}
			}

			bool transitionRequired = targetLayout != RHI_Image_Layout::Undefined;

			// Transition
			if (transitionRequired && !m_IsRenderPassActive)
			{
				texture->SetLayout(targetLayout, this);
			}
			else if (transitionRequired && m_IsRenderPassActive)
			{
				AMETHYST_WARNING("Can't transition texture to target layout while a render pass is active.");
				texture = m_Renderer->RetrieveDefaultTextureTransparent();
			}
		}

		// Set (will only happen if it's not already set).
		m_DescriptorSetLayoutCache->SetTexture(slot, texture, storage);
	}

	// Retrieve GPU memory used.

	// Timestamp start.

	// Timestamp end.

	// Timestamp duration.

	// GPU Query Create

	// GPU Querty Release

	// Timeblock start.

	// Timeblock end.

	void RHI_CommandList::ResetDescriptorCache()
	{
		if (m_DescriptorSetLayoutCache)
		{
			m_DescriptorSetLayoutCache->ResetDescriptorCache();
		}
	}

	bool RHI_CommandList::Deferred_BeginRenderPass()
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		// Retrieve pipeline state.
		RHI_PipelineState* pipelineState = m_Pipeline->RetrievePipelineState();

		// Validate pipeline state.
		AMETHYST_ASSERT(pipelineState != nullptr);
		AMETHYST_ASSERT(pipelineState->RetrieveRenderPass() != nullptr);
		AMETHYST_ASSERT(pipelineState->RetrieveFramebuffer() != nullptr);

		// Clear values.
		std::array<VkClearValue, g_RHI_MaxRenderTargetCount + 1> clearValues; // +1 for Depth-Stencil.
		uint32_t clearValueCount = 0;
		{
			// Color
			for (uint8_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
			{
				if (m_PipelineState->m_RenderTargetColorTextures[i] != nullptr)
				{
					Math::Vector4& color = m_PipelineState->m_ClearValuesColor[i];
					clearValues[clearValueCount++].color = { color.m_X, color.m_Y, color.m_Z, color.m_W };
				}
			}

			// Depth Stencil
			if (m_PipelineState->m_RenderTargetDepthTexture != nullptr)
			{
				clearValues[clearValueCount++].depthStencil = VkClearDepthStencilValue { m_PipelineState->m_ClearValueDepth, m_PipelineState->m_ClearValueStencil };
			}

			// Swapchain
			if (m_PipelineState->m_RenderTargetSwapchain != nullptr)
			{
				Math::Vector4& color = m_PipelineState->m_ClearValuesColor[0];
				clearValues[clearValueCount++].color = { color.m_X, color.m_Y, color.m_Z, color.m_W };
			}
		}

		// Begin Render Pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = static_cast<VkRenderPass>(pipelineState->RetrieveRenderPass());		// The render pass to begin an instance of.
		renderPassInfo.framebuffer = static_cast<VkFramebuffer>(pipelineState->RetrieveFramebuffer());  // The framebuffer containing the attachments that are to be used with the render pass.
		renderPassInfo.renderArea.offset = { 0, 0 }; // The render area that is affected by the render pass instance.
		renderPassInfo.renderArea.extent.width = pipelineState->RetrieveRenderTargetWidth();
		renderPassInfo.renderArea.extent.height = pipelineState->RetrieveRenderTargetHeight();
		renderPassInfo.clearValueCount = clearValueCount;
		renderPassInfo.pClearValues = clearValues.data(); // Clear values for each attachment. The array is indexed by attachment number. Only elements corresponding to cleared attachments are used.

		// After beginning a render pass instance, the command buffer is ready to record the commands for the first subpass of that render pass.
		// The last parameter is a VkSubpassContents value specifying how the commands in the first subpass will be provided - Inline specifies that the contents of the subpass will be recorded inline in the primary command buffer, and secondary command buffers must not be executed within the subpass. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkSubpassContents.html
		vkCmdBeginRenderPass(static_cast<VkCommandBuffer>(m_CommandBuffer), &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		m_IsRenderPassActive = true;

		return true;
	}

	bool RHI_CommandList::Deferred_BindDescriptorSet()
	{
		// Validate command list state.
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Recording);

		// Descriptor Set != null, Result = true -> A descriptor set must be bound.
		// Descriptor Set == null, Result = true -> A descriptor set is already bound.
		// Descriptor Set == null, Result = false -> A new descriptor was needed but we are out of memory (allocates next frame).

		RHI_DescriptorSet* descriptorSet = nullptr;
		bool result = m_DescriptorSetLayoutCache->RetrieveDescriptorSet(descriptorSet);

		if (result && descriptorSet != nullptr)
		{
			// Bind point.
			VkPipelineBindPoint pipelineBindPoint = m_PipelineState->IsComputePipeline() ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS;

			// Dynamic Offsets
			RHI_DescriptorSetLayout* descriptorSetLayout = m_DescriptorSetLayoutCache->RetrieveCurrentDescriptorSetLayout();
			const std::array<uint32_t, g_RHI_MaxConstantBufferCount> dynamicOffsets = descriptorSetLayout->RetrieveDynamicOffsets();
			uint32_t dynamicOffsetCount = descriptorSetLayout->RetrieveDynamicOffsetCount();

			// Validate descriptor sets.
			std::array<void*, 1> descriptorSets = { descriptorSet->RetrieveResource() };
			for (uint32_t i = 0; i < static_cast<uint32_t>(descriptorSets.size()); i++)
			{
				AMETHYST_ASSERT(descriptorSets[i] != nullptr);
			}

			// Bind descriptor sets to a command buffer. Once bound, a descriptor set affects the rendering of subsequent commands that interact with the given pipeline type in the command buffer until a different set is bound or the set is "disturbed".
			vkCmdBindDescriptorSets(
				static_cast<VkCommandBuffer>(m_CommandBuffer),							// Command buffer the descriptor sets will be bound to.
				pipelineBindPoint,														// Indicates the type of pipeline that will use the descriptors. There is a seperate set of bind points for each pipeline type, so binding one does not disturb the others.
				static_cast<VkPipelineLayout>(m_Pipeline->RetrievePipelineLayout()),	// VkPipelineLayout object used to program the bindings.
				0,																		// firstSet is the set number of the first descriptor set to be bound.
				static_cast<uint32_t>(descriptorSets.size()),							// Number of elements in the pDescriptorSets array.
				reinterpret_cast<VkDescriptorSet*>(descriptorSets.data()),				// Pointer to an array of handles to VkDescriptorSet objects describing the descriptor sets to bind to.
				dynamicOffsetCount,														// Number of dynamic offsets in the pDynamicOffsets array.
				!dynamicOffsets.empty() ? dynamicOffsets.data() : nullptr				// Pointer to an array of uint32_t values specifying dynamic offsets.
			);

			/// Profiler.
		}

		return result;
	}
	
	bool RHI_CommandList::Deferred_BindPipeline()
	{
		if (VkPipeline vkPipeline = static_cast<VkPipeline>(m_Pipeline->RetrievePipeline()))
		{
			// Bind point - Specifies the bind point of a pipeline object to a command buffer.
			// Once bound, a pipeline binding affects subsequent commands that interact with the given pipeline type in the command buffer until a different pipeline of the same type is bound to the bind point.
			VkPipelineBindPoint pipelineBindPoint = m_PipelineState->IsComputePipeline() ? VK_PIPELINE_BIND_POINT_COMPUTE : VK_PIPELINE_BIND_POINT_GRAPHICS;

			// Bind a pipeline to a command buffer.
			vkCmdBindPipeline(static_cast<VkCommandBuffer>(m_CommandBuffer), pipelineBindPoint, vkPipeline);
			/// Profiler
			m_IsPipelineActive = true;
		}
		else
		{
			AMETHYST_ERROR("Invalid pipeline.");
			return false;
		}

		return true;
	}
}