#include "Amethyst.h"
#define VMA_IMPLEMENTATION
#include "../RHI_Implementation.h"
#include "Vulkan_Utility.h"

namespace Amethyst::VulkanUtility
{
	VkDebugUtilsMessengerEXT ExtensionFunctions::Messenger = nullptr;													// Our messenger object. 
	PFN_vkCreateDebugUtilsMessengerEXT ExtensionFunctions::CreateMessenger = nullptr;									// Creates a debug messenger. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCreateDebugUtilsMessengerEXT.html
	PFN_vkDestroyDebugUtilsMessengerEXT ExtensionFunctions::DestroyMessenger = nullptr;									// Destroys a debug messenger object. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkDestroyDebugUtilsMessengerEXT.html
	PFN_vkSetDebugUtilsObjectTagEXT ExtensionFunctions::SetObjectTag = nullptr;											// Attaches arbitrary data to an object. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkSetDebugUtilsObjectTagEXT.html
	PFN_vkSetDebugUtilsObjectNameEXT ExtensionFunctions::SetObjectName = nullptr;										// Gives a user-friendly name to an object. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkSetDebugUtilsObjectNameEXT.html
	PFN_vkCmdBeginDebugUtilsLabelEXT ExtensionFunctions::MarkerBegin = nullptr;											// Open a command buffer debug label region. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDebugUtilsLabelEXT.html
	PFN_vkCmdEndDebugUtilsLabelEXT ExtensionFunctions::MarkerEnd = nullptr;												// Ends a command buffer debug label region. 
	PFN_vkGetPhysicalDeviceMemoryProperties2KHR ExtensionFunctions::RetrievePhysicalDeviceMemoryProperties_2 = nullptr; // Reports memory information for the specified physical device. This behaves simiarly to vkGetPhysicalDeviceProperties, with the ability to return extended information in a pNext chain of output structs.

	std::mutex CommandBufferImmediate::m_MutexBegin;
	std::mutex CommandBufferImmediate::m_MutexEnd;
	std::unordered_map<RHI_Queue_Type, CommandBufferImmediate::CBI_Object> CommandBufferImmediate::m_CommandBufferObjects;

