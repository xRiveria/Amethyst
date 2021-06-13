#include "Amethyst.h"
#include "Renderer.h"
#include "../Resource/ResourceCache.h"
#include "../RHI/RHI_Texture2D.h"
#include "../RHI/RHI_Shader.h"
#include "../RHI/RHI_Sampler.h"
#include "../RHI/RHI_BlendState.h"
#include "../RHI/RHI_ConstantBuffer.h"
#include "../RHI/RHI_RasterizerState.h"
#include "../RHI/RHI_DepthStencilState.h"
#include "../RHI/RHI_SwapChain.h"

namespace Amethyst
{
	void Renderer::CreateConstantBuffers()
	{
		bool isDynamicConstantBuffer = true;

		// Frame
		m_BufferFrame_GPU = std::make_shared<RHI_ConstantBuffer>(m_RHI_Device, "Frame", isDynamicConstantBuffer);
		m_BufferFrame_GPU->Create<BufferFrame>(m_SwapChainBufferCount);

		/// Material GPU

		m_BufferUber_GPU = std::make_shared<RHI_ConstantBuffer>(m_RHI_Device, "Uber", isDynamicConstantBuffer);
		m_BufferUber_GPU->Create<BufferUber>(64);

		/// Light GPU
	}

	void Renderer::CreateDepthStencilStates()
	{
		m_DepthStencil_Off_Off		   = std::make_shared<RHI_DepthStencilState>(m_RHI_Device, false, false, RHI_Comparison_Function::RHI_Comparison_Never, false, false, RHI_Comparison_Function::RHI_Comparison_Never); // No Depth or Stencil 
		m_DepthStencil_ReadWrite_Off   = std::make_shared<RHI_DepthStencilState>(m_RHI_Device, true, true,   RetrieveDepthComparisonFunction(), false, false, RHI_Comparison_Function::RHI_Comparison_Never);             // Depth Read/Write & No Stencil
		m_DepthStencil_Read_Off        = std::make_shared<RHI_DepthStencilState>(m_RHI_Device, true, false,  RetrieveDepthComparisonFunction(), false, false, RHI_Comparison_Function::RHI_Comparison_Never);             // Depth Read & No Stencil
		m_DepthStencil_Off_Read        = std::make_shared<RHI_DepthStencilState>(m_RHI_Device, false, false, RHI_Comparison_Function::RHI_Comparison_Never, true, false, RHI_Comparison_Function::RHI_Comparison_Equal);  // No Depth, Stencil Read
		m_DepthStencil_ReadWrite_Write = std::make_shared<RHI_DepthStencilState>(m_RHI_Device, true, true,   RetrieveDepthComparisonFunction(), false, true, RHI_Comparison_Function::RHI_Comparison_Always);			  // Depth Read/Write, Stencil Write
	}

	void Renderer::CreateRasterizerStates()
	{
		/// float depthBias = RetrieveRendererOption(RendererOption::Render_ReverseZ) ? -m_DepthBias : m_DepthBias;
		/// float depthBiasSlopeScaled
		
		m_Rasterizer_CullBackSolid     = std::make_shared<RHI_RasterizerState>(m_RHI_Device, RHI_Cull_Back, RHI_Fill_Solid, true, false, false);
		m_Rasterizer_CullBackWireframe = std::make_shared<RHI_RasterizerState>(m_RHI_Device, RHI_Cull_Back, RHI_Fill_Wireframe, true, false, true);
		/// Light Point Spot
		/// Light Directional
	}

	void Renderer::CreateBlendStates()
	{
		// Blend Enabled, Source Blend Factor, Destination Blend Factor, Blend Operation, Alpha Source Blend, Alpha Destination Blend, Blend Operation alpha, Blend Factor
		m_Blend_Disabled = std::make_shared<RHI_BlendState>(m_RHI_Device, false);
		m_Blend_Alpha    = std::make_shared<RHI_BlendState>(m_RHI_Device, true, RHI_Blend_Source_Alpha, RHI_Blend_Inverse_Source_Alpha, RHI_Blend_Operation_Add, RHI_Blend_One, RHI_Blend_One, RHI_Blend_Operation_Add);
		m_Blend_Additive = std::make_shared<RHI_BlendState>(m_RHI_Device, true, RHI_Blend_One, RHI_Blend_One, RHI_Blend_Operation_Add, RHI_Blend_One, RHI_Blend_One, RHI_Blend_Operation_Add);
	}

