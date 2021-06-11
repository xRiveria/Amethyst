#pragma once
#include <unordered_map>
#include <array>
#include <atomic>
#include "../Core/ISubsystem.h"
#include "../RHI/RHI_Viewport.h"
#include "../RHI/RHI_CommandList.h"
#include "../Runtime/Math/BoundingBox.h"
#include "../Runtime/Math/Rectangle.h"
#include "RendererEnums.h"

namespace Amethyst
{
	// Forward Declarations
	class Entity;
	class Camera;
	class Light;
	class ResourceCache;
	
	class Renderer : public ISubsystem
	{
	public:
		// Constants - We will put these up here so our subsequent functions below can use them. Not a fan...
		#define DEBUG_COLOR Math::Vector4(0.41f, 0.86f, 1.0f, 1.0f)


	public:
		Renderer(Context* context);
		~Renderer();

		// == Subsystem ==
		bool OnInitialize() override;
		void OnUpdate(float deltaTime) override;

		// Primitive Rendering
		void TickPrimitives(const float deltaTime);
		void DrawLine(const Math::Vector3& from, const Math::Vector3& to, const Math::Vector4& colorFrom = DEBUG_COLOR, const Math::Vector4& colorTo = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);
		void DrawTriangle(const Math::Vector3& vertex0, const Math::Vector3& vertex1, const Math::Vector3& vertex2, const Math::Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);
		void DrawRectangle(const Math::Rectangle& rectangle, const Math::Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);
		void DrawBox(const Math::BoundingBox& box, const Math::Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);
		void DrawCircle(const Math::Vector3& center, const Math::Vector3& axis, const float radius, const uint32_t segmentCount, const Math::Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);

		// Viewport
		const RHI_Viewport& RetrieveViewport() const { return m_Viewport; }
		void SetViewport(float width, float height);

		// Resolution Render
		const Math::Vector2& RetrieveResolutionRender() const { return m_ResolutionRender; }
		void SetResolutionRender(uint32_t width, uint32_t height);

		// Resolution Output
		const Math::Vector2& RetrieveResolutionOutput() const { return m_ResolutionOutput; }
		void SetResolutionOutput(const uint32_t width, const uint32_t height);

		// Transform Handle
		std::weak_ptr<Entity> SnapTransformHandleToEntity(const std::shared_ptr<Entity>& entity) const;
		bool IsTransformHandleEditing() const;

		// Debug/Visualize a Render Target
		const std::array<std::shared_ptr<RHI_Texture>, 25> RetrieveRenderTargets() { return m_RenderTargets; }
		void SetRenderTargetDebug(const RendererRenderTarget renderTargetDebug) { m_RenderTargetDebug = renderTargetDebug; }
		RendererRenderTarget RetrieveRenderTargetDebug() const { return m_RenderTargetDebug; }

		// Depth
		float RetrieveClearDepth() { RetrieveRendererOption(RendererOption::Render_ReverseZ) ? m_Viewport.m_DepthMinimum : m_Viewport.m_DepthMaximum; }
		RHI_Comparison_Function RetrieveDepthComaprisonFunction() const { return RetrieveRendererOption(RendererOption::Render_ReverseZ) ? RHI_Comparison_GreaterEqual : RHI_Comparison_LessEqual; }

		// Environment
		const std::shared_ptr<RHI_Texture>& RetrieveEnvironmentTexture();
		void SetEnvironmentTexture(const std::shared_ptr<RHI_Texture> texture);

		// Options
		uint64_t RetrieveRendererOptions() const { return m_RendererOptions; }
		void SetRendererOptions(const uint64_t options) { m_RendererOptions = options; }
		bool RetrieveRendererOption(const RendererOption option) const { return m_RendererOptions & option; }
		void SetRendererOption(RendererOption option, bool isEnabled);

		// Options Values
		template<typename T>
		T RetrieveRendererOptionValue(const RendererOptionValue option) { return static_cast<T>(m_OptionValues[option]); }
		void SetRendererOptionValue(RendererOptionValue option, float value);

		// Swapchain
		RHI_SwapChain* RetrieveSwapChain() const { return m_SwapChain.get(); }
		bool Present();
		bool FlushRenderer();

		// Default Textures
		RHI_Texture* RetrieveDefaultTextureWhite() const { return m_Texture_DefaultWhite.get(); }
		RHI_Texture* RetrieveDefaultTextureBlack() const { return m_Texture_DefaultBlack.get(); }
		RHI_Texture* RetrieveDefaultTextureTransparent() const { return m_Texture_DefaultTransparent.get(); }

