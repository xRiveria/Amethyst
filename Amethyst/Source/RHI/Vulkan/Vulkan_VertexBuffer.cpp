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
		if (m_Mapped)
		{
			vmaUnmapMemory(m_RHI_Device->RetrieveContextRHI()->m_Allocator, static_cast<VmaAllocation>(m_Allocation));
			m_Mapped = nullptr;
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
		*/

		bool useStagingBuffer = vertices != nullptr;
		if (!useStagingBuffer)
		{

		}
		else
		{
			/*
				The reason we use staging buffers is because while memory with VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT is not mappable/accessible by the CPU on dedicated 
				graphics cards, it is fast. We want that.

				We will thus create 1 staging buffer in CPU accessible memory to upload the data from the vertex array to, and the final vertex buffer in device local 
				memory: VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT.
			*/

			// Create staging/source buffer and copy the vertices to it.
			void* stagingBuffer = nullptr;
			VmaAllocation stagingAllocation = VulkanUtility::Buffer::CreateBufferAllocation(stagingBuffer, m_Size_GPU, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, false, vertices);
			if (!stagingAllocation)
			{
				return false;
			}

			// Create destination buffer.
			bool writtenFrequently = false;
			// Note that the data is null here as we are tranferring the data from our staging buffer.
			VmaAllocation destinationAllocation = VulkanUtility::Buffer::CreateBufferAllocation(m_Buffer, m_Size_GPU, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, writtenFrequently, nullptr);
			if (!destinationAllocation)
			{
				return false;
			}

			// Copy staging buffer to destination buffer.
			{
				// Create command buffer.
				VkCommandBuffer commandBuffer = VulkanUtility::CommandBufferImmediate::BeginRecording(RHI_Queue_Type::RHI_Queue_Transfer);

			}
		}
	}
}