	void Renderer::CreateSamplers()
	{
		m_Sampler_CompareDepth    = std::make_shared<RHI_Sampler>(m_RHI_Device, SAMPLER_BILINEAR,  RHI_Sampler_Address_Clamp, RetrieveRendererOption(Render_ReverseZ) ? RHI_Comparison_Greater : RHI_Comparison_Less, false, true);
		m_Sampler_PointClamp      = std::make_shared<RHI_Sampler>(m_RHI_Device, SAMPLER_POINT,     RHI_Sampler_Address_Clamp, RHI_Comparison_Always);
		m_Sampler_PointWrap       = std::make_shared<RHI_Sampler>(m_RHI_Device, SAMPLER_POINT,     RHI_Sampler_Address_Wrap,  RHI_Comparison_Always);
		m_Sampler_BilinearClamp   = std::make_shared<RHI_Sampler>(m_RHI_Device, SAMPLER_BILINEAR,  RHI_Sampler_Address_Clamp, RHI_Comparison_Always);
		m_Sampler_BilinearWrap    = std::make_shared<RHI_Sampler>(m_RHI_Device, SAMPLER_BILINEAR,  RHI_Sampler_Address_Wrap,  RHI_Comparison_Always);
		m_Sampler_TrilinearClamp  = std::make_shared<RHI_Sampler>(m_RHI_Device, SAMPLER_TRILINEAR, RHI_Sampler_Address_Clamp, RHI_Comparison_Always);
		m_Sampler_AnisotropicWrap = std::make_shared<RHI_Sampler>(m_RHI_Device, SAMPLER_TRILINEAR, RHI_Sampler_Address_Wrap,  RHI_Comparison_Always, true);
	}
	
	void Renderer::CreateRenderTextures(const bool createRenderResolutions, const bool createOutputResolutions, const bool createFixedResolutions, const bool createDynamicResolutions)
	{
		// Retrieve Render Resolution
		uint32_t renderWidth = static_cast<uint32_t>(m_ResolutionRender.m_X);
		uint32_t renderHeight = static_cast<uint32_t>(m_ResolutionRender.m_Y);

		// Retrieve Output Resolution
		uint32_t outputWidth = static_cast<uint32_t>(m_ResolutionOutput.m_X);
		uint32_t outputHeight = static_cast<uint32_t>(m_ResolutionOutput.m_Y);

		// Ensure none of the textures is being used by the GPU.
		FlushRenderer();

		// Render Resolution
		if (createRenderResolutions)
		{
			// Full Resolution
			RENDER_TARGET(Renderer_RenderTargetType::Frame) =            std::make_unique<RHI_Texture2D>(m_EngineContext, renderWidth, renderHeight, RHI_Format_R16G16B16A16_Float,   1, 0,								    "RenderTarget_Frame");
			RENDER_TARGET(Renderer_RenderTargetType::Frame_2) =          std::make_unique<RHI_Texture2D>(m_EngineContext, renderWidth, renderHeight, RHI_Format_R16G16B16A16_Float,   1, 0,								    "RenderTarget_Frame2");
			RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Albedo) =   std::make_shared<RHI_Texture2D>(m_EngineContext, renderWidth, renderHeight, RHI_Format_R8G8B8A8_Unorm,		  1, 0,								    "RenderTarget_GBuffer_Albedo");
			RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Normal) =   std::make_shared<RHI_Texture2D>(m_EngineContext, renderWidth, renderHeight, RHI_Format_R16G16B16A16_Float,   1, 0,								    "RenderTarget_GBuffer_Normal");
			RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Material) = std::make_shared<RHI_Texture2D>(m_EngineContext, renderWidth, renderHeight, RHI_Format_R8G8B8A8_Unorm,		  1, 0,								    "RenderTarget_GBuffer_Material");
			RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Velocity) = std::make_shared<RHI_Texture2D>(m_EngineContext, renderWidth, renderHeight, RHI_Format_R16G16_Float,		  1, 0,									"RenderTarget_GBuffer_Velocity");
			RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Depth) =    std::make_shared<RHI_Texture2D>(m_EngineContext, renderWidth, renderHeight, RHI_Format_D32_Float_S8X24_Uint, 1, RHI_Texture_DepthStencilReadOnly,  "RenderTarget_GBuffer_Depth");
		
			/// Half Resolution

			/// Bloom
		}

		// Output Resolution
		if (createOutputResolutions)
		{
			RENDER_TARGET(Renderer_RenderTargetType::Frame_PostProcess)  = std::make_unique<RHI_Texture2D>(m_EngineContext, outputWidth, outputHeight, RHI_Format_R16G16B16A16_Float, 1, 0, "RenderTarget_PostProcess");
			RENDER_TARGET(Renderer_RenderTargetType::Frame_PostProcess2) = std::make_unique<RHI_Texture2D>(m_EngineContext, outputWidth, outputHeight, RHI_Format_R16G16B16A16_Float, 1, 0, "RenderTarget_PostProcess_2");
		}

		/// Fixed Resolution - BRDF Specular LUT

		/// Dynamic Resolution (TAA)
	}

	void Renderer::CreateShaders()
	{
		// Compile asynchronously?
		bool compileAsync = true;

		// Retrieve standard shader directory.
		const std::string shaderDirectory = m_ResourceCache->RetrieveResourceDirectory(ResourceDirectory::Shaders) + "\\";

		// 
	}

	void Renderer::CreateFonts()
	{
		/// Fonts.
	}

	void Renderer::CreateDefaultTextures()
	{
		/// Retrieve Standard Texture Directory. We upload our default textures here.
	}
}