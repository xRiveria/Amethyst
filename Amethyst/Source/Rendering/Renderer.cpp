#include "Amethyst.h"
#include "Renderer.h"
#include "../Core/Window.h"
#include "../Resource/ResourceCache.h"
#include "../Runtime/ECS/Entity.h"
#include "../RHI/RHI_Device.h"
#include "../RHI/RHI_SwapChain.h"
#include "../RHI/RHI_Implementation.h"
#include "../RHI/RHI_Semaphore.h"
#include "../RHI/RHI_DescriptorSetLayoutCache.h"
#include "../RHI/RHI_PipelineCache.h"

namespace Amethyst
{
	Renderer::Renderer(Context* context) : ISubsystem(context)
	{
		// Options - Rendering
		m_RendererOptions |= RendererOption::Render_ReverseZ;

		// Options - Debugging
		m_RendererOptions |= RendererOption::RenderDebug_Transform;
		m_RendererOptions |= RendererOption::RenderDebug_Grid;
		m_RendererOptions |= RendererOption::RenderDebug_Lights;
		///m_Options |= Renderer_Option::RenderDebug_Physics;

		/// Option Values - Rendering
		
		// Subscribe to events.
		SUBSCRIBE_TO_EVENT(EventType::WorldResolved, EVENT_HANDLER_VARIANT(RenderablesAcquire)); ///
		SUBSCRIBE_TO_EVENT(EventType::WorldClear, EVENT_HANDLER(ClearEntities));				 ///
	}

	Renderer::~Renderer()
	{
		// Unsubscribe from events.
		UNSUBSCRIBE_FROM_EVENT(EventType::WorldResolved, EVENT_HANDLER_VARIANT(RenderablesAcquire));

		m_Entities.clear();
		m_Camera = nullptr;

		// Log to file as the renderer is no more.
		LOG_TO_FILE(true);
	}

	bool Renderer::OnInitialize()
	{	   
		// Retrieve required systems.
		m_ResourceCache = m_EngineContext->RetrieveSubsystem<ResourceCache>();
		/// Retrieve Profiler.

		// Create Device
		m_RHI_Device = std::make_shared<RHI_Device>(m_EngineContext);
		if (!m_RHI_Device->IsInitialized())
		{
			AMETHYST_ERROR("Failed to create rendering device.");
			return false;
		}

		// Create pipeline cache.
		m_PipelineCache = std::make_shared<RHI_PipelineCache>(m_RHI_Device.get());

		// Create descriptor set layout cache.
		m_DescriptorSetLayoutCache = std::make_shared<RHI_DescriptorSetLayoutCache>(m_RHI_Device.get());

		// Retrieve Window
		Window* window = m_EngineContext->RetrieveSubsystem<Window>();
		uint32_t windowWidth = window->RetrieveWidth();
		uint32_t windowHeight = window->RetrieveHeight();

		// Create Swapchain
		{
			m_SwapChain = std::make_shared<RHI_SwapChain>
			(
				window->RetrieveHWND(),
				m_RHI_Device,
				windowWidth,
				windowHeight,
				RHI_Format_R8G8B8A8_Unorm,
				m_SwapChainBufferCount,
				RHI_Present_Immediate | RHI_Swap_Flip_Discard,
				"Main Swapchain"
			);

			if (!m_SwapChain->IsInitialized())
			{
				AMETHYST_ERROR("Failed to create swapchain.");
				return false;
			}
		}

		// Full-screen Quad
		m_ViewportQuad = Math::Rectangle(0, 0, static_cast<float>(windowWidth), static_cast<float>(windowHeight));
		m_ViewportQuad.CreateBuffers(this);

		/// Create Line Buffer

		/// Editor Specific Transform Gizmos
		   
		// Set Render, Output and Viewport Resolution/Size to whatever the window is (initially).
		SetResolutionRender(windowWidth, windowHeight);
		SetResolutionOutput(static_cast<uint32_t>(m_ResolutionRender.m_X), static_cast<uint32_t>(m_ResolutionRender.m_Y));
		SetViewport(m_ResolutionRender.m_X, m_ResolutionRender.m_Y);
		   
		// Create Our Renderer Assets
		CreateConstantBuffers();
		CreateShaders();
		CreateDepthStencilStates();
		CreateRasterizerStates();
		CreateBlendStates();
		CreateRenderTextures(false, false, true, true);
		CreateFonts();
		CreateSamplers();
		CreateTextures();

		if (!m_IsRendererInitialized)
		{
			// Log on-screen as the renderer is ready.
			LOG_TO_FILE(false);
			m_IsRendererInitialized = true;
		}

		return true;
	}

	std::weak_ptr<Amethyst::Entity> Renderer::SnapTransformHandleToEntity(const std::shared_ptr<Entity>& entity) const
	{
		///
	}

	bool Renderer::IsTransformHandleEditing() const
	{
		///
	}

	void Renderer::OnUpdate(float deltaTime)
	{

	}

	void Renderer::SetViewport(float width, float height)
	{
		if (m_Viewport.m_Width != width || m_Viewport.m_Height != height)
		{
			FlushRenderer(); // Wait for all queue operations to complete and flush all command lists.

			/// Specular Lut Rendered?

			// Update Viewport
			m_Viewport.m_Width = width;
			m_Viewport.m_Height = height;

			// Update Full-Screen Quad
			m_ViewportQuad = Math::Rectangle(0, 0, width, height);
			m_ViewportQuad.CreateBuffers(this);

			m_UpdateOrthographicProjection = true;
		}
	}

