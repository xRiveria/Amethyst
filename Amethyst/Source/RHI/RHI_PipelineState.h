#pragma once
#include "RHI_Utilities.h"
#include "RHI_Viewport.h"
#include "../Core/AmethystObject.h"
#include "../Runtime/Math/Rectangle.h"
#include <array>

namespace Amethyst
{
	class RHI_PipelineState : public AmethystObject
	{
	public:
		RHI_PipelineState();
		~RHI_PipelineState();

		bool IsPipelineStateValid();

		//Framebuffer
		bool CreateFramebuffer(const RHI_Device* rhi_Device);
		void* RetrieveFramebuffer() const;
		void TransitionRenderTargetLayouts(RHI_CommandList* rhi_CommandList);
		uint32_t RetrieveRenderTargetWidth() const;
		uint32_t RetrieveRenderTargetHeight() const;

		//Hash
		uint32_t ComputeHash();
		uint32_t RetrieveHash() const { return m_Hash; }
		void ResetClearValues();

		bool IsGraphicsPipeline() const { return (m_VertexShader != nullptr || m_PixelShader != nullptr) && !m_ComputeShader; }
		bool IsComputePipeline() const { return m_ComputeShader != nullptr && !IsGraphicsPipeline(); }
		bool IsDummyPipeline() const { return !m_ComputeShader && !m_VertexShader && !m_PixelShader; }
		void* RetrieveRenderPass() const { return m_RenderPass; }

	public:
		//Render Targets
		RHI_Texture* m_RenderTargetDepthTexture = nullptr;
		std::array<RHI_Texture*, g_RHI_MaxRenderTargetCount> m_RenderTargetColorTextures =
		{
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		};

		//Render Target Indices (Affects Render Pass)
		uint32_t m_RenderTargetColorTextureArrayIndex = 0;
		uint32_t m_RenderTargetDepthStencilTextureArrayIndex = 0;

		//Clear Values
		float m_ClearValueDepth = g_RHI_DepthLoadValue;
		uint32_t m_ClearValueStencil = g_RHI_StencilLoadValue;
		std::array<Math::Vector4, g_RHI_MaxRenderTargetCount> m_ClearValuesColor;

		//Dynamic, Modification is Free
		bool m_RenderTargetDepthTextureReadOnly = false;

		//Constant buffer slots which refer to dynamic buffers (-1 means unused).
		std::array<int, g_RHI_MaxConstantBufferCount> m_DynamicConstantBufferSlots =
		{
			0, 1, 2, 3, 4, -1, -1, -1
		};


		//Static, modification here may potentially generate a new pipeline.
		RHI_Shader* m_VertexShader = nullptr;
		RHI_Shader* m_PixelShader = nullptr;
		RHI_Shader* m_ComputeShader = nullptr;
		RHI_SwapChain* m_RenderTargetSwapchain = nullptr;
        RHI_Viewport m_Viewport = RHI_Viewport::Undefined;
		Math::Rectangle m_Scissor = Math::Rectangle::Zero;

		//States
		RHI_RasterizerState* m_RasterizerState = nullptr;
		RHI_BlendState* m_BlendState = nullptr;
		RHI_DepthStencilState* m_DepthStencilState = nullptr;
		RHI_PrimitiveTopology_Mode m_PrimitiveTopology = RHI_PrimitiveTopology_Mode::RHI_PrimitiveTopology_Unknown;

		//Scissor
		bool m_DynamicScissor = false;
		uint32_t m_VertexBufferStride = 0;

		//Profiling
		const char* m_PassName = nullptr;
		bool m_Mark = false;
		bool m_IsProfilingEnabled = false;

	private:
		void DestroyFramebuffer();

	private:
		RHI_Image_Layout m_RenderTargetColorLayoutInitial = RHI_Image_Layout::Undefined;
		RHI_Image_Layout m_RenderTargetColorLayoutFinal = RHI_Image_Layout::Undefined;
		RHI_Image_Layout m_RenderTargetDepthLayoutInitial = RHI_Image_Layout::Undefined;
		RHI_Image_Layout m_RenderTargetDepthLayoutFinal = RHI_Image_Layout::Undefined;

		uint32_t m_Hash = 0;
		void* m_RenderPass = nullptr;
		std::array<void*, g_RHI_MaxConstantBufferCount> m_FrameBuffers;

		//Dependencies
		const RHI_Device* m_RHI_Device = nullptr;
		
 	};
}