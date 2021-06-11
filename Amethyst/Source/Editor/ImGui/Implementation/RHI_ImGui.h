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

	// Forward Declarations
	void InitializePlatformInterface();

	// Engine Subsystems
	Amethyst::Context* g_EngineContext = nullptr;
	Amethyst::Renderer* g_Renderer = nullptr;

	// RHI Resources
	static std::shared_ptr<Amethyst::RHI_Device> g_RHI_Device;
	static std::unique_ptr<Amethyst::RHI_Texture> g_FontAtlas;
	static std::unordered_map<uint32_t, std::vector<std::unique_ptr<Amethyst::RHI_VertexBuffer>>> g_VertexBuffers;
	static std::unordered_map<uint32_t, std::vector<std::unique_ptr<Amethyst::RHI_IndexBuffer>>> g_IndexBuffers;
	static std::unique_ptr<Amethyst::RHI_DepthStencilState> g_DepthStencilState;
	static std::unique_ptr<Amethyst::RHI_RasterizerState> g_RasterizerState;
	static std::unique_ptr<Amethyst::RHI_BlendState> g_BlendState;
	static std::unique_ptr<Amethyst::RHI_Shader> g_ShaderVertex;
	static std::unique_ptr<Amethyst::RHI_Shader> g_ShaderPixel;

	inline bool Initialize(Amethyst::Context* engineContext)
	{
		g_EngineContext = engineContext;
		g_Renderer = engineContext->RetrieveSubsystem<Renderer>();
		g_RHI_Device = g_Renderer->RetrieveRHIDevice();

		if (!g_EngineContext || !g_RHI_Device || !g_RHI_Device->IsInitialized())
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return false;
		}

		// Create required RHI objects.
		{
			g_DepthStencilState = std::make_unique<RHI_DepthStencilState>(g_RHI_Device, false, g_Renderer->RetrieveDepthComparisonFunction());
			g_RasterizerState = std::make_unique<RHI_RasterizerState>
				(
					g_RHI_Device,
					RHI_Cull_None,
					RHI_Fill_Solid,
					true,  // Depth Clip
					true,  // Scissor
					false, // Multi-Sample
					false  // Anti-Aliasing Lines
					);

			g_BlendState = std::make_unique<RHI_BlendState>
				(
					g_RHI_Device,
					true,
					RHI_Blend_Source_Alpha,				// Source Blend
					RHI_Blend_Inverse_Source_Alpha,		// Destination Blend
					RHI_Blend_Operation_Add,			// Blend Operation
					RHI_Blend_Inverse_Source_Alpha,		// Source Blend Alpha
					RHI_Blend_Zero,						// Destination Blend Alpha
					RHI_Blend_Operation_Add				// Destination Operation Alpha
					);

			// Compile Shaders
			const std::string shaderPath = g_EngineContext->RetrieveSubsystem<ResourceCache>()->RetrieveResourceDirectory(ResourceDirectory::Shaders) + "\\ImGui.hlsl";
			g_ShaderVertex = std::make_unique<RHI_Shader>(g_EngineContext, RHI_Vertex_Type::RHI_Vertex_Type_Position2DTextureColor8);
			bool isAsync = false;
			g_ShaderVertex->Compile(RHI_Shader_Vertex, shaderPath, isAsync);
			g_ShaderPixel = std::make_unique<RHI_Shader>(g_EngineContext);
			g_ShaderPixel->Compile(RHI_Shader_Pixel, shaderPath, isAsync);
		}

		// Font Atlas
		{
			unsigned char* pixels;
			int atlasWidth, atlasHeight, bytesPerPixel;
			ImGuiIO& io = ImGui::GetIO();
			io.Fonts->GetTexDataAsRGBA32(&pixels, &atlasWidth, &atlasHeight, &bytesPerPixel);

			// Copy Pixel Data
			const unsigned int size = atlasWidth * atlasHeight * bytesPerPixel; // Returns the number of bytes that make up our texture data.
			std::vector<std::byte> data(size); // Create a vector with n default-initialized values.
			data.reserve(size); // Make our vector contain
			memcpy(&data[0], reinterpret_cast<std::byte*>(pixels), size);

			// Upload texture to graphics system.
			g_FontAtlas = std::make_unique<RHI_Texture2D>(g_EngineContext, atlasWidth, atlasHeight, RHI_Format_R8G8B8A8_Unorm, data);
			io.Fonts->TexID = static_cast<ImTextureID>(g_FontAtlas.get());
		}

		// Setup back-end capabilities flags.
		ImGuiIO& io = ImGui::GetIO();
		io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports; // Backend renderer supports multiple viewports.
		io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset; // Backended renderer supports ImDrawCmd::VtxOffset. This enables output of large meshs (64k+ vertices) while still using 16-bit indices.
		io.BackendRendererName = "RHI";

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			InitializePlatformInterface();
		}

		return true;
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
		while (!g_Renderer->IsAllowedToRender())
		{
			AMETHYST_INFO("Waiting for the Renderer to be ready...");
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}

		// Don't render if the command list is not recording (can happen when the Renderer is not allowed to render).
		if (commandList->RetrieveCommandListState() != Amethyst::RHI_CommandListState::Recording)
		{
			AMETHYST_INFO("Waiting for the command list to be ready...");
			return;
		}

		// Update Vertex and Index buffer.
		RHI_VertexBuffer* vertexBuffer = nullptr;
		RHI_IndexBuffer* indexBuffer = nullptr;
		{
			const uint32_t swapchainID = swapchain->RetrieveObjectID();
			const uint32_t swapchainCommandListIndex = swapchain->RetrieveCommandListIndex();

			const uint32_t gap = Math::Utilities::Clamp<uint32_t>((swapchainCommandListIndex + 1) - static_cast<uint32_t>(g_VertexBuffers[swapchainID].size()), 0, 10);
			for (uint32_t i = 0; i < gap; i++)
			{
				g_VertexBuffers[swapchainID].emplace_back(std::make_unique<RHI_VertexBuffer>(g_RHI_Device, static_cast<uint32_t>(sizeof(ImDrawVert))));
				g_IndexBuffers[swapchainID].emplace_back(std::make_unique<RHI_IndexBuffer>(g_RHI_Device));
			}

			vertexBuffer = g_VertexBuffers[swapchainID][swapchainCommandListIndex].get();
			indexBuffer = g_IndexBuffers[swapchainID][swapchainCommandListIndex].get();

			// Grow Vertex Buffer as Needed.
			if (vertexBuffer->RetrieveVertexCount() < static_cast<unsigned int>(drawData->TotalVtxCount))
			{
				const unsigned int newSize = drawData->TotalVtxCount + 5000;
				if (!vertexBuffer->CreateDynamic<ImDrawVert>(newSize))
				{
					return;
				}
			}

			// Grow Index Buffer as Needed.
			if (indexBuffer->RetrieveIndexCount() < static_cast<unsigned int>(drawData->TotalIdxCount))
			{
				const unsigned int newSize = drawData->TotalIdxCount + 10000;
				if (!indexBuffer->CreateDynamic<ImDrawIdx>(newSize))
				{
					return;
				}
			}

			// Copy and convert all vertices into a single contigous buffer.
			ImDrawVert* vertexDestination = static_cast<ImDrawVert*>(vertexBuffer->Map());
			ImDrawIdx*  indexDestination  = static_cast<ImDrawIdx*>(indexBuffer->Map());
			if (vertexDestination && indexDestination)
			{
				for (auto i = 0; i < drawData->CmdListsCount; i++)
				{
					const ImDrawList* commandList = drawData->CmdLists[i];
					memcpy(vertexDestination, commandList->VtxBuffer.Data, commandList->VtxBuffer.Size * sizeof(ImDrawVert));
					memcpy(indexDestination,  commandList->IdxBuffer.Data, commandList->IdxBuffer.Size * sizeof(ImDrawIdx));
					vertexDestination += commandList->VtxBuffer.Size;
					indexDestination += commandList->IdxBuffer.Size;
				}

				vertexBuffer->Unmap();
				indexBuffer->Unmap();
			}
		}

		// Set Render State
		static RHI_PipelineState pipelineState = {};
		pipelineState.m_VertexShader		   = g_ShaderVertex.get();
		pipelineState.m_PixelShader			   = g_ShaderPixel.get();
		pipelineState.m_RasterizerState		   = g_RasterizerState.get();
		pipelineState.m_BlendState		       = g_BlendState.get();
		pipelineState.m_DepthStencilState      = g_DepthStencilState.get();
		pipelineState.m_VertexBufferStride     = vertexBuffer->RetrieveStride();
		pipelineState.m_RenderTargetSwapchain  = swapchain;
		pipelineState.m_ClearValuesColor[0]    = clear ? Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f) : g_RHI_ColorLoadValue;
		pipelineState.m_Viewport.m_Width       = drawData->DisplaySize.x;
		pipelineState.m_Viewport.m_Height      = drawData->DisplaySize.y;
		pipelineState.m_IsScissorDynamic	   = true;
		pipelineState.m_PrimitiveTopology	   = RHI_PrimitiveTopology_TriangleList;
		pipelineState.m_PassName			   = isChildWindow ? "Pass_ImGui_ChildWindow" : "Pass_ImGui_MainWindow";

		// Record Commands
		if (commandList->BeginRenderPass(pipelineState))
		{
			{
				// Setup orthographic projection matrix into our constant buffer.
				// Our visible ImGui space lies from drawData->DisplayPos (top left) to drawData->DisplayPos + drawData->DisplaySize (bottom right. DisplayMin is (0, 0) for single viewport applications.
				const float L = drawData->DisplayPos.x;
				const float R = drawData->DisplayPos.x + drawData->DisplaySize.x;
				const float T = drawData->DisplayPos.y;
				const float B = drawData->DisplayPos.y + drawData->DisplaySize.y;

				const Math::Matrix worldviewProjection = Math::Matrix
				(
					2.0f / (R - L), 0.0f, 0.0f, (R + L) / (L - R),
					0.0f, 2.0f / (T - B), 0.0f, (T + B) / (B - T),
					0.0f, 0.0f, 0.5f, 0.5f,
					0.0f, 0.0f, 0.0f, 1.0f
				);

				g_Renderer->SetGlobalShaderObjectTransform(commandList, worldviewProjection);
			}

			// Transition Layers
			for (auto i = 0; i < drawData->CmdListsCount; i++)
			{
				auto commandListImGui = drawData->CmdLists[i];
				for (int commandListIndex = 0; commandListIndex < commandListImGui->CmdBuffer.Size; commandListIndex++)
				{
					const auto pcmd = &commandListImGui->CmdBuffer[commandListIndex];
					if (Amethyst::RHI_Texture* texture = static_cast<Amethyst::RHI_Texture*>(pcmd->TextureId))
					{
						texture->SetLayout(RHI_Image_Layout::Shader_Read_Only_Optimal, commandList);
					}
				}
			}

			commandList->SetBufferVertex(vertexBuffer);
			commandList->SetBufferIndex(indexBuffer);

			// Render Command Lists
			int globalVertexOffset = 0;
			int globalIndexOffset = 0;
			const ImVec2& clipOffset = drawData->DisplayPos;
			Math::Rectangle scissorRect;

			for (int i = 0; i < drawData->CmdListsCount; i++)
			{
				ImDrawList* commandListImGui = drawData->CmdLists[i];
				for (int commandListIndex = 0; commandListIndex < commandListImGui->CmdBuffer.Size; commandListIndex++)
				{
					const ImDrawCmd* commandPointer = &commandListImGui->CmdBuffer[commandListIndex];
					if (commandPointer->UserCallback != nullptr)
					{
						commandPointer->UserCallback(commandListImGui, commandPointer);
					}
					else
					{
						// Compute Scissor Rectangle
						scissorRect.m_Left = commandPointer->ClipRect.x - clipOffset.x;
						scissorRect.m_Top = commandPointer->ClipRect.y - clipOffset.y;
						scissorRect.m_Right = commandPointer->ClipRect.z - clipOffset.x;
						scissorRect.m_Bottom = commandPointer->ClipRect.w - clipOffset.y;

						// Apply scissor rectangle, bind texture and draw.
						commandList->SetScissorRectangle(scissorRect);
						commandList->SetTexture(RendererBindingsSRV::Tex, static_cast<Amethyst::RHI_Texture*>(commandPointer->TextureId));
						commandList->DrawIndexed(commandPointer->ElemCount, commandPointer->IdxOffset + globalIndexOffset, commandPointer->VtxOffset + globalVertexOffset);
					}
				}

				globalIndexOffset += commandListImGui->IdxBuffer.Size;
				globalVertexOffset += commandListImGui->VtxBuffer.Size;
			}

			commandList->EndRenderPass();
		}
	}

	// Multi-Viewport / Platform Interface Support

	inline RHI_SwapChain* RetrieveSwapchain(ImGuiViewport* viewport)
	{
		AMETHYST_ASSERT(viewport != nullptr);

		return static_cast<RHI_SwapChain*>(viewport->RendererUserData); // RendererUserData - void* to hold custom data structure for the renderer (swapchain, framebuffer etc), generally set by your Renderer_CreateWindow function.
	}

	static void RHI_Window_Create(ImGuiViewport* viewport)
	{
		AMETHYST_ASSERT(viewport != nullptr);

		RHI_SwapChain* swapchain = new RHI_SwapChain
		(
			viewport->PlatformHandleRaw, // PlatformHandle is SDL_Window/GLFW, PlatformHandleRaw is HWD.
			g_RHI_Device,
			static_cast<uint32_t>(viewport->Size.x),
			static_cast<uint32_t>(viewport->Size.y),
			RHI_Format_R8G8B8A8_Unorm,
			g_Renderer->RetrieveSwapChain()->RetrieveBufferCount(),
			g_Renderer->RetrieveSwapChain()->RetrieveFlags(),
			(std::string("Swapchain Child ") + std::string(std::to_string(viewport->ID))).c_str()
		);

		AMETHYST_ASSERT(swapchain->IsInitialized());

		viewport->RendererUserData = swapchain;
	}

	static void RHI_Window_Destroy(ImGuiViewport* viewport)
	{
		AMETHYST_ASSERT(viewport != nullptr);

		if (RHI_SwapChain* swapchain = RetrieveSwapchain(viewport))
		{
			delete swapchain;
		}

		viewport->RendererUserData = nullptr;
	}

	static void RHI_Window_SetSize(ImGuiViewport* viewport, const ImVec2 size)
	{
		AMETHYST_ASSERT(viewport != nullptr);
		RHI_SwapChain* swapchain = RetrieveSwapchain(viewport);
		AMETHYST_ASSERT(swapchain != nullptr);

		if (!swapchain->Resize(static_cast<uint32_t>(size.x), static_cast<uint32_t>(size.y)))
		{
			AMETHYST_ERROR("Failed to resize swapchain.");
		}
	}

	static void RHI_Window_Render(ImGuiViewport* viewport, void*)
	{
		AMETHYST_ASSERT(viewport != nullptr);
		RHI_SwapChain* swapchain = RetrieveSwapchain(viewport);
		AMETHYST_ASSERT(swapchain != nullptr);

		RHI_CommandList* commandList = swapchain->RetrieveCommandList();
		AMETHYST_ASSERT(commandList != nullptr);

		if (!commandList->BeginCommandBuffer())
		{
			AMETHYST_ERROR("Failed to begin command list.");
			return;
		}

		const bool clear = !(viewport->Flags & ImGuiViewportFlags_NoRendererClear);
		Render(viewport->DrawData, swapchain, clear);

		// Retrieve Wait Semaphore
		RHI_Semaphore* waitSemaphore = commandList->RetrieveProcessedSemaphore();

		/*
			When moving a window outside of the main viewport for the first time, it skips presenting every other time, hence the semaphore will be signalled because
			it was never waited for by present. Thus, we do a dummy present here. Not sure why this behavior is occuring yet.
		*/
		if (waitSemaphore) // Semaphore is null for D3D11.
		{
			if (waitSemaphore->RetrieveState() == RHI_Semaphore_State::Signaled)
			{
				AMETHYST_INFO("Dummy presenting to reset semaphore.");
				swapchain->Present(waitSemaphore);
			}
		}

		if (!commandList->EndCommandBuffer())
		{
			AMETHYST_ERROR("Failed to end command list.");
			return;
		}

		if (!commandList->SubmitCommandBuffer())
		{
			AMETHYST_ERROR("Failed to submit command list.");
		}
	}

	static void RHI_Window_Present(ImGuiViewport* viewport, void*)
	{
		RHI_SwapChain* swapchain = RetrieveSwapchain(viewport);
		AMETHYST_ASSERT(swapchain != nullptr);
		AMETHYST_ASSERT(swapchain->RetrieveCommandList() != nullptr);

		if (!swapchain->Present(swapchain->RetrieveCommandList()->RetrieveProcessedSemaphore()))
		{
			AMETHYST_ERROR("Failed to present.");
		}
	}

	inline void InitializePlatformInterface()
	{
		ImGuiPlatformIO& platformIO = ImGui::GetPlatformIO();
		platformIO.Renderer_CreateWindow = RHI_Window_Create;   // Create Swapchain/Framebuffer etc. 
		platformIO.Renderer_DestroyWindow = RHI_Window_Destroy; // Destroy Swapchain/Framebuffer etc.
		platformIO.Renderer_SetWindowSize = RHI_Window_SetSize; // Resize Swapchain/Framebuffer etc.
		platformIO.Renderer_RenderWindow = RHI_Window_Render;   // Clear Framebuffer, setup render target, then render the viewport->DrawData.
		platformIO.Renderer_SwapBuffers = RHI_Window_Present;	// Call Present/SwapBuffers.
	}
}