#pragma once
#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"
#include "../RHI_VertexBuffer.h"
#include "../RHI_Vertex.h"
#include "../RHI_CommandList.h"

namespace Amethyst
{
	void RHI_VertexBuffer::_Destroy()
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

	bool RHI_VertexBuffer::_Create(const void* vertices)
	{
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice)
		{
			AMETHYST_ERROR_INVALID_INTERNALS();
			return false;
		}

		// Destroy previous buffer.
		_Destroy();

		/*
			Memory in Vulkan does not need to be unmapped before using it on the GPU, but unless a memory type has VK_MEMORY_PROPERTY_HOST_COHERENT_BIT flag set, 
			you need to manually invalidate cache before reading of the mapped pointer and flush cache after writing to the mapped pointer. Map/unmap operations don't do that automatically.
		
			Remember that mapping is only possible of GPU memory allocated from a memory type that has VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT. This tells Vulkan that our host can see this device memory and thus allows us to obtain a CPU pointer to device memory.
		*/

		bool useStagingBuffer = vertices != nullptr;
		if (!useStagingBuffer)
		{
			VkMemoryPropertyFlags memoryFlags = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT;
			memoryFlags |= !m_PersistentMapping ? VK_MEMORY_PROPERTY_HOST_COHERENT_BIT : 0; // Persistently mapped memory allows us to access the CPU pointer to it any time without a need to call any map or unmap functions.
			
			VmaAllocation allocation = VulkanUtility::Buffer::CreateBufferAllocation(m_Buffer, m_Size_GPU, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, memoryFlags, true);
			if (!allocation)
			{
				return false;
			}

			m_BufferAllocation = static_cast<void*>(allocation);
			m_IsMappable = true; // As we are not using staging buffers, the memory remains on our host. We can map it. 
		}
		else
		{
			/*
				The reason we use staging buffers is because while memory with VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT is not mappable/accessible by the CPU on dedicated 
				graphics cards, it is fast. We want that. We will thus create 1 staging buffer in CPU accessible memory to upload the data from the vertex array to, 
				and transfer the data to the aforementioned device local memory.
			*/

			// Create staging/source buffer with VMA and copy the vertices to it.
			void* stagingBuffer = nullptr;
			VmaAllocation stagingAllocation = VulkanUtility::Buffer::CreateBufferAllocation(stagingBuffer, m_Size_GPU, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false, vertices);
			if (!stagingAllocation)
			{ 
				return false;
			}

			// Create destination buffer with VMA.
			bool writtenFrequently = false;
			// Note that the data is null here as we are tranferring the data from our staging buffer.
			// VK_BUFFER_USAGE_VERTEX_BUFFER_BIT specifies that the buffer is suitable for passing as an element of the pBuffers array to VkCmdBindVertexBuffers.
			VmaAllocation destinationAllocation = VulkanUtility::Buffer::CreateBufferAllocation(m_Buffer, m_Size_GPU, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, writtenFrequently, nullptr);
			if (!destinationAllocation)
			{
				return false;
			}

			// Copy staging buffer to destination buffer.
			{
				// Create command buffer.
				VkCommandBuffer commandBuffer = VulkanUtility::CommandBufferImmediate::BeginRecording(RHI_Queue_Type::RHI_Queue_Transfer);

				VkBuffer* vulkanBuffer = reinterpret_cast<VkBuffer*>(&m_Buffer);
				VkBuffer* vulkanStagingBuffer = reinterpret_cast<VkBuffer*>(&stagingBuffer);

				// Copy
				VkBufferCopy copyRegion = {};
				copyRegion.size = m_Size_GPU; // The number of bytes to copy. We can also specify offsets.
				vkCmdCopyBuffer(commandBuffer, *vulkanStagingBuffer, *vulkanBuffer, 1, &copyRegion); //Copies the data between buffer regions.

				// Flush and free command buffer.
				if (!VulkanUtility::CommandBufferImmediate::EndRecordingAndSubmit(RHI_Queue_Type::RHI_Queue_Transfer))
				{
					return false;
				}

				// Destroy Staging Resources
				VulkanUtility::Buffer::DestroyBufferAllocation(stagingBuffer);
			}

			m_BufferAllocation = static_cast<void*>(destinationAllocation);
			m_IsMappable = false; // Updates can only be done through staging as well, not direct mapping as the data is now on the device.
		}

		// Set debug name.
		VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkBuffer>(m_Buffer), "Vertex Buffer");

		return true;
	}

	void* RHI_VertexBuffer::Map()
	{
		if (!m_IsMappable)
		{
			AMETHYST_ERROR("Not mappable. Buffer can only be updated through staging.");
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

	bool RHI_VertexBuffer::Unmap()
	{
		if (!m_IsMappable)
		{
			AMETHYST_ERROR("Not mappable. Data can only be updated via staging.");
			return false;
		}

		if (!m_BufferAllocation)
		{
			AMETHYST_ERROR("Invaliad allocation");
			return false;
		}

		if (m_PersistentMapping) // VMA has a special way to ensure memory stays mapped, allowing us to access the CPU pointer to it any time without a need to call Map/Unmap functions. We are not using the flag VMA_ALLOCATION_CREATE_MAPPED_BIT, so we will have to continue with unmapping before using its data on the GPU.
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