#pragma once
#include <array>
#include <atomic>
#include <mutex>
#include "RHI_Utilities.h"
#include "../Core/AmethystObject.h"
#include "../Rendering/RendererEnums.h"

namespace Amethyst
{
	//Forward Declarations
	class Renderer;
	class Context;

	enum RHI_CommandListState : uint8_t
	{
		Idle,
		Recording,
		Ended,
		Submitted
	};

	class RHI_CommandList : public AmethystObject
	{
	public:
		RHI_CommandList(uint32_t index, RHI_SwapChain* swapchain, Context* context);
		~RHI_CommandList();

		//Command List
		bool Begin();
		bool End();
		bool Submit();
		bool Wait();
		bool Reset();
		bool Flush();

		//Render Pass
		bool BeginRenderPass(RHI_PipelineState& pipelineState);
		bool EndRenderPass();

		//Clear
		void ClearPipelineStateRenderTargets(RHI_PipelineState& pipelineState);
		void ClearRenderTarget(RHI_Texture* texture, const uint32_t colorIndex = 0, const uint32_t depthStencilIndex = 0, const bool storage = false, const Math::Vector4& clearColor = g_RHI_ColorLoadValue, const float clearDepth = g_RHI_DepthLoadValue, const uint32_t clearStencil = g_RHI_StencilLoadValue);

		//Draw
		bool Draw(uint32_t vertexCount);
		bool DrawIndexed(uint32_t indexCount, uint32_t indexOffset = 0, uint32_t vertexOffset = 0);

		//Dispatch
		bool Dispatch(uint32_t x, uint32_t y, uint32_t z, bool async = false);

		//Viewport
		void SetViewport(const RHI_Viewport& viewport) const;

		//Scissor
		//void SetScissorRectangle(const Math::Rectangle& scissorRectangle) const;

		//Vertex Buffer
		void SetBufferVertex(const RHI_VertexBuffer* buffer, const uint64_t offset = 0);

		//Index Buffer
		void SetBufferIndex(const RHI_IndexBuffer* buffer, const uint64_t offset = 0);

		//Constant Buffer
		bool SetConstantBuffer(const uint32_t slot, const uint8_t scope, RHI_ConstantBuffer* constantBuffer) const;
		inline bool SetConstantBuffer(const uint32_t slot, const uint8_t scope, const std::shared_ptr<RHI_ConstantBuffer>& constantBuffer) const { return SetConstantBuffer(slot, scope, constantBuffer.get()); }

		//Sampler
		void SetSampler(const uint32_t slot, RHI_Sampler* sampler) const;
		inline void SetSampler(const uint32_t slot, const std::shared_ptr<RHI_Sampler>& sampler) const { SetSampler(slot, sampler.get()); }

		//Texture
		void SetTexture(const uint32_t slot, RHI_Texture* texture, const bool storage = false);
		inline void SetTexture(const RendererBindingsUAV slot, RHI_Texture* texture) { SetTexture(static_cast<uint32_t>(slot), texture, true); }
		inline void SetTexture(const RendererBindingsUAV slot, const std::shared_ptr<RHI_Texture>& texture) { SetTexture(static_cast<uint32_t>(slot), texture.get(), true); }
		inline void SetTexture(const RendererBindingsSRV slot, RHI_Texture* texture) { SetTexture(static_cast<uint32_t>(slot), texture, false); }
		inline void SetTexture(const RendererBindingsSRV slot, const std::shared_ptr<RHI_Texture>& texture) { SetTexture(static_cast<uint32_t>(slot), texture.get(), false); }

		//Timestamps & GPU Stuff

		//Misc
		void ResetDescriptorCache();
		void* RetrieveCommandBuffer() const { return m_CommandBuffer; }
		RHI_Semaphore* RetrieveProcessedSemaphore() { return m_ProcessedSemaphore.get(); }
		const RHI_CommandListState RetrieveCommandListState() const { return m_CommandListState; }

	private:
		void Deferred_BeginRenderPass();
		bool Deferred_BindPipeline();
		bool Deferred_BindDescriptorSet();
		bool OnDraw();

	private:
		Renderer* m_Renderer = nullptr;
		RHI_Pipeline* m_Pipeline = nullptr;
		RHI_PipelineCache* m_PipelineCache = nullptr;
		RHI_PipelineState* m_PipelineState = nullptr;
		RHI_SwapChain* m_SwapChain = nullptr;
		RHI_DescriptorSetLayoutCache* m_DescriptorSetLayoutCache = nullptr;
		RHI_Device* m_Device = nullptr;
		//Profiler
		void* m_CommandBuffer;

		std::shared_ptr<RHI_Fence> m_ProcessedFence = nullptr;
		std::shared_ptr<RHI_Semaphore> m_ProcessedSemaphore = nullptr;
		void* m_QueryPool = nullptr;

		std::atomic<bool> m_IsRenderPassActive = false;
		std::atomic<bool> m_IsPipelineActive = false;
		std::atomic<bool> m_Flushed = false;
		std::atomic<RHI_CommandListState> m_CommandListState = RHI_CommandListState::Idle;
		static bool m_MemoryQuerySupport;
		std::mutex m_MutexReset;

		//Profiling Stuff

		//Variables to minimise state changes.
		uint32_t m_VertexBufferID = 0;
		uint32_t m_VertexBufferOffset = 0;
		uint32_t m_IndexBufferID = 0;
		uint32_t m_IndexBufferOffset = 0;
	};
}