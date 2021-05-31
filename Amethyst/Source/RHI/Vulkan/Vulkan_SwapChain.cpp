#pragma once
#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_SwapChain.h"
#include "../RHI_Device.h"
#include "../RHI_CommandList.h"
#include "../RHI_Pipeline.h"
#include "../RHI_Semaphore.h"

namespace Amethyst
{
	static bool CreateSwapchain
	(
		RHI_Device* rhi_Device,
		uint32_t* width,
		uint32_t* height,
		uint32_t bufferCount,
		RHI_Format format,
		uint32_t presentationFlags,
		void* windowHandle,
		void*& surfaceOut,
		void*& swapchainViewOut,
		std::array<void*, g_RHI_MaxRenderTargetCount>& resourceTextures,
		std::array<void*, g_RHI_MaxRenderTargetCount>& resourceViews,
		std::array<std::shared_ptr<RHI_Semaphore>, g_RHI_MaxRenderTargetCount>& imageAcquiredSemaphore
	)
	{
		RHI_Context* rhi_Context = rhi_Device->RetrieveContextRHI();

		/*
			Native platform surface or window objects are abstracted by surface objects, represented by VkSurfaceKHR handles. As Vulkan is platform agnostic, 
			we will require a VkSurfaceKHR object to interface with the window system, establishing a connection between Vulkan and our window to present results 
			to the screen. Window surfaces are optional if you just need off-screen rendering.
		*/

		// Create Presentation Surface
		VkSurfaceKHR presentationSurface = nullptr; 
		{
			VkWin32SurfaceCreateInfoKHR surfaceCreateInfo = {};
			surfaceCreateInfo.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
			surfaceCreateInfo.hwnd = static_cast<HWND>(windowHandle); // Win32 HWND (handle to a window) for the window to associate the surface with. 
			surfaceCreateInfo.hinstance = GetModuleHandle(nullptr); // Win32 hardware instance for the window to associate the surface with.

			// Creates a VkSurfaceKHR object for a Win32 native window.
			if (!VulkanUtility::Error::CheckResult(vkCreateWin32SurfaceKHR(rhi_Device->RetrieveContextRHI()->m_VulkanInstance, &surfaceCreateInfo, nullptr, &presentationSurface)))
			{
				return false;
			}

			VkBool32 presentationSupport = false;
			// Determines whether a queue family of a physical device supports presentation to a given surface.
			if (!VulkanUtility::Error::CheckResult(vkGetPhysicalDeviceSurfaceSupportKHR(rhi_Context->m_PhysicalDevice, rhi_Context->m_Queue_GraphicsIndex, presentationSurface, &presentationSupport)))
			{
				return false; // Surface can't present...
			}

			if (!presentationSupport) 
			{
				AMETHYST_ERROR("The surface does not have presentation support.");
				return false;
			}
		}

		// Retrieve surface capacities.
		VkSurfaceCapabilitiesKHR capabilities = VulkanUtility::Surface::RetrieveSurfaceCapabilities(presentationSurface);

		// Compute Extents
		*width = Math::Utilities::Clamp(*width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
		*height = Math::Utilities::Clamp(*height, capabilities.minImageExtent.width, capabilities.maxImageExtent.height);
		VkExtent2D extent = { *width, *height }; // The size of our swapchain.

		// Detect Surface Format and Color Space
		VulkanUtility::Surface::RetrieveFormatAndColorSpace(presentationSurface, &rhi_Context->m_SurfaceFormat, &rhi_Context->m_SurfaceColorSpace);

		// Create Swap-Chain
		VkSwapchainKHR swapchain;
		{
			VkSwapchainCreateInfoKHR swapchainCreateInfo = {};
			swapchainCreateInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
			swapchainCreateInfo.surface = presentationSurface;
			swapchainCreateInfo.minImageCount = bufferCount;
			swapchainCreateInfo.imageFormat = rhi_Context->m_SurfaceFormat; // The format the swapchain images will be created with.
			swapchainCreateInfo.imageColorSpace = rhi_Context->m_SurfaceColorSpace; // Specifies the way the swapchain interprets image data. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkColorSpaceKHR.html
			swapchainCreateInfo.imageExtent = extent;
			swapchainCreateInfo.imageArrayLayers = 1; // The number of views in a multiview/stereoscopic surface. For non-stereoscopic 3D applications, this value is 1.
			swapchainCreateInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT; // The intended usage of the (acquired) swapchain images. We intend to use the images as color or resolve attachment in a VkFramebuffer.

			uint32_t queueFamilyIndices[] = { rhi_Context->m_Queue_ComputeIndex, rhi_Context->m_Queue_GraphicsIndex };
			if (rhi_Context->m_Queue_ComputeIndex != rhi_Context->m_Queue_GraphicsIndex) // If they belong to different queue families...
			{
				/* Sharing Mode: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkSharingMode.html
				
					Buffer and image objects are created with a sharing mode controlling how they can be accessed from queues. The supported sharing modes are:

					- VK_SHARING_MODE_EXCLUSIVE: Specifies that access to any range or image subresource of the object will be exclusive to a single queue family at a time. Resources created 
					with this mode enabled are not owned by any queue family initially. Instead, ownership is implicitly acquired upon first use within a queue. Once owned by a queue family, 
					ownership transfer must be performed to make the memory contents of a range or image subresource accessible to a different queue family.

					- VK_SHARING_MODE_CONCURRENT: Specifies that concurrent access to any range or image subresource of the object from multiple queue families is supported. These 
					queue families must be specified through the queueFamilyIndesCount and pQueueFamilyIndicies parameters.
				
				*/
				swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
				swapchainCreateInfo.queueFamilyIndexCount = 2; // The number of queue families having access to the image(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT.
				swapchainCreateInfo.pQueueFamilyIndices = queueFamilyIndices; // Pointer to an array of queue family indices having access to the image(s) of the swapchain when imageSharingMode is VK_SHARING_MODE_CONCURRENT.
			}
			else
			{
				swapchainCreateInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
				swapchainCreateInfo.queueFamilyIndexCount = 0; // Ignored if sharing mode is Exclusive.
				swapchainCreateInfo.pQueueFamilyIndices = nullptr; // Ignored if sharing mode is Exclusive.
			}

			/* Pre-Transform: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkSurfaceTransformFlagBitsKHR.html
			* 
				Describes the transform relative to the presentation engine's natural orientation, applied to the image content prior to presentation. 
				If it does not match the currentTransform value returned by surface capabilities, the presentation engine will transform the image content as part of 
				the presentation operation. Examples of these transformations are image rotations and mirroring.
			*/
			swapchainCreateInfo.preTransform = capabilities.currentTransform; 

			/* Alpha Compositing: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkCompositeAlphaFlagBitsKHR.html
			
				Indicates the alpha compositing mode to use when this surface is composited/blended together with other surfaces on certain window systems. In short, 
				it is how the alpha pixels of the window are treated. You will almost always want to ignore the alpha channel.
			*/
			swapchainCreateInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR; // The alpha channel, if it exists, of the images is ignored in the compositing process. The image is treated as if it has a constant alpha of 1.0f.
			swapchainCreateInfo.presentMode = VulkanUtility::Surface::SetPresentationMode(presentationSurface, presentationFlags); // Present mode determines how incoming present requests will be processed and queued internally.

			/* Clipping
			
				 Specifies whether Vulkan is allowed to discard rendering operations that affect regions of the surface that are not visible. If true, presentable 
				 images associated with the swapchain may not own all of their pixels. Pixels in the presentable images that correspond to regions of the target surface 
				 obscured by another window on the desktop, or subject to some other clipping mechanism will have undefined content when read back. 
					
				 Fragment shaders may not execute for these pixels, and thus any side effects they would have had will not occur. Setting VK_TRUE does not guarantee that 
				 any clipping will occur, but allows for more efficient presentation methods to be used on some platforms.

				 If false, presentable images associated with the swapchain will own all of the pixels they contain. 

				 Applications should set this value to VK_TRUE if they do not expect to read back the content of presentable images before presenting them or after 
				 reacquiring them, and if their fragment shaders do not have any side effects that require them to run for all pixels in the presentable image. Enable for 
				 best performance!
			*/
			swapchainCreateInfo.clipped = VK_TRUE;  

			// Is either VK_NULL_HANDLE, or the existing non-retired swapchain currently associated with the surface. Providing a valid old swapchain /may/ aid in resource usage, and also allows the application to still present any images that are already acquired from it. 
			// If this parameter is a valid swapchain, which has exclusive full screen access, that access is released from the oldSwapchain. The newly created swapchain will automatically acquire exclusive full-screen access from old swapchain.
			swapchainCreateInfo.oldSwapchain = nullptr;

			if (!VulkanUtility::Error::CheckResult(vkCreateSwapchainKHR(rhi_Context->m_LogicalDevice, &swapchainCreateInfo, nullptr, &swapchain)))
			{
				return false;
			}
		}

		// Create Image Buffers
		uint32_t imageCount;
		std::vector<VkImage> images;
		{
			// Retrieve Images
			vkGetSwapchainImagesKHR(rhi_Context->m_LogicalDevice, swapchain, &imageCount, nullptr);
			images.resize(imageCount);
			vkGetSwapchainImagesKHR(rhi_Context->m_LogicalDevice, swapchain, &imageCount, images.data());

			// Transition layouts to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR. This flag must only be used for presenting a presentable image for display. A swapchain's image must be transitioned to this layout before calling vkQueuePresentKHR, and must be transitioned away from this layout after calling vkAcquireNextImageKHR.
			if (VkCommandBuffer commandBuffer = VulkanUtility::CommandBufferImmediate::BeginRecording(RHI_Queue_Type::RHI_Queue_Graphics))
			{
				for (VkImage& image : images)
				{
					//VulkanUtility::Image::SetLayout()
				}
			}
		}

		// Create Image Views

		// Semaphores


	}
}