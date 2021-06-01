#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_ConstantBuffer.h"
#include "../RHI_Device.h"
#include "../RHI_CommandList.h"

namespace Amethyst
{
	void RHI_ConstantBuffer::_Destroy()
	{
		// Wait in case its still in use by the GPU.
		m_RHI_Device->Queue_WaitAll();

		// Unmap
		if (m_MappedMemory)
		{
			vmaUnmapMemory(m_RHI_Device->RetrieveContextRHI()->m_Allocator, static_cast<VmaAllocation>(m_BufferAllocation));
			m_MappedMemory = nullptr;
		}

		// Destroy
		VulkanUtility::Buffer::DestroyBufferAllocation(m_Buffer);
	}

	RHI_ConstantBuffer::RHI_ConstantBuffer(const std::shared_ptr<RHI_Device>& rhi_Device, const std::string& name, bool isDynamic /*= false*/)
	{
		m_RHI_Device = rhi_Device;
		m_Name = name;
		m_IsDynamic = isDynamic;
	}

	bool RHI_ConstantBuffer::_Create()
	{
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice)
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return false;
		}

		// Destroy previous buffer.
		_Destroy();

		/* Calculate required alignment based on minimum device offset alignment.
		 
			minUniformBufferOffsetAlignment is the minimum required alignment in bytes for the offset member of the VkDescriptorBufferInfo structure for uniform buffers. 
			When a descriptor of type VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC is updated, the offset must be an integer multiple 
			of this limit. Similarly, the dynamic offsets for uniform buffers must be multiples of this limit.
		*/
		size_t minimumUBOAlignment = m_RHI_Device->RetrieveContextRHI()->m_PhysicalDeviceProperties.limits.minUniformBufferOffsetAlignment;
		if (minimumUBOAlignment > 0) 
		{
			m_Stride = static_cast<uint32_t>((m_Stride + minimumUBOAlignment - 1) & ~(minimumUBOAlignment - 1)); ///
		}
		m_Size_GPU = m_OffsetCount * m_Stride;

		// Create Buffer
		VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
		memoryFlags |= !m_PersistentMapping ? VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : 0;
		bool writtenFrequently = true;
		
		// UNIFORM_BUFFER_BIT specifies that the buffer can be used in a VkDescriptorBufferInfo suitable for occupying a VkDescriptorSet slot of either VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER or VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC.
		VmaAllocation allocation = VulkanUtility::Buffer::CreateBufferAllocation(m_Buffer, m_Size_GPU, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, memoryFlags, writtenFrequently, nullptr);
		if (!allocation)
		{
			AMETHYST_ERROR("Failed to allocate buffer.");
			return false;
		}

		// Set debug name.
		VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkBuffer>(m_Buffer), "Constant Buffer");

		return true;
	}

	void* RHI_ConstantBuffer::Map()
	{
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice)
		{
			AMETHYST_ERROR_INVALID_INTERNALS();
			return nullptr;
		}

		if (!m_BufferAllocation)
		{
			AMETHYST_ERROR("Invalid allocation.");
			return nullptr;
		}

		if (!m_MappedMemory) // Let's map it.
		{
			if (!VulkanUtility::Error::CheckResult(vmaMapMemory(m_RHI_Device->RetrieveContextRHI()->m_Allocator, static_cast<VmaAllocation>(m_BufferAllocation), reinterpret_cast<void**>(&m_MappedMemory))))
			{
				AMETHYST_ERROR("Failed to map memory.");
				return nullptr;
			}
		}

		return m_MappedMemory;
	}

	bool RHI_ConstantBuffer::Unmap(const uint64_t offset /*= 0*/, const uint64_t size /*= 0*/)
	{
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice)
		{
			AMETHYST_ERROR_INVALID_INTERNALS();
			return false;
		}

		if (!m_BufferAllocation)
		{
			AMETHYST_ERROR("Invalid allocation");
			return false;
		}

		if (m_PersistentMapping) // VMA has a special way to ensure memory stays mapped, allowing us to access the CPU pointer to it any time without a need to call Map/Unmap functions. We are not using the flag VMA_ALLOCATION_CREATE_MAPPED_BIT, so we will have to continue with flushing the data to let it become visible to the GPU.
		{
			if (!VulkanUtility::Error::CheckResult(vmaFlushAllocation(m_RHI_Device->RetrieveContextRHI()->m_Allocator, static_cast<VmaAllocation>(m_BufferAllocation), offset, size != 0 ? size : VK_WHOLE_SIZE))) // VK_WHOLE_SIZE maps from offset to the end of the allocation.
			{
				AMETHYST_ERROR("Failed to flush memory.");
				return false;
			}
		}
		else
		{
			if (m_MappedMemory)
			{
				vmaUnmapMemory(m_RHI_Device->RetrieveContextRHI()->m_Allocator, static_cast<VmaAllocation>(m_BufferAllocation));
				m_MappedMemory = nullptr;
			}
		}

		return true;
	}
}