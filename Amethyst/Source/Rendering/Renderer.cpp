#include "Amethyst.h"
#include "Renderer.h"
#include "../Core/Window.h"
#include "../Runtime/ECS/World.h"
#include "../Resource/ResourceCache.h"
#include "../Runtime/ECS/Entity.h"
#include "../Runtime/ECS/Components/Transform.h"
#include "../Runtime/ECS/Components/Renderable.h"
#include "../Runtime/ECS/Components/Camera.h"
#include "../RHI/RHI_Device.h"
#include "../RHI/RHI_PipelineCache.h"
#include "../RHI/RHI_ConstantBuffer.h"
#include "../RHI/RHI_CommandList.h"
#include "../RHI/RHI_Texture2D.h"
#include "../RHI/RHI_SwapChain.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "../RHI/RHI_DescriptorSetLayoutCache.h"
#include "../RHI/RHI_Implementation.h"
#include "../RHI/RHI_Semaphore.h"
#include "../Display/Display.h"

namespace Amethyst
{
	Renderer::Renderer(Context* context) : ISubsystem(context)
	{
		// Options - Rendering
		m_RendererOptions |= RendererOption::Render_ReverseZ;

		// Options - Debugging
		m_RendererOptions |= RendererOption::RenderDebug_Transform;
		m_RendererOptions |= RendererOption::RenderDebug_Grid;
		///m_RendererOptions |= RendererOption::RenderDebug_Lights;
		///m_Options |= Renderer_Option::RenderDebug_Physics;

		/// Option Values - Rendering
		
		// Subscribe to events.
		SUBSCRIBE_TO_EVENT(EventType::WorldResolved, EVENT_HANDLER_VARIANT(RenderablesAcquire)); 
		SUBSCRIBE_TO_EVENT(EventType::WorldClear, EVENT_HANDLER(ClearEntities));				 
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

	void Renderer::OnUpdate(float deltaTime)
	{
		AMETHYST_ASSERT(m_RHI_Device != nullptr);
		AMETHYST_ASSERT(m_RHI_Device->IsInitialized());

		// Only do work if the swapchain is presenting and when we are allowed to render.
		if (m_SwapChain && m_SwapChain->IsPresentationEnabled() && m_IsAllowedToRender)
		{
			// Acquire command list.
			RHI_CommandList* commandList = m_SwapChain->RetrieveCommandList();

			// Begin
			commandList->BeginCommandBuffer();

			// Only render when the world is not loading as the command list will get flushed by the loading thread.
			if (!m_EngineContext->RetrieveSubsystem<World>()->IsWorldLoading())
			{
				m_IsRendering = true;

				// If there is no camera, clear to black.
				if (!m_Camera)
				{
					commandList->ClearRenderTarget(RENDER_TARGET(Renderer_RenderTargetType::Frame_PostProcess).get(), 0, 0, false, Math::Vector4(0.0f, 0.0f, 0.0f, 1.0f));
					return;
				}

				// If there is a camera but no entities to render, clear to camera's color.
				if (m_Entities[Renderer_ObjectType::Renderer_Object_Opaque].empty() && m_Entities[Renderer_ObjectType::Renderer_Object_Transparent].empty())
				{
					commandList->ClearRenderTarget(RENDER_TARGET(Renderer_RenderTargetType::Frame_PostProcess).get(), 0, 0, false, m_Camera->RetrieveClearColor());
					return;
				}

				// Reset dynamic buffer indices when the swapchain reverts to first buffer/command list.
				if (m_SwapChain->RetrieveCommandListIndex() == 0)
				{
					m_BufferUber_DynamicOffsetIndex = 0;
					m_BufferFrame_DynamicOffsetIndex = 0;
					m_BufferLight_DynamicOffsetIndex = 0;
					m_BufferMaterial_DynamicOffsetIndex = 0;
				}

				// Update Framebuffer
				{
					if (m_UpdateOrthographicProjection || m_NearPlane != m_Camera->RetrieveNearPlane() || m_FarPlane != m_Camera->RetrieveFarPlane()) // There were changes to our camera because of user input. Thus, recompute for our constant buffers.
					{
						m_BufferFrame_CPU.m_ProjectionOrthographic = Math::Matrix::CreateOrthographic(m_Viewport.m_Width, m_Viewport.m_Height, m_NearPlane, m_FarPlane);
						m_BufferFrame_CPU.m_ViewProjectionOrthographic = Math::Matrix::CreateLookAtMatrix(Math::Vector3(0, 0, -m_NearPlane), Math::Vector3::Forward, Math::Vector3::Up) * m_BufferFrame_CPU.m_ProjectionOrthographic;
						m_UpdateOrthographicProjection = false;
					}

					m_NearPlane = m_Camera->RetrieveNearPlane();
					m_FarPlane = m_Camera->RetrieveFarPlane();
					m_BufferFrame_CPU.m_View = m_Camera->RetrieveViewMatrix();
					m_BufferFrame_CPU.m_Projection = m_Camera->RetrieveProjectionMatrix();

					/// TAA Shenanigans

					// Update the remaining of the frame buffer.
					m_BufferFrame_CPU.m_ViewProjectionPrevious = m_BufferFrame_CPU.m_ViewProjection;
					m_BufferFrame_CPU.m_ViewProjection = m_BufferFrame_CPU.m_View * m_BufferFrame_CPU.m_Projection;
					m_BufferFrame_CPU.m_ViewProjectionInversed = Math::Matrix::Invert(m_BufferFrame_CPU.m_ViewProjection);
					m_BufferFrame_CPU.m_ViewProjectionUnjittered = m_BufferFrame_CPU.m_View * m_Camera->RetrieveProjectionMatrix();
					/// Aperture
					/// Shutter Speed
					/// ISO
					m_BufferFrame_CPU.m_CameraNear = m_Camera->RetrieveNearPlane();
					m_BufferFrame_CPU.m_CameraFar = m_Camera->RetrieveFarPlane();
					m_BufferFrame_CPU.m_CameraPosition = m_Camera->RetrieveTransform()->RetrievePosition();
					m_BufferFrame_CPU.m_CameraDirection = m_Camera->RetrieveTransform()->RetrieveForwardVector();
					m_BufferFrame_CPU.m_ResolutionOutput = m_ResolutionOutput;
					m_BufferFrame_CPU.m_ResolutionRender = m_ResolutionRender;
					/// Jitter Offset
					m_BufferFrame_CPU.m_DeltaTime = static_cast<float>(m_EngineContext->RetrieveSubsystem<Timer>()->RetrieveDeltaTimeSmoothedInSeconds());
					m_BufferFrame_CPU.m_Time = static_cast<float>(m_EngineContext->RetrieveSubsystem<Timer>()->RetrieveTimeInSeconds());
					/// Bloom Intensity
					/// Sharpen Strength
					/// Fog
					/// Tonemapping
					/// Gamma
					/// Shadow Resolution
					m_BufferFrame_CPU.m_FrameNumber = static_cast<uint32_t>(m_FrameNumber);

					// These must match what CommonBuffer.hlsl is reading.
					/// Screen Space Reflections
					/// TAA
				}

				Pass_Main(commandList);

				// TickPrimitives(deltaTime);

				m_FrameNumber++;
				m_IsOddFrameNumber = (m_FrameNumber % 2) == 1;
				m_IsRendering = false;
			}
		}

		// Resize Swapchain to Window Size if Needed.
		{
			// Passing zero dimensions will cause the swapchain to not present at all.
			Window* window = m_EngineContext->RetrieveSubsystem<Window>();
			uint32_t width = static_cast<uint32_t>(window->IsMinimized() ? 0 : window->RetrieveWidth());
			uint32_t height = static_cast<uint32_t>(window->IsMinimized() ? 0 : window->RetrieveHeight());

			if (!m_SwapChain->IsPresentationEnabled() || m_SwapChain->RetrieveWidth() != width || m_SwapChain->RetrieveHeight() != height)
			{
				m_SwapChain->Resize(static_cast<uint32_t>(width), static_cast<uint32_t>(height));

				// Log
				AMETHYST_INFO("Swapchain resolution has been set to %dx%d.", width, height);
			}
		}
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
		DisplayMode displayMode = Display::RetrieveActiveDisplayMode();
		displayMode.m_Width = width;
		displayMode.m_Height = height;
		Display::SetActiveDisplayMode(displayMode);

		// Register display mode in case it doesn't exist.
		Display::RegisterDisplayMode(displayMode, m_EngineContext);

		// Re-create render textures.
		CreateRenderTextures(true, false, false, true);

		// Log
		AMETHYST_INFO("Render resolution has been set to %dx%d", width, height);
	}

	void Renderer::SetResolutionOutput(uint32_t width, uint32_t height)
	{
		// Return if resolution is invalid.
		if (!m_RHI_Device->IsValidResolution(width, height))
		{
			AMETHYST_WARNING("%dx%d is an invalid resolution.", width, height);
			return;
		}

		// Make sure we are pixel perfect.
		width  -= (width % 2 != 0) ? 1 : 0;
		height -= (height % 2 != 0) ? 1 : 0;

		// Silently return if resolution is already set.
		if (m_ResolutionOutput.m_X == width && m_ResolutionOutput.m_Y == height)
		{
			return;
		}

		// Set resolution.
		m_ResolutionOutput.m_X = static_cast<float>(width);
		m_ResolutionOutput.m_Y = static_cast<float>(height);

		// Re-create render textures.
		CreateRenderTextures(false, true, false, true);

		// Log
		AMETHYST_INFO("Output resolution has been set to %dx%d.", width, height);
	}

	template<typename T>
	bool UpdateDynamicBuffer(RHI_CommandList* commandList, RHI_ConstantBuffer* bufferGPU, T& bufferCPU, T& bufferCPUPrevious, uint32_t& offsetIndex)
	{
		// Only update if needed.
		if (bufferCPU == bufferCPUPrevious)
		{
			return true;
		}

		offsetIndex++;

		// Reallocate buffer with double size if needed.
		if (bufferGPU->IsDynamic())
		{
			if (offsetIndex >= bufferGPU->RetrieveOffsetCount())
			{
				commandList->FlushCommandBuffer();
				const uint32_t newSize = Math::Utilities::NextPowerOfTwo(offsetIndex + 1);
				if (!bufferGPU->Create<T>(newSize))
				{
					AMETHYST_ERROR("Failed to re-allocate %s buffer with %d offsets.", bufferGPU->RetrieveObjectName().c_str(), newSize);
					return false;
				}

				AMETHYST_INFO("Increased %s buffer offsets to %d. That's %dkb.", bufferGPU->RetrieveObjectName().c_str(), newSize, (newSize * bufferGPU->RetrieveStride()) / 1000);
			}
		}

		// Set new buffer offset.
		if (bufferGPU->IsDynamic())
		{
			bufferGPU->SetOffsetIndexDynamic(offsetIndex);
		}

		// Map
		T* buffer = static_cast<T*>(bufferGPU->Map());
		if (!buffer)
		{
			AMETHYST_ERROR("Failed to map buffer.");
			return false;
		}

		const uint64_t size = bufferGPU->RetrieveStride();
		const uint64_t offset = offsetIndex * size;

		// Update
		if (bufferGPU->IsDynamic())
		{
			memcpy(reinterpret_cast<std::byte*>(buffer) + offset, reinterpret_cast<std::byte*>(&bufferCPU), size);
		}
		else
		{
			*buffer = bufferCPU;
		}

		bufferCPUPrevious = bufferCPU;

		// Unmap
		return bufferGPU->Unmap(offset, size);
	}

	bool Renderer::UpdateFrameBuffer(RHI_CommandList* commandList)
	{
		/// Directional Light Intensity Stuff
		if (!commandList)
		{
			AMETHYST_ERROR("Invalid command list.");
			return false;
		}

		if (!UpdateDynamicBuffer<BufferFrame>(commandList, m_BufferFrame_GPU.get(), m_BufferFrame_CPU, m_BufferFrame_CPUPrevious, m_BufferFrame_DynamicOffsetIndex))
		{
			return false;
		}

		// Dynamic buffers with offsets have to be rebound whenever the offset changes.
		return commandList->SetConstantBuffer(0, RHI_Shader_Vertex | RHI_Shader_Pixel | RHI_Shader_Compute, m_BufferFrame_GPU);
	}

	bool Renderer::UpdateUberBuffer(RHI_CommandList* commandList)
	{
		if (!commandList)
		{
			AMETHYST_ERROR("Invalid command list.");
			return false;
		}

		if (!UpdateDynamicBuffer<BufferUber>(commandList, m_BufferUber_GPU.get(), m_BufferUber_CPU, m_BufferUber_CPUPrevious, m_BufferUber_DynamicOffsetIndex))
		{
			return false;
		}

		// Dynamic buffers with offsets have to be rebound whenever the offset changes.
		return commandList->SetConstantBuffer(2, RHI_Shader_Vertex | RHI_Shader_Pixel | RHI_Shader_Compute, m_BufferUber_GPU);
	}

	void Renderer::RenderablesAcquire(const Variant& renderables)
	{
		// Clear previous state.
		m_Entities.clear();
		m_Camera = nullptr;

		std::vector<std::shared_ptr<Entity>> entities = renderables.RetrieveValue<std::vector<std::shared_ptr<Entity>>>();
		for (const std::shared_ptr<Entity>& entity : entities)
		{
			if (!entity || !entity->IsActive())
			{
				continue;
			}

			// Retrieve all the components we are interested in.
			Renderable* renderable = entity->GetComponent<Renderable>();
			/// Light
			Camera* camera = entity->GetComponent<Camera>();

			if (renderable)
			{
				bool isTransparent = false;

				/// Material. 

				/// For now, no logic of transparency.
				m_Entities[isTransparent ? Renderer_ObjectType::Renderer_Object_Transparent : Renderer_ObjectType::Renderer_Object_Opaque].emplace_back(entity.get());
			}

			/// Light Logic


			if (camera)
			{
				m_Entities[Renderer_ObjectType::Renderer_Object_Camera].emplace_back(entity.get());
				m_Camera = camera->RetrieveSharedPointer<Camera>();
			}
		}

		RenderablesSort(&m_Entities[Renderer_ObjectType::Renderer_Object_Opaque]);
		/// Sort transparent objects as well.
	}

	void Renderer::RenderablesSort(std::vector<Entity*>* renderables)
	{
		if (!m_Camera || renderables->size() <= 2)
		{
			return;
		}

		// Retrieve magnitude from camera to the object.
		auto comparisonOperation = [this](Entity* entity)
		{
			Renderable* renderable = entity->RetrieveRenderable();
			if (!renderable)
			{
				return 0.0f;
			}

			return (renderable->RetrieveAABB().RetrieveCenter() - m_Camera->RetrieveTransform()->RetrievePosition()).LengthSquared();
		};

		// Sort by depth (front to back).
		std::sort(renderables->begin(), renderables->end(), [&comparisonOperation](Entity* a, Entity* b)
		{
			return comparisonOperation(a) < comparisonOperation(b);
		});
	}

	void Renderer::ClearEntities()
	{
		// Flush to remove references to entity resources that will be deallocated.
		FlushRenderer();
		m_Entities.clear();
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

		// Wait in case this call is coming from a different thread as attempting to end a render pass while its being used causes an exception.
		m_RHI_Device->Queue_WaitAll();

		if (!m_SwapChain->RetrieveCommandList()->FlushCommandBuffer())
		{
			AMETHYST_ERROR("Failed to flush renderer.");
			return false;
		}

		return true;
	}

	void Renderer::SetGlobalShaderObjectTransform(RHI_CommandList* commandList, const Math::Matrix& transform)
	{
		m_BufferUber_CPU.m_Transform = transform;
		UpdateUberBuffer(commandList);
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