	void Renderer::SetResolutionRender(uint32_t width, uint32_t height)
	{
		// Return if resolution is invalid.
		if (!RHI_Device::IsValidResolution(width, height))
		{
			AMETHYST_WARNING("%dx%d is an invalid resolution.", width, height);
			return;
		}

		// Make sure we are pixel perfect.
		width  -= (width  % 2 != 0) ? 1 : 0;
		height -= (height % 2 != 0) ? 1 : 0;

		// Silently return if resolution is already set.
		if (m_ResolutionRender.m_X == width && m_ResolutionRender.m_Y == height)
		{
			return;
		}

		// Set resolution.
		m_ResolutionRender.m_X = static_cast<float>(width);
		m_ResolutionRender.m_Y = static_cast<float>(height);

		// Set as active display mode.
		

		// Register Display Mode (if it doesn't exist).

		// Re-create render textures.
		CreateRenderTextures(true, false, false, true);

		// Log
		AMETHYST_INFO("Render resolution has been set to %dx%d", width, height);
	}

	void Renderer::SetResolutionOutput(uint32_t width, uint32_t height)
	{
		
	}

	bool Renderer::UpdateFrameBuffer(RHI_CommandList* commandList)
	{
		return false;
	}

	bool Renderer::UpdateMaterialBuffer(RHI_CommandList* commandList)
	{
		return false;
	}

	bool Renderer::UpdateUberBuffer(RHI_CommandList* commandList)
	{
		return false;
	}

	bool Renderer::UpdateLightBuffer(RHI_CommandList** commandList, const Light* light)
	{
		return false;
	}

	void Renderer::RenderablesAcquire(const Variant& renderables)
	{
		// Clear previous state.
		m_Entities.clear();
		m_Camera = nullptr;


	}

	void Renderer::RenderablesSort(std::vector<Entity*>* renderables)
	{

	}

	void Renderer::ClearEntities()
	{
		// Flush to remove references to entity resources that will be deallocated.
		FlushRenderer();
		m_Entities.clear();
	}

	const std::shared_ptr<Amethyst::RHI_Texture>& Renderer::RetrieveEnvironmentTexture()
	{
		///
	}

	void Renderer::SetEnvironmentTexture(const std::shared_ptr<RHI_Texture> texture)
	{
		///
	}

	void Renderer::SetRendererOption(RendererOption option, bool isEnabled)
	{
		if (isEnabled && !RetrieveRendererOption(option))
		{
			m_RendererOptions |= option;
		}
		else if (!isEnabled && RetrieveRendererOption(option))
		{
			m_RendererOptions &= ~option;
		}
	}

	void Renderer::SetRendererOptionValue(RendererOptionValue option, float value)
	{
		///
	}

	bool Renderer::Present()
	{
		// Retrieve Command List
		RHI_CommandList* commandList = m_SwapChain->RetrieveCommandList();

		// Retrieve Wait Semaphore
		RHI_Semaphore* waitSemaphore = commandList->RetrieveProcessedSemaphore();

		// When moving an ImGui window outside of the main viewport for the first time, it skips presenting every other time.
		// Hence, the semaphore will signal as it was never waited for by present. Thus, we do a dummy present here.

		if (waitSemaphore) 	// Semaphore is null for D3D11.
		{
			if (waitSemaphore->RetrieveState() == RHI_Semaphore_State::Signaled) 
			{
				AMETHYST_INFO("Dummy presenting to reset semaphore.");
				m_SwapChain->Present(waitSemaphore);
			}
		}

		// Finalize command list.
		if (commandList->RetrieveCommandListState() == RHI_CommandListState::Recording)
		{
			commandList->EndCommandBuffer();
			commandList->SubmitCommandBuffer();
		}

		if (!m_SwapChain->IsPresentationEnabled())
		{
			return false;
		}

		// Semaphore is null for D3D11.
		if (waitSemaphore)
		{
			// Validate semaphore state.
			AMETHYST_ASSERT(waitSemaphore->RetrieveState() == RHI_Semaphore_State::Signaled);
		}

		return m_SwapChain->Present(waitSemaphore);
	}

	bool Renderer::FlushRenderer()
	{
		AMETHYST_ASSERT(m_SwapChain != nullptr);
		AMETHYST_ASSERT(m_SwapChain->RetrieveCommandList() != nullptr);

		// Wait in case this call is coming from a different threead as attempting to end a render pass while its being used causes an exception.
		m_RHI_Device->Queue_WaitAll();

		if (!m_SwapChain->RetrieveCommandList()->FlushCommandBuffer())
		{
			AMETHYST_ERROR("Failed to flush renderer.");
			return false;
		}

		return true;
	}

	void Renderer::StopRendering()
	{
		// Notify rendering to stop.
		m_IsAllowedToRender = false;

		// Wait for rendering to stop.
		while (m_IsRendering)
		{
			AMETHYST_INFO("Waiting for rendering to complete...");
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}
	}

	void Renderer::StartRendering()
	{
		m_IsAllowedToRender = true;
	}

	uint32_t Renderer::RetrieveMaxResolution() const
	{
		return RHI_Context::m_Texture2D_DimensionsMax;
	}
}