	bool Image::CreateImage(RHI_Texture* texture)
	{
		// Retrieve format support.
		RHI_Format format = texture->RetrieveFormat();
		bool isRenderTargetDepthStencil = texture->IsDepthStencil();

		// VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT: Specifies that an image view can be used as a framebuffer color attachment and as an input attachment.
		// VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT: Specifies that an image view can be used as a framebuffer depth/stencil attachment and as an input attachment.
		VkFormatFeatureFlags formatFlags = isRenderTargetDepthStencil ? VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT : VK_FORMAT_FEATURE_COLOR_ATTACHMENT_BIT; // Bitmask for specifying features supported by a buffer. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkFormatFeatureFlagBits.html
		VkImageTiling imageTiling = RetrieveFormatTiling(format, formatFlags);

		// Ensure that the format is supported by the GPU.
		if (imageTiling == VK_IMAGE_TILING_MAX_ENUM)
		{
			AMETHYST_ERROR("GPU does not support the usage of %s as a %s.", RHI_Format_ToString(format), isRenderTargetDepthStencil ? "Depth Stencil Attachment" : "Color Attachment");
		}

		// Reject any image which has a non-optimal format.
		if (imageTiling != VK_IMAGE_TILING_OPTIMAL)
		{
			AMETHYST_ERROR("Format %s does not support optimal tiling, consider switching to a more efficient format.", RHI_Format_ToString(format));
		}

		// Set layout to preinitialized (required by Vulkan).
		texture->SetLayout(RHI_Image_Layout::Preinitialized);

		VkImageCreateInfo createInfo = {};
		createInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		createInfo.imageType = VK_IMAGE_TYPE_2D; // Specifies the type of an image object (basic dimensionality): 1D, 2D, 3D. 
		createInfo.flags = (texture->RetrieveResourceType() == ResourceType::TextureCube) ? VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT : 0; // Specifies that the image can be used to create a VkImageView of type VK_IMAGE_VIEW_TYPE_CUBE or VK_IMAGE_VIEW_TYPE_CUBE_ARRAY.
		createInfo.extent.width = texture->RetrieveWidth();
		createInfo.extent.height = texture->RetrieveHeight();
		createInfo.extent.depth = 1;
		createInfo.mipLevels = texture->RetrieveMipCount(); // The number of levels of details avaliable for minified sampling of the image.
		createInfo.arrayLayers = texture->RetrieveArraySize();
		createInfo.format = VulkanFormat[format]; // The format and type of texel blocks that will be contained in the image.
		createInfo.tiling = VK_IMAGE_TILING_OPTIMAL; // Specifies the tiling arragement of the texel blocks in memory. Ideally, we can always choose optimal for this.
		createInfo.initialLayout = VulkanImageLayout[static_cast<uint8_t>(texture->RetrieveLayout())];
		createInfo.usage = RetrieveUsageFlags(texture); // The intended usage of the image: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkImageUsageFlagBits.html
		createInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		createInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE; // Sharing mode (exclusive/concurrent) when accessed by multiple queue families. 

		VmaAllocationCreateInfo allocationInfo = {};

		/* VMA_MEMORY_USAGE_GPU_ONLY. See: https://gpuopen-librariesandsdks.github.io/VulkanMemoryAllocator/html/vk__mem__alloc_8h.html#aa5846affa1e9da3800e3e78fae2305cc
		
			Memory will be used on device only, so fast access from the device is preferred. It usually means device-local GPU (video) memory. 
			No need to be mappable on the host. Usages: resources written and read by device (images used as attachments) / resources transferred from host once 
			(immutable) or infrequently and read by device multiple times, e.g. textures to be sampled, vertex buffers, uniform (constant) buffers and majority of other types of resources used on the GPU.
		*/
		allocationInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY; 

		// Create image, allocate memory and bind memory to image.
		VmaAllocation memoryAllocation;
		void* resource = nullptr; // Our image buffer.
		if (!Error::CheckResult(vmaCreateImage(Globals::g_RHI_Context->m_Allocator, &createInfo, &allocationInfo, reinterpret_cast<VkImage*>(&resource), &memoryAllocation, nullptr)))
		{
			return false;
		}

		texture->SetResource(resource);

		// Keep allocation reference.
		Globals::g_RHI_Context->m_Allocations[texture->RetrieveObjectID()] = memoryAllocation;

		return true;
	}

	void Image::DestroyImage(RHI_Texture* texture)
	{
		void* resource = texture->RetrieveResource();
		uint64_t allocationID = texture->RetrieveObjectID();

		auto it = Globals::g_RHI_Context->m_Allocations.find(allocationID);
		if (it != Globals::g_RHI_Context->m_Allocations.end())
		{
			VmaAllocation allocation = it->second;
			vmaDestroyImage(Globals::g_RHI_Context->m_Allocator, static_cast<VkImage>(resource), allocation);
			Globals::g_RHI_Context->m_Allocations.erase(allocationID);
			texture->SetResource(nullptr);
		}
	}

