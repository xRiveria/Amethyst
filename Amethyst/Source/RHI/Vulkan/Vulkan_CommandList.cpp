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
			return;
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


}