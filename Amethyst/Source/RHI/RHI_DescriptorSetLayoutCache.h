#pragma once
#include "../Core/AmethystObject.h"
#include "RHI_Descriptor.h"
#include "RHI_PipelineState.h"
#include <atomic>
#include <unordered_map>

namespace Amethyst
{
	class RHI_DescriptorSetLayoutCache : public AmethystObject
	{
	public:
		RHI_DescriptorSetLayoutCache(const RHI_Device* rhi_Device);
		~RHI_DescriptorSetLayoutCache();

		void SetPipelineState(RHI_PipelineState& pipelineState);
		void Reset(uint32_t descriptorSetCapacity = 0);

		//Descriptor resource updating.
		bool SetConstantBuffer(const uint32_t slot, RHI_ConstantBuffer* constantBuffer);
		void SetSampler(const uint32_t slot, RHI_Sampler* sampler);
		void SetTexture(const uint32_t slot, RHI_Texture* texture, const bool storage);

		RHI_DescriptorSetLayout* RetrieveCurrentDescriptorSetLayout() const {  }
		bool RetrieveDescriptorSet(RHI_DescriptorSet*& descriptorSet);
		void* RetrieveResource_DescriptorPool() const { return m_DescriptorPool; }

		//Capacity

	private:
		uint32_t RetrieveDescriptorSetCount() const;
		void SetDescriptorSetCapacity(uint32_t descriptorSetCapacity);
		bool CreateDescriptorPool(uint32_t descriptorSetCapacity);
		void RetrieveDescriptors(RHI_PipelineState& pipelineState, std::vector<RHI_Descriptor>& descriptors);

	private:
		//Descriptor Set Layouts
		std::unordered_map<std::size_t, std::shared_ptr<RHI_DescriptorSetLayout>> m_DescriptorSetLayouts;
		RHI_DescriptorSetLayout* m_DescriptorLayoutCurrent = nullptr;
		std::vector<RHI_Descriptor> m_Descriptors;
		
		//Descriptor Pool
		void* m_DescriptorPool = nullptr;
		uint32_t m_DescriptorSetCapacity = 16;

		//Misc
		std::atomic<bool> m_AreDescriptorSetLayoutsBeingCleared = false;
		const RHI_Device* m_RHI_Device;
	};
}