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

		//Viewport
		const RHI_Viewport& RetrieveViewport() const { return m_Viewport; }
		void SetViewport(float width, float height);

		// Global Shader Resources
		void SetGlobalSamplersAndConstantBuffers(RHI_CommandList* commandList) const;

		//Primitive Rendering

		// Misc
		RHI_PipelineCache* RetrievePipelineCache() const { return m_PipelineCache.get(); }
		RHI_DescriptorSetLayoutCache* RetrieveDescriptorLayoutCache() const { return m_DescriptorSetLayoutCache.get(); }

		const std::shared_ptr<RHI_Device>& RetrieveRHIDevice() const { return m_RHI_Device; }

	private:
		//RHI Core
		std::shared_ptr<RHI_Device> m_RHI_Device;
		std::shared_ptr<RHI_PipelineCache> m_PipelineCache;
		std::shared_ptr<RHI_DescriptorSetLayoutCache> m_DescriptorSetLayoutCache;

		std::unordered_map<RendererOptionValue, float> m_OptionValues;

		//Resolution & Viewport
		RHI_Viewport m_Viewport = RHI_Viewport(0, 0, 0, 0);
		 
		//std::shared_ptr<RHI_PipelineCache> m_RHI_PipelineCache;
	};
}