#include "Amethyst.h"
#include "../RHI/RHI_Semaphore.h"
#include "../RHI/RHI_Implementation.h"

namespace Amethyst
{
	RHI_Semaphore::RHI_Semaphore(RHI_Device* rhi_Device, bool isTimeline /*= false*/, const char* name /*= nullptr*/)
	{
		m_IsTimelineSemaphore = isTimeline;
		m_RHI_Device = rhi_Device;

		/* Two Types of Semaphores in Vulkan
			
			- VK_SEMAPHORE_TYPE_BINARY: Has a boolean payload indicating if the semaphore is currently signaled or unsignaled. Default state of unsignaled.

			- VK_SEMAPHORE_TYPE_TIMELINE: Has a strictly increasing 64-bit unsiogned integer payload indicating if the semaphore is signaled with respect to a particular reference value. 
			Default state equals initialValue field in creation struct. This value can be a monotonic timestamp, framecount etc.
		*/

		VkSemaphoreTypeCreateInfo semaphoreTypeCreateInfo = {};
		semaphoreTypeCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_TYPE_CREATE_INFO;
		semaphoreTypeCreateInfo.pNext = nullptr;
		semaphoreTypeCreateInfo.semaphoreType = VK_SEMAPHORE_TYPE_TIMELINE;
		semaphoreTypeCreateInfo.initialValue = 0;
		
		VkSemaphoreCreateInfo semaphoreCreateInfo = {};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		semaphoreCreateInfo.pNext = m_IsTimelineSemaphore ? &semaphoreTypeCreateInfo : nullptr;
		semaphoreCreateInfo.flags = 0;

		// Create
		if (!VulkanUtility::Error::CheckResult(vkCreateSemaphore(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, &semaphoreCreateInfo, nullptr, reinterpret_cast<VkSemaphore*>(&m_Resource))))
		{
			return;
		}

		// Name
		if (name)
		{
			m_Name = name;
			VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkSemaphore>(m_Resource), name);
		}
	}

	RHI_Semaphore::~RHI_Semaphore()
	{
		if (!m_Resource)
		{
			return;
		}

		// Wait in case it's still in use by the GPU.
		m_RHI_Device->Queue_WaitAll();

		vkDestroySemaphore(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkSemaphore>(m_Resource), nullptr);

		m_Resource = nullptr;
	}

	bool RHI_Semaphore::Wait(const uint64_t value, const uint64_t timeout /*= (std::numeric_limits<uint64_t>::max)()*/)
	{
		AMETHYST_ASSERT(m_IsTimelineSemaphore);

		// All elements of pSemaphores must reference a semaphore created with a VkSemaphoreType of VK_SEMAPHORE_TYPE_TIMELINE.
		VkSemaphoreWaitInfo semaphoreWaitInfo = {};
		{
			semaphoreWaitInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_WAIT_INFO;
			semaphoreWaitInfo.pNext = nullptr;
			semaphoreWaitInfo.flags = 0;
			semaphoreWaitInfo.semaphoreCount = 1;
			semaphoreWaitInfo.pSemaphores = reinterpret_cast<VkSemaphore*>(&m_Resource);
			semaphoreWaitInfo.pValues = &value;
		}

		//Waits for timeline semaphores on the host. We will block and wait until the condition is specified (VK_SUCCESS) or the timeout expires (VK_TIMEOUT), whichever is sooner.
		return VulkanUtility::Error::CheckResult(vkWaitSemaphores(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, &semaphoreWaitInfo, timeout));
	}

	bool RHI_Semaphore::Signal(const uint64_t value)
	{
		AMETHYST_ASSERT(m_IsTimelineSemaphore);
		
		VkSemaphoreSignalInfo semaphoreSignalInfo = {};
		semaphoreSignalInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_SIGNAL_INFO;
		semaphoreSignalInfo.pNext = nullptr;
		semaphoreSignalInfo.semaphore = static_cast<VkSemaphore>(m_Resource);
		semaphoreSignalInfo.value = value;

		// Signal a timeline semaphore on the host. Once executed, it sets the timeline semaphore to the given value.
		return VulkanUtility::Error::CheckResult(vkSignalSemaphore(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, &semaphoreSignalInfo));
	}

	uint64_t RHI_Semaphore::RetrieveValue()
	{
		AMETHYST_ASSERT(m_IsTimelineSemaphore);

		uint64_t value = 0;

		// Queries the current state of the timeline semaphore.
		VulkanUtility::Error::CheckResult(vkGetSemaphoreCounterValue(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkSemaphore>(m_Resource), &value));
		return value;
	}
}