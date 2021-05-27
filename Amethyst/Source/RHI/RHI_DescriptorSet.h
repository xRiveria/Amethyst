#pragma once
#include "../Core/AmethystObject.h"
#include "RHI_Descriptor.h"

namespace Amethyst
{
	class RHI_DescriptorSet : public AmethystObject
	{
	public:
		RHI_DescriptorSet() = default;
		RHI_DescriptorSet(const RHI_Device* rhi_Device, const RHI_DescriptorSetLayoutCache* descriptorSetLayoutCache, const std::vector<RHI_Descriptor>& descriptors);
		~RHI_DescriptorSet();

		void* RetrieveResource() { return m_Resource; }

	private:
		bool Create();
		void Update(const std::vector<RHI_Descriptor>& descriptors);

	private:
		void* m_Resource = nullptr;
		const RHI_DescriptorSetLayoutCache* m_DescriptorSetLayoutCache = nullptr;
		const RHI_Device* m_RHI_Device = nullptr;
	};
}