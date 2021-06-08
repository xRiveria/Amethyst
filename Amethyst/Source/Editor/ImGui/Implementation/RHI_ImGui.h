#pragma once
#include <vector>
#include "Source/Core/AmethystDefinitions.h"
#include "../Source/imgui.h"
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
	using namespace Amethyst;

	// Engine Subsystems
	Amethyst::Context* g_EngineContext = nullptr;
	Amethyst::Renderer* g_Renderer = nullptr;

	// RHI Resources
	static std::shared_ptr<Amethyst::RHI_Device> g_RHI_Device;
	static std::unique_ptr<Amethyst::RHI_Texture> g_RHI_Texture;
	static std::unordered_map<uint32_t, std::vector<std::unique_ptr<Amethyst::RHI_VertexBuffer>>> g_VertexBuffers;
	static std::unordered_map<uint32_t, std::vector<std::unique_ptr<Amethyst::RHI_IndexBuffer>>> g_IndexBuffers;
	static std::unique_ptr<Amethyst::RHI_DepthStencilState> g_DepthStencilState;
	static std::unique_ptr<Amethyst::RHI_RasterizerState> g_RasterizerState;
	static std::unique_ptr<Amethyst::RHI_BlendState> g_BlendState;
	static std::unique_ptr<Amethyst::RHI_Shader> g_ShaderVertex;
	static std::unique_ptr<Amethyst::RHI_Shader> g_PixelShader;
	
	inline bool Initialize(Amethyst::Context* engineContext)
	{

	}

	inline void Shutdown()
	{
		DestroyPlatformWindows();
	}

	inline void Render(ImDrawData* drawData, Amethyst::RHI_SwapChain* swapchainOther = nullptr, const bool clear = true)
	{
		// Validate Draw Data.
		AMETHYST_ASSERT(drawData != nullptr);

		// Avoid rendering when minimized/scale coordinates for retina displays (screen coordinates != framebuffer coordinates).
		const int framebufferWidth = static_cast<int>(drawData->DisplaySize.x * drawData->FramebufferScale.x);
		const int framebufferHeight = static_cast<int>(drawData->DisplaySize.y * drawData->FramebufferScale.y);

		// If any of the below is invalid, return (no rendering).
		if (framebufferWidth <= 0 || framebufferHeight <= 0 || drawData->TotalVtxCount == 0)
		{
			return;
		}

		// Retrieve Swapchain.
		bool isChildWindow = swapchainOther != nullptr;
		RHI_SwapChain* swapchain = isChildWindow ? swapchainOther : g_Renderer->RetrieveSwapChain();
		RHI_CommandList* commandList = swapchain->RetrieveCommandList();

		// Validate Command List.
		AMETHYST_ASSERT(commandList != nullptr);

		// Wait until the Renderer is able to render (it can get flushed and stopped during world loading).

	}
}