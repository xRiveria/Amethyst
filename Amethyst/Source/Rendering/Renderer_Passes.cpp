#include "Amethyst.h"
#include "Renderer.h"
#include "Model.h"
#include "../RHI/RHI_CommandList.h"
#include "../RHI/RHI_Implementation.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "../RHI/RHI_PipelineState.h"
#include "../RHI/RHI_Texture.h"
#include "../RHI/RHI_SwapChain.h"
#include "../Runtime/ECS/Entity.h"
#include "../Runtime/ECS/Components/Camera.h"
#include "../Runtime/ECS/Components/Transform.h"
#include "../Runtime/ECS/Components/Renderable.h"

namespace Amethyst
{
	void Renderer::SetGlobalSamplersAndConstantBuffers(RHI_CommandList* commandList) const
	{
		// Constant Buffers
		commandList->SetConstantBuffer(0, RHI_Shader_Vertex | RHI_Shader_Pixel | RHI_Shader_Compute, m_BufferFrame_GPU);
		/// Material Constant Buffer
		commandList->SetConstantBuffer(2, RHI_Shader_Vertex | RHI_Shader_Pixel | RHI_Shader_Compute, m_BufferUber_GPU);
		/// Light Constant Buffer

		// Samplers
		commandList->SetSampler(0, m_Sampler_CompareDepth);
		commandList->SetSampler(1, m_Sampler_PointClamp);
		commandList->SetSampler(2, m_Sampler_PointWrap);
		commandList->SetSampler(3, m_Sampler_BilinearClamp);
		commandList->SetSampler(4, m_Sampler_BilinearWrap);
		commandList->SetSampler(5, m_Sampler_TrilinearClamp);
		commandList->SetSampler(6, m_Sampler_AnisotropicWrap);
	}

	void Renderer::Pass_Main(RHI_CommandList* commandList)
	{
		// Validate Command List.
		AMETHYST_ASSERT(commandList != nullptr);
		AMETHYST_ASSERT(commandList->RetrieveCommandListState() == RHI_CommandListState::Recording);

		// Update frame constant buffer.
		Pass_UpdateFrameBuffer(commandList);

		/// Generate BRDF LUT.

		/// Transparency Stuff

		/// Depth/Lighting Stuff

		// Acquire Render Targets
		RHI_Texture* renderTarget1 = RENDER_TARGET(Renderer_RenderTargetType::Frame).get();
		RHI_Texture* renderTarget2 = RENDER_TARGET(Renderer_RenderTargetType::Frame_2).get();

		// G-Buffer & Lighting
		{
			// G-Buffer
			Pass_GBuffer(commandList);

			/// Passes that rely on the G-Buffer.
			/// Lighting
			/// Transparent Objects Stuff
		}

		/// Pass for Post Processing
		Pass_PostProcess(commandList);
	}

	void Renderer::Pass_UpdateFrameBuffer(RHI_CommandList* commandList)
	{
		// Set Render State
		static RHI_PipelineState pipelineStateObject;
		pipelineStateObject.m_PassName = "Pass_UpdateFrameBuffer";

		// Draw
		if (commandList->BeginRenderPass(pipelineStateObject))
		{
			UpdateFrameBuffer(commandList);
			commandList->EndRenderPass();
		}
	}

	void Renderer::Pass_GBuffer(RHI_CommandList* commandList, const bool isTransparentPass /*= false*/)
	{
		// Acquire required resources/shaders.
		RHI_Texture* textureAlbedo   = RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Albedo).get();
		RHI_Texture* textureNormal   = RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Normal).get();
		RHI_Texture* textureMaterial = RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Material).get();
		RHI_Texture* textureVelocity = RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Velocity).get();
		RHI_Texture* textureDepth    = RENDER_TARGET(Renderer_RenderTargetType::GBuffer_Depth).get();

	}

	void Renderer::Pass_PostProcess(RHI_CommandList* commandList)
	{

	}

	void Renderer::Pass_Lines(RHI_CommandList* commandList, RHI_Texture* textureOut)
	{

	}
}