	VmaAllocation Buffer::CreateBufferAllocation(void*& buffer, const uint64_t size, VkBufferUsageFlags usageFlags, VkMemoryPropertyFlags memoryPropertyFlags, const bool writtenFrequently /*= false*/, const void* data /*= nullptr*/)
	{
		VmaAllocator allocator = Globals::g_RHI_Context->m_Allocator;

		VkBufferCreateInfo bufferCreateInfo = {};
		bufferCreateInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferCreateInfo.size = size; // Size in bytes of the buffer to be created.
		bufferCreateInfo.usage = usageFlags; // Bitmask of VkBufferUsageFlagBits specifying allowed usages of the buffer, such as occupying a VkDescriptorSet slot of Uniform Buffer, Stgorage Buffer etc. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkBufferUsageFlagBits.html
		bufferCreateInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		bool usedForStaging = (usageFlags & VK_BUFFER_USAGE_TRANSFER_SRC_BIT) != 0; // If it has a transfer source flag, it specifies that the buffer is to be used as the source of a transfer command.

		VmaAllocationCreateInfo allocateCreateInfo = {};
		allocateCreateInfo.usage = usedForStaging ? VMA_MEMORY_USAGE_CPU_ONLY : (writtenFrequently ? VMA_MEMORY_USAGE_CPU_TO_GPU : VMA_MEMORY_USAGE_GPU_ONLY);
		allocateCreateInfo.preferredFlags = memoryPropertyFlags;

		// Create buffer, allocate memory and bind to buffer.
		VmaAllocation bufferAllocation = nullptr;
		VmaAllocationInfo allocationInfo;

		if (!Error::CheckResult(vmaCreateBuffer(allocator, &bufferCreateInfo, &allocateCreateInfo, reinterpret_cast<VkBuffer*>(&buffer), &bufferAllocation, &allocationInfo)))
		{
			return nullptr;
		}

		// Keep allocation reference.
		Globals::g_RHI_Context->m_Allocations[reinterpret_cast<uint64_t>(buffer)] = bufferAllocation;

		// If a pointer to a data buffer has been passed, map the buffer and copy over the data.
		if (data != nullptr)
		{
			VkMemoryPropertyFlags memoryFlags;
			vmaGetMemoryTypeProperties(allocator, allocationInfo.memoryType, &memoryFlags);

			bool isMappable = (memoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0; // It is mappable only if the host can see the memory.
			bool isHostCoherent = (memoryFlags & VK_MEMORY_PROPERTY_HOST_COHERENT_BIT) != 0; // It is host coherent if VK_MEMORY_PROPERTY_HOST_COHERENT_BIT is present.

			if (isMappable) 
			{
				if (!isHostCoherent) // If it is not host coherent...
				{
					if (!Error::CheckResult(vmaInvalidateAllocation(allocator, bufferAllocation, 0, size))) // Before reading, invalidate allocation.
					{
						return bufferAllocation;
					}
				}

				void* mappedMemory = nullptr;
				if (Error::CheckResult(vmaMapMemory(allocator, bufferAllocation, &mappedMemory))) // If our memory is successfully mapped...
				{
					memcpy(mappedMemory, data, size); // Copy memory into our GPU.

					if (!isHostCoherent) // If it is not host coherent...
					{
						if (!Error::CheckResult(vmaFlushAllocation(allocator, bufferAllocation, 0, size))) // After writing, flush caches.
						{
							return bufferAllocation; // It is fine to leave our memory mapped and just return here. See: https://stackoverflow.com/questions/64296581/do-i-need-to-memory-map-unmap-a-buffer-every-time-the-content-of-the-buffer-chan
						}
					}

					vmaUnmapMemory(allocator, bufferAllocation); // Unmap when done.
				}
			}
			else
			{
				AMETHYST_ERROR("Allocation ended up in non-mappable memory. You need to create a CPU-side buffer in VMA_MEMORY_USAGE_CPU_ONLY and make a transfer.");
			}
		}

		return bufferAllocation;
	}

	void Buffer::DestroyBufferAllocation(void*& buffer)
	{
		if (!buffer)
		{
			return;
		}

		uint64_t allocationID = reinterpret_cast<uint64_t>(buffer);
		auto it = Globals::g_RHI_Context->m_Allocations.find(allocationID);
		if (it != Globals::g_RHI_Context->m_Allocations.end())
		{
			VmaAllocation allocation = it->second;
			vmaDestroyBuffer(Globals::g_RHI_Context->m_Allocator, static_cast<VkBuffer>(buffer), allocation);
			Globals::g_RHI_Context->m_Allocations.erase(allocationID);
			buffer = nullptr;
		}
	}
}