#include "Amethyst.h"
#include "RHI_Implementation.h"

namespace Amethyst
{
#ifdef API_GRAPHICS_VULKAN
	bool RHI_Context::InitializeAllocator()
	{
		VmaAllocatorCreateInfo allocatorInfo = {};
		allocatorInfo.physicalDevice = m_PhysicalDevice;
		allocatorInfo.device = m_LogicalDevice;
		allocatorInfo.instance = m_VulkanInstance;
		allocatorInfo.vulkanApiVersion = m_APIVersion;

		return VulkanUtility::Error::CheckResult(vmaCreateAllocator(&allocatorInfo, &m_Allocator));
	}

	void RHI_Context::DestroyAllocator()
	{
		if (m_Allocator != nullptr)
		{
			vmaDestroyAllocator(m_Allocator);
			m_Allocator = nullptr;
		}
	}

#endif
}