		// Global Shader Resources
		void SetGlobalShaderObjectTransform(RHI_CommandList* commandList, const Math::Matrix& transform);
		void SetGlobalSamplersAndConstantBuffers(RHI_CommandList* commandList) const;

		// Rendering
		void StopRendering();
		void StartRendering();
		bool IsAllowedToRender() const { return m_IsAllowedToRender; }
		bool IsRendering() const { return m_IsRendering; }
		bool IsInitialized() const { return m_IsRendererInitialized; }

		// Misc
		const std::shared_ptr<RHI_Device>& RetrieveRHIDevice() const { return m_RHI_Device; }
		RHI_PipelineCache* RetrievePipelineCache() const { return m_PipelineCache.get(); }
		RHI_DescriptorSetLayoutCache* RetrieveDescriptorLayoutCache() const { return m_DescriptorSetLayoutCache.get(); }
		RHI_Texture* RetrieveFrameTexture() { return m_RenderTargets[static_cast<uint8_t>(RendererRenderTarget::Frame_PostProcess)].get(); }
		uint64_t RetrieveFrameNumber() const { return m_FrameNumber; }
		std::shared_ptr<Camera> RetrieveCamera() const { return m_Camera; }
		// Retrieve Shaders
		uint32_t RetrieveMaxResolution() const;
		void ClearEntities();

		// Passes
		void Pass_CopyToBackbuffer(RHI_CommandList* commandList);

	private:
		// Resource Creation
		void CreateConstantBuffers();
		void CreateDepthStencilStates();
		void CreateRasterizerStates();
		void CreateBlendStates();
		void CreateFonts();
		void CreateTextures();
		void CreateShaders();
		void CreateSamplers();
		void CreateRenderTextures(const bool createRender, const bool createOutput, const bool createFixed, const bool createDynamic);

		// Passes
		void Pass_Main(RHI_CommandList* commandList);
		void Pass_UpdateFrameBuffer(RHI_CommandList* commandList);

		// Constant Buffers
		bool UpdateFrameBuffer(RHI_CommandList* commandList);
		bool UpdateMaterialBuffer(RHI_CommandList* commandList);
		bool UpdateUberBuffer(RHI_CommandList* commandList);
		bool UpdateLightBuffer(RHI_CommandList** commandList, const Light* light);

		// Misc
		void RenderablesAcquire(const Variant& renderables);
		void RenderablesSort(std::vector<Entity*>* renderables);

	private:


		// Render Targets
		std::array<std::shared_ptr<RHI_Texture>, 25> m_RenderTargets;
		RendererRenderTarget m_RenderTargetDebug = RendererRenderTarget::Undefined;

		// Standard Textures
		std::shared_ptr<RHI_Texture> m_Texture_DefaultWhite;
		std::shared_ptr<RHI_Texture> m_Texture_DefaultBlack;
		std::shared_ptr<RHI_Texture> m_Texture_DefaultTransparent;

		// Gizmos

		//Resolution & Viewport
		RHI_Viewport m_Viewport = RHI_Viewport(0, 0, 0, 0);
		Math::Vector2 m_ResolutionOutput = Math::Vector2::Zero;
		Math::Vector2 m_ResolutionRender = Math::Vector2::Zero;

		// Options
		uint64_t m_RendererOptions = 0;
		std::unordered_map<RendererOptionValue, float> m_OptionValues;

		// Misc
		Math::Rectangle m_ViewportQuad;
		uint64_t m_FrameNumber = 0;
		bool m_UpdateOrthographicProjection = true;
		bool m_IsRendererInitialized = false;
		std::atomic<bool> m_IsAllowedToRender = true;
		std::atomic<bool> m_IsRendering = false;

		// RHI Core
		std::shared_ptr<RHI_Device> m_RHI_Device;
		std::shared_ptr<RHI_PipelineCache> m_PipelineCache;
		std::shared_ptr<RHI_DescriptorSetLayoutCache> m_DescriptorSetLayoutCache;

		// Swapchain
		std::shared_ptr<RHI_SwapChain> m_SwapChain;
		static const uint8_t m_SwapChainBufferCount = 3;

		// ==== Constant Buffers ====

		// Entities and Material References
		std::unordered_map<Renderer_Object_Type, std::vector<Entity*>> m_Entities; // Binds an entity to an object type.
		//std::array<Material*, m_MaxMaterialInstances> m_MaterialInstances;
		std::shared_ptr<Camera> m_Camera;

		// Dependencies
		ResourceCache* m_ResourceCache = nullptr;
	};
}