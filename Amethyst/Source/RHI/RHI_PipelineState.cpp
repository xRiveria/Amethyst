#include "Amethyst.h"
#include "RHI_PipelineState.h"
#include "RHI_Shader.h"
#include "RHI_Texture.h"
#include "RHI_SwapChain.h"
#include "RHI_BlendState.h"
#include "RHI_InputLayout.h"
#include "RHI_RasterizerState.h"
#include "RHI_DepthStencilState.h"
#include "../Utilities/Hash.h"

namespace Amethyst
{
	//These can mostly only be filled once the other classes have been created. We will leave it as they are for now.

	RHI_PipelineState::RHI_PipelineState()
	{
		m_FrameBuffers.fill(nullptr);
		m_ClearValuesColor.fill(g_RHI_ColorLoadValue);
	}

	RHI_PipelineState::~RHI_PipelineState()
	{
		DestroyFramebuffer();
	}

	bool RHI_PipelineState::IsPipelineStateValid()
	{
		//Deduce if marking and profiling should occur.
		m_IsProfilingEnabled = m_PassName != nullptr;
		m_Mark = m_PassName != nullptr;

		//Deduce States
		bool hasComputeShader = m_ComputeShader ? m_ComputeShader->IsCompiled() : false;
		bool hasVertexShader = m_VertexShader ? m_VertexShader->IsCompiled() : false;
		bool hasPixelShader = m_PixelShader ? m_PixelShader->IsCompiled() : false;
		bool hasRenderTarget = m_RenderTargetColorTextures[0] || m_RenderTargetDepthTexture; //Check that there is at least one render target.
		bool hasBackbuffer = m_RenderTargetSwapchain;
		bool hasGraphicsStates = m_RasterizerState && m_DepthStencilState && m_PrimitiveTopology != RHI_PrimitiveTopology_Mode::RHI_PrimitiveTopology_Unknown;
		bool isGraphicsPSO = (hasVertexShader || hasPixelShader) && !hasComputeShader;
		bool isComputePSO = hasComputeShader && (!hasVertexShader && !hasPixelShader);

		///
		//Note: Sometimes, a pipeline state is needed just to update a constant buffer, therefore a pipeline state can have no shaders but still be valid.

		//Validate graphics states.
		if (isGraphicsPSO && !hasGraphicsStates)
		{
			AMETHYST_ERROR("Invaliad Graphics PSO (Pipeline State Object). Not all required graphics states have been provided.");
			return false;
		}

		//Validate Render Targets
		if (isGraphicsPSO && !hasRenderTarget && !hasBackbuffer)
		{
			if (!hasRenderTarget && !hasBackbuffer)
			{
				AMETHYST_ERROR("Invalid Graphics PSO (Pipeline State Object). No render targets or backbuffers have been provided.");
				return false;
			}
		}

		return true;
	}

	void RHI_PipelineState::ResetClearValues()
	{
		m_ClearValuesColor.fill(g_RHI_ColorLoadValue);
		m_ClearValueDepth = g_RHI_DepthLoadValue;
		m_ClearValueStencil = g_RHI_StencilLoadValue;
	}

