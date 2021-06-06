#include "Amethyst.h"
#include "../RHI/RHI_Implementation.h"
#include "../RHI_DescriptorSetLayoutCache.h"
#include "../RHI_Shader.h"

namespace Amethyst
{
	RHI_DescriptorSetLayoutCache::~RHI_DescriptorSetLayoutCache()
	{
		if (m_DescriptorPool)
		{
			// Wait in case it's still in use by the GPU.
			m_RHI_Device->Queue_WaitAll();

			vkDestroyDescriptorPool(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkDescriptorPool>(m_DescriptorPool), nullptr);
			m_DescriptorPool = nullptr;
		}
	}

	void RHI_DescriptorSetLayoutCache::ResetDescriptorCache(uint32_t descriptorSetCapacity /*= 0*/)
	{
		// If the requested capacity is zero, then only recreate the existing descriptor pool. Otherwise, it will not be a recreation but a new one with a new capacity.
		if (descriptorSetCapacity == 0)
		{
			descriptorSetCapacity = m_DescriptorSetCapacity;
		}

		// Destroy Layouts
		m_AreDescriptorSetLayoutsBeingCleared = true;
		m_DescriptorSetLayouts.clear();
		m_AreDescriptorSetLayoutsBeingCleared = false;
		m_DescriptorSetLayoutCurrent = nullptr;

		// Destroy Pool
		if (m_DescriptorPool)
		{
			// Wait in case its still in use by the GPU.
			m_RHI_Device->Queue_WaitAll();

			vkDestroyDescriptorPool(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkDescriptorPool>(m_DescriptorPool), nullptr);
			m_DescriptorPool = nullptr;
		}

		// Create Pool
		CreateDescriptorPool(descriptorSetCapacity);

		// Log
		if (descriptorSetCapacity > m_DescriptorSetCapacity)
		{
			AMETHYST_INFO("Capacity has been increased to %d elements.", descriptorSetCapacity);
		}
		else if (descriptorSetCapacity < m_DescriptorSetCapacity)
		{
			AMETHYST_INFO("Capacity has been decreased to %d elements.", descriptorSetCapacity);
		}
		else
		{
			AMETHYST_INFO("Descriptor pool has been reset.");
		}
	}

	void RHI_DescriptorSetLayoutCache::SetDescriptorSetCapacity(uint32_t descriptorSetCapacity)
	{
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI())
		{
			AMETHYST_ERROR_INVALID_INTERNALS();
			return;
		}

		if (m_DescriptorSetCapacity == descriptorSetCapacity)
		{
			AMETHYST_INFO("Descriptor set capacity is already at %d elements.", m_DescriptorSetCapacity);
			return;
		}

		// Re-create descriptor pool.
		ResetDescriptorCache(descriptorSetCapacity);

		// Update Capacity
		m_DescriptorSetCapacity = descriptorSetCapacity;
	}


	bool RHI_DescriptorSetLayoutCache::CreateDescriptorPool(uint32_t descriptorSetCapacity)
	{
		// Pool Sizes. Specifies the type of descriptor and the number of descriptors of mentioned type to allocate. 
		std::array<VkDescriptorPoolSize, 5> poolSizes =
		{
			VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_SAMPLER,				  g_RHI_Descriptor_Max_Samplers },
			VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,		  g_RHI_Descriptor_Max_Textures },
			VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_STORAGE_IMAGE,		  g_RHI_Descriptor_Max_StorageTextures },
			VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,		  g_RHI_Descriptor_Max_ConstantBuffers },
			VkDescriptorPoolSize { VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, g_RHI_Descriptor_Max_ConstantBuffersDynamic }
		};

		// Creation Info
		VkDescriptorPoolCreateInfo poolCreationInfo = {};
		{
			poolCreationInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolCreationInfo.flags = 0;
			poolCreationInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size()); // Number of elements in pPoolSizes.
			poolCreationInfo.pPoolSizes = poolSizes.data(); // Pointer to an array of vkDescriptorPoolSize structures, each containing a descriptor type and number of descriptors of that type to be allocated in the pool.
			poolCreationInfo.maxSets = descriptorSetCapacity; // Maximum number of descriptor sets that can be allocated from the pool.
		}

		// Pool
		VkDescriptorPool* descriptorPool = reinterpret_cast<VkDescriptorPool*>(&m_DescriptorPool);
		return VulkanUtility::Error::CheckResult(vkCreateDescriptorPool(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, &poolCreationInfo, nullptr, descriptorPool));
	}
}