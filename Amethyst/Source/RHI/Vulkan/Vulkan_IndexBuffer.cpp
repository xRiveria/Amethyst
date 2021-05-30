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

	/* Memory Coherence
		
		Memory is memory. However, different things can access that memory. The GPU can access the memory, the CPU can access memory, perhaps other hardware bits - whatever.

		A particular thing has "coherent" access to memory if changes made by others to that memory are visible to the reader. Now, this might be silly. After all, if 
		memory has indeed been changed, how could someone possibly be unable to see it?

		Simply put, caches.

		It turns out that changing memory is expensive. So we do everything possible to avoid changing memory unless we absolutely have to. When you write a single byte 
		from the CPU to a pointer in memory, the CPU doesn't write that byte yet. Or at least, not to memory. It writes it to a local copy of that memory called a "cache".

		The reason for this is that generally speaking, applications do not write (or read) single bytes. They are more likely to write (and read) lots of bytes in small 
		chunks. So if you're going to perform an expensive operation like a memory load or store. you should load or store a large chunk of memory. Thus, you store all 
		of the changes you're going to make to a chunk of memory in a cache, then make a single write of that cached chunk to actual memory at some point in the future.

		However, if you have two seperate devices that use the same memory, you need some way to be certain that writes one device makes are visible to other devices. Most GPUs
		can't read the CPU cache. And most CPU languages don't have language-level support that say "Hey, that stuff I wrote to memory? I really mean for you to write it 
		to memory now". So you usually need something to ensure visiblity of changes.

		In Vulkan, memory that is labelled as "HOST_COHERENT" means that, if you write to that GPU memory (via a CPU mapped pointer, since that's the only way Vulkan lets you
		directly write to memory), you don't have to need to use special features to make sure the GPU can see those changes. The GPU's visiblity of any changes is 
		guarenteed. If that flag isn't avaliable on the memory, then you must use Vulkan APIs to ensure the coherency of specific regions of data you want to access.

		With coherent memory, one of two things is going on in terms of hardware. Either CPU access to the memory is not cached in any of the CPU's caches, or the GPU 
		has direct access to the CPU's caches (perhaps due to being on the same die as the CPU(s)). You can usually tell that the latter is happening, because on-die GPU 
		implementations of Vulkan don't bother to offer non-coherent memory options.
		
		Without this flag, before reading/writing of the GPU memory (via the CPU pointer) we need to call vkFlushMappedMemoryRanges after writing 
		to the mapped pointer (to expunge writes from the cache lines) and vkInvalidateMappedMemoryRanges (discarding cache lines which may contain stale copies of the data)
		before reading the (GPU) memory via a mapped CPU pointer make sure caches are flushed/invalidated automatically. 

		Imagine a void* dataBuffer (a CPU pointer) that points to some memory on the GPU. After we write to the memory or before reading said memory, we will need to 
		flush/invalidate the memory ranges so that our GPU is aware of the changes we made to that memory. This is if we our memory is not marked as VK_MEMORY_PROPERTY_HOST_COHERENT_BIT.
		With the flag, the GPU's visibility of the changes are guaranteed. 
	*/

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