	void RHI_PipelineState::TransitionRenderTargetLayouts(RHI_CommandList* rhi_CommandList) 
	{
		//Color
		{
			//Texture
			for (uint8_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
			{
				if (RHI_Texture* texture = m_RenderTargetColorTextures[i])
				{
					RHI_Image_Layout layout = RHI_Image_Layout::Color_Attachment_Optimal;

					texture->SetLayout(layout, rhi_CommandList);
					m_RenderTargetColorLayoutInitial = layout;
					m_RenderTargetColorLayoutFinal = layout;
				}
			}

			//Swapchain
			if (RHI_SwapChain* swapchain = m_RenderTargetSwapchain)
			{
				RHI_Image_Layout layout = RHI_Image_Layout::Present_Source;

				m_RenderTargetColorLayoutInitial = layout;
				m_RenderTargetColorLayoutFinal = layout;
			}
		}

		//Depth
		if (RHI_Texture* texture = m_RenderTargetDepthTexture)
		{
			RHI_Image_Layout layout = m_RenderTargetDepthTextureReadOnly ? RHI_Image_Layout::Depth_Stencil_Read_Only_Optimal : RHI_Image_Layout::Depth_Stencil_Attachment_Optimal;
			
			texture->SetLayout(layout, rhi_CommandList);

			m_RenderTargetDepthLayoutInitial = layout;
			m_RenderTargetDepthLayoutFinal = layout;
		}
	}

	uint32_t RHI_PipelineState::RetrieveRenderTargetWidth() const
	{
		if (m_RenderTargetSwapchain)
		{
			return m_RenderTargetSwapchain->RetrieveWidth();
		}

		if (m_RenderTargetColorTextures[0])
		{
			return m_RenderTargetColorTextures[0]->RetrieveWidth();
		}

		if (m_RenderTargetDepthTexture)
		{
			return m_RenderTargetDepthTexture->RetrieveWidth();
		}

		return 0;
	}

	uint32_t RHI_PipelineState::RetrieveRenderTargetHeight() const
	{
		if (m_RenderTargetSwapchain)
		{
			return m_RenderTargetSwapchain->RetrieveHeight();
		}

		if (m_RenderTargetColorTextures[0])
		{
			return m_RenderTargetColorTextures[0]->RetrieveHeight();
		}

		if (m_RenderTargetDepthTexture)
		{
			return m_RenderTargetDepthTexture->RetrieveHeight();
		}

		return 0;
	}

	uint32_t RHI_PipelineState::ComputeHash()
	{
		m_Hash = 0;

		Utility::HashCombine(m_Hash, m_IsScissorDynamic);
		Utility::HashCombine(m_Hash, m_Viewport.m_X);
		Utility::HashCombine(m_Hash, m_Viewport.m_Y);
		Utility::HashCombine(m_Hash, m_Viewport.m_Width);
		Utility::HashCombine(m_Hash, m_Viewport.m_Height);
		Utility::HashCombine(m_Hash, m_PrimitiveTopology);
		Utility::HashCombine(m_Hash, m_VertexBufferStride);
		Utility::HashCombine(m_Hash, m_RenderTargetColorTextureArrayIndex);
		Utility::HashCombine(m_Hash, m_RenderTargetDepthStencilTextureArrayIndex);
		Utility::HashCombine(m_Hash, m_RenderTargetSwapchain ? m_RenderTargetSwapchain->RetrieveObjectID() : 0);

		if (!m_IsScissorDynamic)
		{
			Utility::HashCombine(m_Hash, m_Scissor.m_Left);
			Utility::HashCombine(m_Hash, m_Scissor.m_Top);
			Utility::HashCombine(m_Hash, m_Scissor.m_Right);
			Utility::HashCombine(m_Hash, m_Scissor.m_Bottom);
		}

		if (m_RasterizerState)
		{
			Utility::HashCombine(m_Hash, m_RasterizerState->RetrieveObjectID());
		}

		if (m_BlendState)
		{
			Utility::HashCombine(m_Hash, m_BlendState->RetrieveObjectID());
		}

		if (m_DepthStencilState)
		{
			Utility::HashCombine(m_Hash, m_DepthStencilState->RetrieveObjectID());
		}

		//Shaders
		{
			if (m_ComputeShader)
			{
				Utility::HashCombine(m_Hash, m_ComputeShader->RetrieveObjectID());
			}

			if (m_VertexShader)
			{
				Utility::HashCombine(m_Hash, m_VertexShader->RetrieveObjectID());
			}

			if (m_PixelShader)
			{
				Utility::HashCombine(m_Hash, m_PixelShader->RetrieveObjectID());
			}
		}

		//Render Targets
		bool hasRenderTargetColor = false;
		{
			uint8_t loadOperation = 0;

			//Color
			for (uint32_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
			{
				if (RHI_Texture* texture = m_RenderTargetColorTextures[i])
				{
					Utility::HashCombine(m_Hash, texture->RetrieveObjectID());

					loadOperation = m_ClearValuesColor[i] == g_RHI_ColorIgnoreValue ? 0 : m_ClearValuesColor[i] == g_RHI_ColorLoadValue ? 1 : 2;
					Utility::HashCombine(m_Hash, loadOperation);

					hasRenderTargetColor = true;
				}
			}

			//Depth
			if (m_RenderTargetDepthTexture)
			{
				Utility::HashCombine(m_Hash, m_RenderTargetDepthTexture->RetrieveObjectID());
				
				loadOperation = m_ClearValueDepth == g_RHI_DepthIgnoreValue ? 0 : m_ClearValueDepth == g_RHI_DepthLoadValue ? 1 : 2;
				Utility::HashCombine(m_Hash, loadOperation);

				loadOperation = m_ClearValueStencil == g_RHI_StencilIgnoreValue ? 0 : m_ClearValueStencil == g_RHI_StencilLoadValue ? 1 : 2;
				Utility::HashCombine(m_Hash, loadOperation);
			}
		}

		//Initial and Final Layouts
		{
			if (hasRenderTargetColor)
			{
				Utility::HashCombine(m_Hash, m_RenderTargetColorLayoutInitial);
				Utility::HashCombine(m_Hash, m_RenderTargetColorLayoutFinal);
			}

			if (m_RenderTargetDepthTexture)
			{
				Utility::HashCombine(m_Hash, m_RenderTargetDepthLayoutInitial);
				Utility::HashCombine(m_Hash, m_RenderTargetDepthLayoutFinal);
			}
		}

		return m_Hash;
	}
}