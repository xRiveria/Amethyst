#pragma once
#include "../Core/AmethystObject.h"
#include <unordered_map>
#include <vector>
#include <array>
#include "RHI_Descriptor.h"

namespace Amethyst
{
	class RHI_DescriptorSetLayout : public AmethystObject
	{
	public:
		RHI_DescriptorSetLayout() = default;
		RHI_DescriptorSetLayout(const RHI_Device* rhi_Device, const std::vector<RHI_Descriptor>& descriptors, const std::string& name);
		~RHI_DescriptorSetLayout();

		//Descriptor Types
		bool SetConstantBuffer(const uint32_t slot, RHI_ConstantBuffer* constantBuffer);
		void SetSampler(const uint32_t slot, RHI_Sampler* sampler);
		void SetTexture(const uint32_t slot, RHI_Texture* texture, const bool isStorage);

		bool RetrieveDescriptorSet(RHI_DescriptorSetLayoutCache* descriptorSetLayoutCache, RHI_DescriptorSet*& descriptorSet);
		const std::array<uint32_t, g_RHI_MaxConstantBufferCount> RetrieveDynamicOffsets() const;
		uint32_t RetrieveDynamicOffsetCount() const;
		uint32_t RetrieveDescriptorSetCount() const { return static_cast<uint32_t>(m_DescriptorSets.size()); }
		void NeedsToBind() { m_NeedsToBind = true; }
		void* RetrieveResource() const { return m_Resource; }

	private:
		void CreateResource(const std::vector<RHI_Descriptor>& descriptors);

	private:
		//Descriptor Set Layout
		void* m_Resource = nullptr;
		uint32_t m_Hash = 0;

		//Descriptor Sets
		std::unordered_map<uint32_t, RHI_DescriptorSet> m_DescriptorSets;

		//Descriptors
		std::vector<RHI_Descriptor> m_Descriptors;

		//Misc
		bool m_NeedsToBind = false;
		std::array<uint32_t, g_RHI_MaxConstantBufferCount> m_DynamicOffsets;
		const RHI_Device* m_RHI_Device = nullptr;
	};
}