#include "Amethyst.h"
#include "../RHI_Fence.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"

namespace Amethyst
{
	RHI_Fence::RHI_Fence(RHI_Device* rhi_Device, const char* name /*= nullptr*/)
	{
		m_RHI_Device = rhi_Device;

		VkFenceCreateInfo fenceInfo = {};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

		// Create Fence
		if (!VulkanUtility::Error::CheckResult(vkCreateFence(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, &fenceInfo, nullptr, reinterpret_cast<VkFence*>(&m_Resource))))
		{
			return;
		}

		// Name
		if (name)
		{
			m_Name = name;
			VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkFence>(m_Resource), name);
		}
	}

	RHI_Fence::~RHI_Fence()
	{
		if (!m_Resource)
		{
			return;
		}

		// Wait in case it's still in use by the GPU.
		m_RHI_Device->Queue_WaitAll();

		vkDestroyFence(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkFence>(m_Resource), nullptr);
		m_Resource = nullptr;
	}

	bool RHI_Fence::IsSignaled()
	{
		// Queries the status of a fence from the host (VK_SUCCESS - Signaled, VK_NOT_READY - Unsignaled)
		return vkGetFenceStatus(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, reinterpret_cast<VkFence>(m_Resource)) == VK_SUCCESS;
	}

	bool RHI_Fence::Wait(uint64_t timeout /*= (std::numeric_limits<uint64_t>::max)()*/)
	{
		// Waits for one or more fences to become signalled. waitAll is the condition that must be satisfied to successfully unblock the wait. If waitAll is VK_TRUE, then the condition is that all fences in pFences are signalled. Otherwise, at least one fence in pFences must be signalled.
		// Similarly with Semaphores, vkWaitForFences will block and wait until the condition is satisfied (VK_SUCCESS) or the timeout has expired (VK_TIMEOUT), whichever is sooner. 
		return VulkanUtility::Error::CheckResult(vkWaitForFences(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, 1, reinterpret_cast<VkFence*>(&m_Resource), true, timeout));
	}

	bool RHI_Fence::Reset()
	{
		// Resets one or more fence objects to unsignalled state.
		return IsSignaled() ? VulkanUtility::Error::CheckResult(vkResetFences(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, 1, reinterpret_cast<VkFence*>(&m_Resource))) : true;
	}
}