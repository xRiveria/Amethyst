#include "Amethyst.h"
#include "RHI_DescriptorSet.h"

namespace Amethyst
{
	RHI_DescriptorSet::RHI_DescriptorSet(const RHI_Device* rhi_Device, const RHI_DescriptorSetLayoutCache* descriptorSetLayoutCache, const std::vector<RHI_Descriptor>& descriptors)
	{
		m_RHI_Device = rhi_Device;
		m_DescriptorSetLayoutCache = descriptorSetLayoutCache;

		if (Create())
		{
			Update(descriptors);
		}
	}
}