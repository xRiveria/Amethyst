#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"
#include "../RHI_IndexBuffer.h"
#include "../RHI_CommandList.h"

namespace Amethyst
{
	void RHI_IndexBuffer::_Destroy()
	{
		// Wait in case it's still in use by the GPU.
		m_RHI_Device->Queue_WaitAll();

		// Unmap
		if (m_MappedMemory) // If we have currently have a mapped pointer to GPU memory...
		{
			vmaUnmapMemory(m_RHI_Device->RetrieveContextRHI()->m_Allocator, static_cast<VmaAllocation>(m_BufferAllocation));
			m_MappedMemory = nullptr;
		}

		// Destroy
		VulkanUtility::Buffer::DestroyBufferAllocation(m_Buffer);
	}

	bool RHI_IndexBuffer::_Create(const void* indices)
	{
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice)
		{
			AMETHYST_ERROR_INVALID_INTERNALS();
			return false;
		}

		RHI_Context* rhi_Context = m_RHI_Device->RetrieveContextRHI();

		// Destroy previous Buffer.
		_Destroy();

		bool useStagingBuffer = indices != nullptr;
		if (!useStagingBuffer)
		{
			VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			memoryFlags |= !m_PersistentMapping ? VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : 0;

			VmaAllocation allocation = VulkanUtility::Buffer::CreateBufferAllocation(m_Buffer, m_Size_GPU, VK_BUFFER_USAGE_INDEX_BUFFER_BIT, memoryFlags, true);
			if (!allocation)
			{
				return false;
			}

			m_BufferAllocation = static_cast<void*>(allocation);
			m_IsMappable = true;
		}
		else
		{
			// Create staging/source buffer with VMA and copy the indices to it.
			void* stagingBuffer = nullptr;
			VmaAllocation stagingAllocation = VulkanUtility::Buffer::CreateBufferAllocation(stagingBuffer, m_Size_GPU, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false, indices);
			if (!stagingAllocation)
			{
				return false;
			}

			// Create destination buffer with VMA.
			// INDEX_BUFFER_BIT specifies that the buffer is suitable for passing as the buffer parameter to vkCmdBindIndexBuffer.
			VmaAllocation destinationAllocation = VulkanUtility::Buffer::CreateBufferAllocation(m_Buffer, m_Size_GPU, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
			if (!destinationAllocation)
			{
				return false;
			}

			// Copy staging buffer to destination buffer.
			{
				// Create command buffer.
				VkCommandBuffer commandBuffer = VulkanUtility::CommandBufferImmediate::BeginRecording(RHI_Queue_Type::RHI_Queue_Transfer);

				VkBuffer* vulkanDestinationBuffer = reinterpret_cast<VkBuffer*>(&m_Buffer);
				VkBuffer* vulkanStagingBuffer = reinterpret_cast<VkBuffer*>(&stagingBuffer);

				// Copy
				VkBufferCopy copyRegionInfo = {};
				copyRegionInfo.size = m_Size_GPU;
				vkCmdCopyBuffer(commandBuffer, *vulkanStagingBuffer, *vulkanDestinationBuffer, 1, &copyRegionInfo);

				// Flush and free command buffer.
				if (!VulkanUtility::CommandBufferImmediate::EndRecordingAndSubmit(RHI_Queue_Type::RHI_Queue_Transfer))
				{
					return false;
				}

				// Destroy staging buffer.
				VulkanUtility::Buffer::DestroyBufferAllocation(stagingBuffer);
			}

			m_BufferAllocation = static_cast<void*>(destinationAllocation);
			m_IsMappable = false;
		}
		
		// Set debug name.
		VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkBuffer>(m_Buffer), "Index Buffer");
		
		return true;
	}

	void* RHI_IndexBuffer::Map()
	{
		if (!m_IsMappable)
		{
			AMETHYST_ERROR("Not mappable. Buffer can only be updated via staging.");
			return nullptr;
		}

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

		if (!m_MappedMemory)
		{
			if (!VulkanUtility::Error::CheckResult(vmaMapMemory(m_RHI_Device->RetrieveContextRHI()->m_Allocator, static_cast<VmaAllocation>(m_BufferAllocation), reinterpret_cast<void**>(&m_MappedMemory))))
			{
				AMETHYST_ERROR("Failed to map memory.");
				return nullptr;
			}
		}
		
		return m_MappedMemory;
	}

	bool RHI_IndexBuffer::Unmap()
	{
		if (!m_IsMappable)
		{
			AMETHYST_ERROR("Not mappable. Buffer can only be updated via staging.");
			return false;
		}

		if (!m_BufferAllocation)
		{
			AMETHYST_ERROR("Invalid allocation.");
			return false;
		}

		if (m_PersistentMapping) // Persistent memory, unless created with the VMA flag VM_ALLOCATION_CREATE_MAPPED_BIT, must be mapped/unmapped before using its data on the GPU.
		{
			if (!VulkanUtility::Error::CheckResult(vmaFlushAllocation(m_RHI_Device->RetrieveContextRHI()->m_Allocator, static_cast<VmaAllocation>(m_BufferAllocation), 0, m_Size_GPU)))
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
