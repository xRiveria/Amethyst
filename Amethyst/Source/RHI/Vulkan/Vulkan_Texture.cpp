#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"
#include "../RHI_Texture2D.h"
#include "../RHI_TextureCube.h"
#include "../RHI_CommandList.h"
#include "../RHI_DescriptorSetLayoutCache.h"
#include "../Rendering/Renderer.h"

namespace Amethyst
{
	inline void SetDebugName(RHI_Texture* texture)
	{
		std::string textureName = texture->RetrieveObjectName();

		// If a name hasn't been defined, try to make a reasonable one.
		if (textureName.empty())
		{
			if (texture->IsSampled())
			{
				textureName += textureName.empty() ? "Sampled" : "-Sampled";
			}

			if (texture->IsDepthStencil())
			{
				textureName += textureName.empty() ? "Depth Stencil" : "-Depth Stencil";
			}

			if (texture->IsRenderTarget())
			{
				textureName += textureName.empty() ? "Render Target" : "-Render Target";
			}
		}

		VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkImage>(texture->RetrieveResource()), textureName.c_str());
		VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkImageView>(texture->RetrieveResourceView(0)), textureName.c_str());
		if (texture->IsSampled() && texture->IsStencilFormat())
		{
			VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkImageView>(texture->RetrieveResourceView(1)), textureName.c_str());
		}
	}

	/* Texture Arrays

		Unity: A texture array is a collection of same size/format/flags 2D textures that look like a single object to the GPU. They are useful for implementating custom
		terrain rendering systems or other special effects where you need an efficient way of accessing many textures of the same size and format. Elements of a
		2D texture array are also known as slices or layers.

		Khronos: An array texture is a Texture where each mipmap level contains an array of images of the same size. Array textures may have mipmaps, but each mipmap
		in the texture has the same number of levels.

		The high cost of texture switchers resulted in a widespread preference for texture atlases (a single texture which stores data for multiple objects). This resulted
		in fewer texture switching, and thus less overhead. Array textures are an alternative to atlases, as they can store multiple layers of texture data for objects without
		some of the problems of atlases. Think how Minecraft loads in texture data through texture sheets - those are texture arrays! Each part of the array has a mipmap
		level index, which allows for multiple mip-map levels. See: https://image.slidesharecdn.com/sa2008modernopengl-1231549184153966-1/95/siggraph-asia-2008-modern-opengl-56-638.jpg?cb=1422672731
	*/

	inline bool CopyToStagingBuffer(RHI_Texture* texture, std::vector<VkBufferImageCopy>& bufferImageCopies, void*& stagingBuffer)
	{
		if (!texture->HasData())
		{
			AMETHYST_WARNING("No data to stage.");
			return true;
		}

		const uint32_t textureWidth = texture->RetrieveWidth();
		const uint32_t textureHeight = texture->RetrieveHeight();
		const uint32_t arraySize = texture->RetrieveArraySize();
		const uint32_t mipLevels = texture->RetrieveMipCount();
		const uint32_t bytesPerPixel = texture->RetrieveBytesPerPixel();

		// Fill out VkBufferImageCopy structs describing the array and mip-levels.
		VkDeviceSize bufferOffset = 0;
		for (uint32_t arrayIndex = 0; arrayIndex < arraySize; arrayIndex++) // For each array texture...
		{
			for (uint32_t mipIndex = 0; mipIndex < mipLevels; mipIndex++) // For each of its mip levels...
			{
				uint32_t mipWidth = textureWidth >> mipIndex; // Example: 1024 >> 3 = 128.
				uint32_t mipHeight = textureHeight >> mipIndex; // Example: 1024 >> 3 = 128.

				VkBufferImageCopy regionInfo = {};
				regionInfo.bufferOffset = bufferOffset; // Offset in bytes from the start of the buffer object where the image data is copied from or to.

				/*
					bufferRowLength and bufferImageHeight specify in texels a subregion of a larger two or three-dimensional image in buffer memory, and control the 
					addressing calculations. If either of these values is zero, that aspect of the buffer memory is considered to be tightly packed according to imageExtent.
				*/
				regionInfo.bufferRowLength = 0;
				regionInfo.bufferImageHeight = 0;
				regionInfo.imageSubresource.aspectMask = VulkanUtility::Image::RetrieveAspectMask(texture);
				regionInfo.imageSubresource.mipLevel = mipIndex;
				regionInfo.imageSubresource.baseArrayLayer = arrayIndex;
				regionInfo.imageSubresource.layerCount = arraySize;
				regionInfo.imageOffset = { 0, 0, 0 };
				regionInfo.imageExtent = { mipWidth, mipHeight, 1 };

				bufferImageCopies[mipIndex] = regionInfo;

				// Update staging buffer memory requirement (in bytes).
				bufferOffset += mipWidth * mipHeight * bytesPerPixel;
			}
		}

		// Create staging buffer. VK_BUFFER_USAGE_TRANSFER_SRC_BIT specifies that the buffer can be used as the source of a transfer command. 
		VmaAllocation allocation = VulkanUtility::Buffer::CreateBufferAllocation(stagingBuffer, bufferOffset, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

		// Copy array and mip-level data to the staging buffer.
		void* data = nullptr; // Staging buffer.
		bufferOffset = 0;
		if (VulkanUtility::Error::CheckResult(vmaMapMemory(VulkanUtility::Globals::g_RHI_Context->m_Allocator, allocation, &data)))
		{
			for (uint32_t arrayIndex = 0; arrayIndex < arraySize; arrayIndex++) // For each array texture...
			{
				for (uint32_t mipIndex = 0; mipIndex < mipLevels; mipIndex++) // For each of its mip levels...
				{
					uint64_t bufferSize = (textureWidth >> mipIndex) * (textureHeight >> mipIndex) * bytesPerPixel;
					memcpy(static_cast<std::byte*>(data) + bufferOffset, texture->RetrieveMip(arrayIndex + mipIndex).data(), bufferSize);
					bufferOffset += bufferSize;
				}
			}

			vmaUnmapMemory(VulkanUtility::Globals::g_RHI_Context->m_Allocator, allocation);
		}

		return true;
	}

	inline bool StageMemoryAndCopyBufferToImage(RHI_Texture* texture, RHI_Image_Layout& textureLayout)
	{
		// Copy the texture's data to a staging buffer.
		void* stagingBuffer = nullptr;
		std::vector<VkBufferImageCopy> bufferImageCopies(texture->RetrieveMipCount()); // Structure specifying a buffer image copy operation. Can be used to copy specific regions of an image.
		if (!CopyToStagingBuffer(texture, bufferImageCopies, stagingBuffer))
		{
			return false;
		}

		// Copy the staging buffer into the image.
		if (VkCommandBuffer commandBuffer = VulkanUtility::CommandBufferImmediate::BeginRecording(RHI_Queue_Type::RHI_Queue_Graphics))
		{
			// Optimal layout for images which are the destination of a transfer.
			RHI_Image_Layout layout = RHI_Image_Layout::Transfer_Destination_Optimal;

			// Transition to Layout.
			if (!VulkanUtility::Image::SetLayout(commandBuffer, texture, layout))
			{
				return false;
			}

			// Copy data from the staging buffer into our Vulkan image.
			vkCmdCopyBufferToImage(
				commandBuffer, // Command Buffer
				static_cast<VkBuffer>(stagingBuffer), // Source Buffer
				static_cast<VkImage>(texture->RetrieveResource()), // Destination Image
				VulkanImageLayout[static_cast<uint8_t>(layout)], // Destination Image Layout
				static_cast<uint32_t>(bufferImageCopies.size()), // The number of regions to copy. Each region is copied from the specified region of the source buffer to the specified region of the destination image.
				bufferImageCopies.data() // Pointer to an array of VkBufferImageCopy structures specifying the regions to copy.
			);

			// End command buffer.
			if (!VulkanUtility::CommandBufferImmediate::EndRecordingAndSubmit(RHI_Queue_Type::RHI_Queue_Graphics))
			{
				return false;
			}

			// Free staging buffer as we have already done everything we need to.
			VulkanUtility::Buffer::DestroyBufferAllocation(stagingBuffer);

			// Let the texture know about its new layout.
			textureLayout = layout;
		}

		return true;
	}

	RHI_Texture2D::~RHI_Texture2D()
	{
		if (!m_RHI_Device || !m_RHI_Device->IsInitialized())
		{
			AMETHYST_ERROR("Invalid RHI Device.");
		}

		// Wait in case its still in use by the GPU.
		m_RHI_Device->Queue_WaitAll();

		/*
			Make sure that no descriptor sets refer to this texture. Right now, we just reset the descriptor pool, which works but is not ideal.
			To Do: Retrieve only the referring descriptor sets, and simply update the slot this texture is bound to.
		*/
		if (Renderer* renderer = m_RHI_Device->RetrieveContextEngine()->RetrieveSubsystem<Renderer>())
		{
			if (RHI_DescriptorSetLayoutCache* descriptorSetLayoutCache = renderer->RetrieveDescriptorLayoutCache())
			{
				descriptorSetLayoutCache->ResetDescriptorCache();
			}
		}

		// Deallocate everything.
		m_Data.clear();
		VulkanUtility::Image::View::DestroyImageView(m_ResourceView[0]); // Color/Depth
		VulkanUtility::Image::View::DestroyImageView(m_ResourceView[1]); // Stencil

		for (uint32_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
		{
			VulkanUtility::Image::View::DestroyImageView(m_ResourceView_DepthStencil[i]);
			VulkanUtility::Image::View::DestroyImageView(m_ResourceView_RenderTarget[i]);
		}

		VulkanUtility::Image::DestroyImage(this);
	}

	void RHI_Texture::SetLayout(const RHI_Image_Layout newLayout, RHI_CommandList* commandList /*= nullptr*/)
	{
		// The texture is most likely still initializing.
		if (m_Layout == RHI_Image_Layout::Undefined)
		{
			return;
		}

		if (m_Layout == newLayout)
		{
			return;
		}

		// If a command list was provided, this means we should insert a pipeline barrier for the layout transition.
		if (commandList)
		{
			if (!VulkanUtility::Image::SetLayout(static_cast<VkCommandBuffer>(commandList->RetrieveCommandBuffer()), this, newLayout))
			{
				return;
			}

			// Profiler Stuff.
		}

		m_Layout = newLayout;
	}

	inline RHI_Image_Layout RetrieveAppropriateLayout(RHI_Texture* texture)
	{
		RHI_Image_Layout targetLayout = RHI_Image_Layout::Preinitialized;

		if (texture->IsSampled() && texture->IsColorFormat()) // If it is a simple (color) texture for sampling...
		{
			targetLayout = RHI_Image_Layout::Shader_Read_Only_Optimal;
		}

		if (texture->IsRenderTarget()) // If the texture is a render target...
		{
			targetLayout = RHI_Image_Layout::Color_Attachment_Optimal;
		}

		if (texture->IsDepthStencil()) // If it is a depth/stencil texture...
		{
			targetLayout = RHI_Image_Layout::Depth_Stencil_Attachment_Optimal;
		}

		/*
			Storage textures allow performing texture reads without sampling and store to arbitrary positions in shaders.
		*/
		if (texture->IsStorage()) // If it is a storage texture...
		{
			targetLayout = RHI_Image_Layout::General;
		}

		return targetLayout;
	}

	// Create our Image.
	bool RHI_Texture2D::CreateResourceGPU()
	{
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice)
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return false;
		}

		// Create Image
		if (!VulkanUtility::Image::CreateImage(this))
		{
			AMETHYST_ERROR("Failed to create image.");
			return false;
		}

		// If the texture has any data, stage it.
		if (HasData())
		{
			if (!StageMemoryAndCopyBufferToImage(this, m_Layout))
			{
				AMETHYST_ERROR("Failed to stage data.");
				return false;
			}
		}

		// Transition to target layout.
		if (VkCommandBuffer commandBuffer = VulkanUtility::CommandBufferImmediate::BeginRecording(RHI_Queue_Type::RHI_Queue_Graphics))
		{
			RHI_Image_Layout targetLayout = RetrieveAppropriateLayout(this);

			// Transition to the final layout.
			if (!VulkanUtility::Image::SetLayout(commandBuffer, this, targetLayout))
			{
				AMETHYST_ERROR("Failed to transition layout.");
				return false;
			}

			// Submit
			if (!VulkanUtility::CommandBufferImmediate::EndRecordingAndSubmit(RHI_Queue_Type::RHI_Queue_Graphics))
			{
				AMETHYST_ERROR("Failed to end command buffer.");
				return false;
			}

			// Update this texture with the new layout.
			m_Layout = targetLayout;
		}

		// Create Image Views. Remember that image objects are not directly accessed by pipeline shaders for reading or writing of image data. Instead, image views representating contiguous ranges of the image subresources/additional metadata are used for that purpose.
		{
			// Shader Resource Views
			if (IsSampled())
			{
				if (IsColorFormat())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView[0], this))
					{
						return false;
					}
				}

				if (IsDepthFormat())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView[0], this, 0, m_ArraySize, true, false))
					{
						return false;
					}
				}

				if (IsStencilFormat())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView[1], this, 0, m_ArraySize, false, true))
					{
						return false;
					}
				}
			}

			// Render-Target Views
			for (uint32_t i = 0; i < m_ArraySize; i++) // Can be 1 or multiple (if texture array).
			{
				if (IsRenderTarget())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView_RenderTarget[i], this, i, 1))
					{
						return false;
					}
				}

				if (IsDepthStencil())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView_DepthStencil[i], this, i, 1, true))
					{
						return false;
					}
				}
			}

			// Name the image and image views(s).
			SetDebugName(this);
		}

		return true;
	}

	// Texture Cube Stuff
	RHI_TextureCube::~RHI_TextureCube()
	{
		if (!m_RHI_Device->IsInitialized())
		{
			return;
		}

		m_RHI_Device->Queue_WaitAll();
		m_Data.clear();

		VulkanUtility::Image::View::DestroyImageView(m_ResourceView[0]);
		VulkanUtility::Image::View::DestroyImageView(m_ResourceView[1]);

		for (uint32_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
		{
			VulkanUtility::Image::View::DestroyImageView(m_ResourceView_DepthStencil[i]);
			VulkanUtility::Image::View::DestroyImageView(m_ResourceView_RenderTarget[i]);
		}

		VulkanUtility::Image::DestroyImage(this);
	}

	bool RHI_TextureCube::CreateResourceGPU()
	{
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice)
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return false;
		}

		// Create Image
		if (!VulkanUtility::Image::CreateImage(this))
		{
			AMETHYST_ERROR("Failed to create image.");
			return false;
		}

		// If the texture has any data, stage it.
		if (HasData())
		{
			if (!StageMemoryAndCopyBufferToImage(this, m_Layout))
			{
				return false;
			}
		}

		// Transition to target layout.
		if (VkCommandBuffer commandBuffer = VulkanUtility::CommandBufferImmediate::BeginRecording(RHI_Queue_Type::RHI_Queue_Graphics))
		{
			RHI_Image_Layout targetLayout = RetrieveAppropriateLayout(this);

			// Transition to the final layout.
			if (!VulkanUtility::Image::SetLayout(commandBuffer, this, targetLayout))
			{
				return false;
			}

			// Submit
			if (!VulkanUtility::CommandBufferImmediate::EndRecordingAndSubmit(RHI_Queue_Type::RHI_Queue_Graphics))
			{
				return false;
			}

			// Update this texture with the new layout.
			m_Layout = targetLayout;
		}

		// Create Image Views
		{
			// Shader Resource Views
			if (IsSampled())
			{
				if (IsColorFormat())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView[0], this))
					{
						return false;
					}
				}

				if (IsDepthFormat())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView[0], this, 0, m_ArraySize, true, false))
					{
						return false;
					}
				}

				if (IsStencilFormat())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView[1], this, 0, m_ArraySize, false, true))
					{
						return false;
					}
				}
			}

			// Render-Target Views
			for (uint32_t i = 0; i < m_ArraySize; i++)
			{
				if (IsRenderTarget())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView_RenderTarget[i], this, i, 1))
					{
						return false;
					}
				}

				if (IsDepthStencil())
				{
					if (!VulkanUtility::Image::View::CreateImageView(m_Resource, m_ResourceView_DepthStencil[i], this, i, 1, true))
					{
						return false;
					}
				}
			}

			// Name the image and image views.
			SetDebugName(this);
		}

		return true;
	}
}