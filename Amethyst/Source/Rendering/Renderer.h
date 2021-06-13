#pragma once
#include <unordered_map>
#include <array>
#include <atomic>
#include "Renderer_ConstantBuffers.h"
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
	///class Light;
	class ResourceCache;
	
	class Renderer : public ISubsystem
	{
	public:
		// Constants - We will put these up here so our subsequent functions below can use them. Not a fan...
		#define DEBUG_COLOR Math::Vector4(0.41f, 0.86f, 1.0f, 1.0f)
		#define RENDER_TARGET(renderTargetEnum) m_RenderTargets[static_cast<uint8_t>(renderTargetEnum)]

	public:
		Renderer(Context* context);
		~Renderer();

		// == Subsystem ==
		bool OnInitialize() override;
		void OnUpdate(float deltaTime) override;

		// Primitive Rendering for Debugging
		///void TickPrimitives(const float deltaTime);
		///void DrawLine(const Math::Vector3& from, const Math::Vector3& to, const Math::Vector4& colorFrom = DEBUG_COLOR, const Math::Vector4& colorTo = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);
		///void DrawTriangle(const Math::Vector3& vertex0, const Math::Vector3& vertex1, const Math::Vector3& vertex2, const Math::Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);
		///void DrawRectangle(const Math::Rectangle& rectangle, const Math::Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);
		///void DrawBox(const Math::BoundingBox& box, const Math::Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);
		///void DrawCircle(const Math::Vector3& center, const Math::Vector3& axis, const float radius, const uint32_t segmentCount, const Math::Vector4& color = DEBUG_COLOR, const float duration = 0.0f, const bool depthEnabled = true);

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
		/// std::weak_ptr<Entity> SnapTransformHandleToEntity(const std::shared_ptr<Entity>& entity) const;
		/// bool IsTransformHandleEditing() const;

		// Debug/Visualize a Render Target
		const std::array<std::shared_ptr<RHI_Texture>, 25> RetrieveRenderTargets() { return m_RenderTargets; }
		void SetRenderTargetDebug(const Renderer_RenderTargetType renderTargetDebug) { m_RenderTargetDebug = renderTargetDebug; }
		Renderer_RenderTargetType RetrieveRenderTargetDebug() const { return m_RenderTargetDebug; }

		// Depth
		float RetrieveClearDepth() { RetrieveRendererOption(RendererOption::Render_ReverseZ) ? m_Viewport.m_DepthMinimum : m_Viewport.m_DepthMaximum; }
		RHI_Comparison_Function RetrieveDepthComparisonFunction() const { return RetrieveRendererOption(RendererOption::Render_ReverseZ) ? RHI_Comparison_GreaterEqual : RHI_Comparison_LessEqual; }

		// Environment
		/// const std::shared_ptr<RHI_Texture>& RetrieveEnvironmentTexture();
		/// void SetEnvironmentTexture(const std::shared_ptr<RHI_Texture> texture);

		// Options
		uint64_t RetrieveRendererOptions() const { return m_RendererOptions; }
		void SetRendererOptions(const uint64_t options) { m_RendererOptions = options; }
		bool RetrieveRendererOption(const RendererOption option) const { return m_RendererOptions & option; }
		void SetRendererOption(RendererOption option, bool isEnabled);

		// Options Values
		template<typename T>
		T RetrieveRendererOptionValue(const RendererOptionValue option) { return static_cast<T>(m_OptionValues[option]); }
		/// void SetRendererOptionValue(RendererOptionValue option, float value);

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
		RHI_Texture* RetrieveFrameTexture() { return m_RenderTargets[static_cast<uint8_t>(Renderer_RenderTargetType::Frame_PostProcess)].get(); }
		uint64_t RetrieveFrameNumber() const { return m_FrameNumber; }
		std::shared_ptr<Camera> RetrieveCamera() const { return m_Camera; }
		// Retrieve Shaders
		uint32_t RetrieveMaxResolution() const;
		void ClearEntities();

		void Pass_CopyToBackbuffer(RHI_CommandList* commandList);

	private:
		// Resource Creation
		void CreateConstantBuffers();
		void CreateDepthStencilStates();
		void CreateRasterizerStates();
		void CreateBlendStates();
		void CreateFonts();
		void CreateDefaultTextures();
		void CreateShaders();
		void CreateSamplers();
		void CreateRenderTextures(const bool createRenderResolutions, const bool createOutputResolutions, const bool createFixedResolutions, const bool createDynamicResolutions);

		// Passes
		void Pass_Main(RHI_CommandList* commandList);
		void Pass_UpdateFrameBuffer(RHI_CommandList* commandList);
		void Pass_GBuffer(RHI_CommandList* commandList, const bool isTransparentPass = false);
		void Pass_PostProcess(RHI_CommandList* commandList);
		void Pass_Lines(RHI_CommandList* commandList, RHI_Texture* outputTexture);

		// Constant Buffers
		bool UpdateFrameBuffer(RHI_CommandList* commandList);
		///bool UpdateMaterialBuffer(RHI_CommandList* commandList);
		bool UpdateUberBuffer(RHI_CommandList* commandList);
		///bool UpdateLightBuffer(RHI_CommandList** commandList, const Light* light);

		// Misc
		void RenderablesAcquire(const Variant& renderables);
		void RenderablesSort(std::vector<Entity*>* renderables);

	private:


		// Render Targets
		std::array<std::shared_ptr<RHI_Texture>, 25> m_RenderTargets;
		Renderer_RenderTargetType m_RenderTargetDebug = Renderer_RenderTargetType::Undefined;

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
		bool m_IsOddFrameNumber = false;

		float m_NearPlane = 0.0f;
		float m_FarPlane = 0.0;
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

		// Depth-Stencil States - [(Depth) Read/Write] & [(Stencil) Read/Write]
		std::shared_ptr<RHI_DepthStencilState> m_DepthStencil_Off_Off;
		std::shared_ptr<RHI_DepthStencilState> m_DepthStencil_Off_Read;
		std::shared_ptr<RHI_DepthStencilState> m_DepthStencil_ReadWrite_Off;
		std::shared_ptr<RHI_DepthStencilState> m_DepthStencil_Read_Off;
		std::shared_ptr<RHI_DepthStencilState> m_DepthStencil_ReadWrite_Write;

		// Blend States
		std::shared_ptr<RHI_BlendState> m_Blend_Disabled;
		std::shared_ptr<RHI_BlendState> m_Blend_Alpha;
		std::shared_ptr<RHI_BlendState> m_Blend_Additive;

		// Rasterizer States
		std::shared_ptr<RHI_RasterizerState> m_Rasterizer_CullBackSolid;
		std::shared_ptr<RHI_RasterizerState> m_Rasterizer_CullBackWireframe;
		/// std::shared_ptr<RHI_RasterizerState> m_Rasterizer_LightPointSpot;
		/// std::shared_ptr<RHI_RasterizerState> m_Rasterizer_LightDirectional;

		// ==== Samplers ====
		std::shared_ptr<RHI_Sampler> m_Sampler_CompareDepth;
		std::shared_ptr<RHI_Sampler> m_Sampler_PointClamp;
		std::shared_ptr<RHI_Sampler> m_Sampler_PointWrap;
		std::shared_ptr<RHI_Sampler> m_Sampler_BilinearClamp;
		std::shared_ptr<RHI_Sampler> m_Sampler_BilinearWrap;
		std::shared_ptr<RHI_Sampler> m_Sampler_TrilinearClamp;
		std::shared_ptr<RHI_Sampler> m_Sampler_AnisotropicWrap;

		// ==== Constant Buffers ====
		BufferFrame m_BufferFrame_CPU;
		BufferFrame m_BufferFrame_CPUPrevious;
		std::shared_ptr<RHI_ConstantBuffer> m_BufferFrame_GPU;
		uint32_t m_BufferFrame_DynamicOffsetIndex = 0;

		BufferUber m_BufferUber_CPU;
		BufferUber m_BufferUber_CPUPrevious;
		std::shared_ptr<RHI_ConstantBuffer> m_BufferUber_GPU;
		uint32_t m_BufferUber_DynamicOffsetIndex = 0;

		uint32_t m_BufferMaterial_DynamicOffsetIndex = 0;
		uint32_t m_BufferLight_DynamicOffsetIndex = 0;

		// Entities and Material References
		std::unordered_map<Renderer_ObjectType, std::vector<Entity*>> m_Entities; // Binds an entity to an object type.
		//std::array<Material*, m_MaxMaterialInstances> m_MaterialInstances;
		std::shared_ptr<Camera> m_Camera;

		// Dependencies
		ResourceCache* m_ResourceCache = nullptr;
	};
}