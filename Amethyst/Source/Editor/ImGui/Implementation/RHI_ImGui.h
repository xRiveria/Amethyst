#pragma once
#include <vector>
#include "Source/Rendering/Renderer.h"
#include "Source/RHI/RHI_Device.h"
#include "Source/RHI/RHI_Shader.h"
#include "Source/RHI/RHI_Texture2D.h"
#include "Source/RHI/RHI_SwapChain.h"
#include "Source/RHI/RHI_BlendState.h"
#include "Source/RHI/RHI_CommandList.h"
#include "Source/RHI/RHI_IndexBuffer.h"
#include "Source/RHI/RHI_VertexBuffer.h"
#include "Source/RHI/RHI_PipelineState.h"
#include "Source/RHI/RHI_RasterizerState.h"
#include "Source/RHI/RHI_DepthStencilState.h"
#include "Source/RHI/RHI_Semaphore.h"

namespace ImGui::RHI
{
	// RHI Resources
	static std::shared_ptr<Amethyst::RHI_Device> g_RHI_Device;
	
	inline bool Initialize(Amethyst::Context* engineContext)
	{

	}

	inline void Shutdown()
	{

	}
}