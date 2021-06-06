#pragma once
#include <unordered_map>
#include <array>
#include <atomic>
#include "../Core/ISubsystem.h"
#include "../RHI/RHI_Viewport.h"
#include "../RHI/RHI_CommandList.h"
#include "RendererEnums.h"

namespace Amethyst
{
	// Forward Declarations
	class Entity;
	class Camera;
	
	class Renderer : public ISubsystem
	{
	public:
		Renderer(Context* context);
		~Renderer();

		bool InitializeSubsystem() override;
		void OnUpdate(float deltaTime) override;

		// Options Values
		template<typename T>
		T RetrieveOptionValue(const RendererOptionValue option) { return static_cast<T>(m_OptionValues[option]); }

		// Viewport
		const RHI_Viewport& RetrieveViewport() const { return m_Viewport; }
		void SetViewport(float width, float height);

		// Default Textures
		RHI_Texture* RetrieveDefaultTextureWhite() const { return m_Texture_DefaultWhite.get(); }
		RHI_Texture* RetrieveDefaultTextureBlack() const { return m_Texture_DefaultBlack.get(); }
		RHI_Texture* RetrieveDefaultTextureTransparent() const { return m_Texture_DefaultTransparent.get(); }

		// Global Shader Resources
		void SetGlobalSamplersAndConstantBuffers(RHI_CommandList* commandList) const;

		// Swapchain
		RHI_SwapChain* RetrieveSwapChain() const { return m_SwapChain.get(); }

		// Primitive Rendering

		// Misc
		RHI_PipelineCache* RetrievePipelineCache() const { return m_PipelineCache.get(); }
		RHI_DescriptorSetLayoutCache* RetrieveDescriptorLayoutCache() const { return m_DescriptorSetLayoutCache.get(); }
		void Clear();
		bool IsInitialized() const { return m_IsInitialized; }

		const std::shared_ptr<RHI_Device>& RetrieveRHIDevice() const { return m_RHI_Device; }

	private:
		// Misc
		void RenderablesAcquire(const Variant& renderables);

	private:
		//RHI Core
		std::shared_ptr<RHI_Device> m_RHI_Device;
		std::shared_ptr<RHI_PipelineCache> m_PipelineCache;
		std::shared_ptr<RHI_DescriptorSetLayoutCache> m_DescriptorSetLayoutCache;

		// Options
		uint64_t m_Options = 0;
		std::unordered_map<RendererOptionValue, float> m_OptionValues;

		// Standard Textures
		std::shared_ptr<RHI_Texture> m_Texture_DefaultWhite;
		std::shared_ptr<RHI_Texture> m_Texture_DefaultBlack;
		std::shared_ptr<RHI_Texture> m_Texture_DefaultTransparent;

		// Swapchain
		std::shared_ptr<RHI_SwapChain> m_SwapChain;

		//Resolution & Viewport
		RHI_Viewport m_Viewport = RHI_Viewport(0, 0, 0, 0);
		 
		//std::shared_ptr<RHI_PipelineCache> m_RHI_PipelineCache;

		bool m_IsInitialized = false;

		// Entities and Material References
		std::unordered_map<Renderer_Object_Type, std::vector<Entity*>> m_Entities; // Binds an entity to an object type.
		std::shared_ptr<Camera> m_Camera;
	};
}