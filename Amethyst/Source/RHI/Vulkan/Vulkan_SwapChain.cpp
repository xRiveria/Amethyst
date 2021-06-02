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
		std::array<std::shared_ptr<RHI_Semaphore>, g_RHI_MaxRenderTargetCount>& imageAcquiredSemaphores // 1 semaphore per swapchain image buffer.
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

		/*
			A VkSwapchainKHR is an opaque handle to a swapchain object. A swapchain object (aka Swapchain) provides the ability to present rendering results to a surface. 
			Swapchain objects are presented by VkSwapchainKHR handles.

			A swapchain is an abstraction for an array of presentable images that are associated with a surface. The presentable images are represented by VkImage 
			objects created by the platform. One image (which can be an array image for multiview/stereoscopic-3D surfaces) is displayed one at a time, but multiple 
			images can be queued for presentation. An application renders to the image, and then queues the image for presentation to the surface. 

			The presentable images of a swapchain are owned by the presentation engine. An application can acquire use of a presentable image from the presentation engine. 
			Use of a presentable image must occur only after the image is returned by vkAcquireNextImageKHR, and before it is released by vkQueuePresentKHR. The includes 
			transitioning the image layout and rendering commands.
		*/

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

		// Transition Image Layouts
		uint32_t imageCount;
		std::vector<VkImage> swapchainImages;
		{
			// Obtain the array of presentable images associated with the swapchain.
			vkGetSwapchainImagesKHR(rhi_Context->m_LogicalDevice, swapchain, &imageCount, nullptr);
			swapchainImages.resize(imageCount);
			vkGetSwapchainImagesKHR(rhi_Context->m_LogicalDevice, swapchain, &imageCount, swapchainImages.data());

			// Transition layouts to VK_IMAGE_LAYOUT_PRESENT_SRC_KHR. This flag must only be used for presenting a presentable image for display. A swapchain's image must be transitioned to this layout before calling vkQueuePresentKHR, and must be transitioned away from this layout after calling vkAcquireNextImageKHR.
			if (VkCommandBuffer commandBuffer = VulkanUtility::CommandBufferImmediate::BeginRecording(RHI_Queue_Type::RHI_Queue_Graphics))
			{
				/*
					Upon creation, all image subresources of an image are initially in the same layout, where that layout is selected by the VkImageCreateInfo::initialLayout member.
					
					The initialLayout must be either VK_IMAGE_LAYOUT_UNDEFINED (all contents of the data are considered to be undefined, and transition away from this layout is not guarenteed to preserve that data) 
					or VK_IMAGE_LAYOUT_PREINITIALIZED (image data can be preinitialized by the host, and preservation is guarenteed upon transition away from this layout). 
				    
					For either of these initial layouts, any image subresources must be transitioned to another layout before they are accessed by the device.
					See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#resources-image-layouts		
				*/

				for (VkImage& image : swapchainImages)
				{
					VulkanUtility::Image::SetLayout(reinterpret_cast<void*>(commandBuffer), reinterpret_cast<void*>(image), VK_IMAGE_ASPECT_COLOR_BIT, 1, 1, RHI_Image_Layout::Undefined, RHI_Image_Layout::Present_Source);
				}

				// End command buffer.
				if (!VulkanUtility::CommandBufferImmediate::EndRecordingAndSubmit(RHI_Queue_Type::RHI_Queue_Graphics))
				{
					return false;
				}
			}
		}

		// Create Image Views
		{
			for (uint32_t i = 0; i < imageCount; i++)
			{
				resourceTextures[i] = static_cast<void*>(swapchainImages[i]);

				// Name the image.
				VulkanUtility::Debug::SetVulkanObjectName(swapchainImages[i], std::string(std::string("Swapchain Image ") + std::to_string(i)).c_str());

				if (!VulkanUtility::Image::View::CreateImageView(static_cast<void*>(swapchainImages[i]), resourceViews[i], VK_IMAGE_VIEW_TYPE_2D, rhi_Context->m_SurfaceFormat, VK_IMAGE_ASPECT_COLOR_BIT))
				{
					return false;
				}
			}
		}

		surfaceOut = static_cast<void*>(presentationSurface);
		swapchainViewOut = static_cast<void*>(swapchain);

		// Semaphores
		for (uint32_t i = 0; i < bufferCount; i++)
		{
			imageAcquiredSemaphores[i] = std::make_shared<RHI_Semaphore>(rhi_Device, false, (std::string("Swapchain Image Acquired Semaphore ") + std::to_string(i)).c_str());
		}

		return true;
	}

	static void DestroySwapchain(
	RHI_Device* rhi_Device,
	uint8_t bufferCount,
	void*& surface, // A surface is an abstract handle to a platform window.
	void*& swapchainView,
	std::array<void*, g_RHI_MaxRenderTargetCount>& swapchainImageViews,
	std::array<std::shared_ptr<RHI_Semaphore>, g_RHI_MaxRenderTargetCount>& imageAcquiredSemaphores
	)
	{
		RHI_Context* rhi_Context = rhi_Device->RetrieveContextRHI();

		// Semaphores
		imageAcquiredSemaphores.fill(nullptr);

		// Image Views
		VulkanUtility::Image::View::DestroyImageViews(swapchainImageViews);

		// Swapchain
		if (swapchainView)
		{
			vkDestroySwapchainKHR(rhi_Context->m_LogicalDevice, static_cast<VkSwapchainKHR>(swapchainView), nullptr);
			swapchainView = nullptr;
		}

		// Surface
		if (surface)
		{
			vkDestroySurfaceKHR(rhi_Context->m_VulkanInstance, static_cast<VkSurfaceKHR>(surface), nullptr);
			surface = nullptr;
		}
	}

	RHI_SwapChain::RHI_SwapChain(
		void* windowHandle,
		const std::shared_ptr<RHI_Device>& rhi_Device,
		uint32_t width,
		uint32_t height,
		RHI_Format format		 /*= RHI_Format::RHI_Format_R8G8B8A8_Unorm*/,
		uint32_t bufferCount	 /*= 2*/,
		uint32_t flags		     /*= RHI_Present_Mode::RHI_Present_Immediate*/,
		const char* name		 /*= nullptr*/
	)
	{
		m_Name = name;

		// Validate Device
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice)
		{
			AMETHYST_ERROR("Invalid device.");
			return;
		}

		// Validate Resolution
		if (!RHI_Device::IsValidResolution(width, height))
		{
			AMETHYST_WARNING("%dx%d is an invalid resolution.", width, height);
			return;
		}

		// Validate Window Handle
		const auto hwnd = static_cast<HWND>(windowHandle);
		if (!hwnd || !IsWindow(hwnd)) // Determines whether the specified window handle identifies an existing window.
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return;
		}

		// Copy Parameters
		m_Format = format;
		m_RHI_Device = rhi_Device.get();
		m_BufferCount = bufferCount;
		m_Width = width;
		m_Height = height;
		m_WindowHandle = windowHandle;
		m_Flags = flags;

		m_Initialized = CreateSwapchain(m_RHI_Device, &m_Width, &m_Height, m_BufferCount, m_Format, m_Flags, m_WindowHandle, m_Surface, m_SwapchainView, m_Resource, m_ResourceViews, m_ImageAcquiredSemaphores);
		
		// Create Command Pool
		VulkanUtility::CommandPool::CreateCommandPool(m_CommandPool, RHI_Queue_Type::RHI_Queue_Graphics);

		// Create Command Lists.
		for (uint32_t i = 0; i < m_BufferCount; i++)
		{
			m_CommandLists.emplace_back(std::make_shared<RHI_CommandList>(i, this, rhi_Device->RetrieveContextEngine())); ///
		}

		AcquireNextImage();
	}

	RHI_SwapChain::~RHI_SwapChain()
	{
		// Wait in case any command buffer is still in use.
		m_RHI_Device->Queue_WaitAll();

		// Command Buffers.
		m_CommandLists.clear();

		// Command Pool.
		VulkanUtility::CommandPool::DestroyCommandPool(m_CommandPool);

		// Resources
		DestroySwapchain(m_RHI_Device, m_BufferCount, m_Surface, m_SwapchainView, m_ResourceViews, m_ImageAcquiredSemaphores);
	}

	bool RHI_SwapChain::Resize(const uint32_t width, const uint32_t height, const bool forceResize /*= false*/)
	{
		// Validate resolution.
		m_IsPresentationEnabled = RHI_Device::IsValidResolution(width, height);
		if (!m_IsPresentationEnabled)
		{
			// Return true as when minimizing, a resolution of 0,0 can be passed in, and this is fine.
			return true;
		}

		// Only resize if needed.
		if (!forceResize)
		{
			if (m_Width == width && m_Height == height)
			{
				return true;
			}
		}

		// Wait in case any command buffer is still in use.
		m_RHI_Device->Queue_WaitAll();

		// Save new dimensions.
		m_Width = width;
		m_Height = height;

		// Destroy previous swapchain.
		DestroySwapchain(m_RHI_Device, m_BufferCount, m_Surface, m_SwapchainView, m_ResourceViews, m_ImageAcquiredSemaphores);

		// Create the swapchain with the new dimensions.
		m_Initialized = CreateSwapchain(m_RHI_Device, &m_Width, &m_Height, m_BufferCount, m_Format, m_Flags, m_WindowHandle, m_Surface, m_SwapchainView, m_Resource, m_ResourceViews, m_ImageAcquiredSemaphores);
	
		// The pipeline state used by the pipeline will now be invalid since its referring to a destroyed swapchain view. By generating a new ID, the pipeline cache will automatically generate a new pipeline for this swapchain.
		if (m_Initialized) ///
		{
			m_ID = GenerateObjectID(); /// ?
		}

		return m_Initialized;
	}
	
	// Acquire image from our swapchain.
	bool RHI_SwapChain::AcquireNextImage()
	{
		if (!m_IsPresentationEnabled)
		{
			return true;
		}

		// Retrieve next command index.
		uint32_t nextCommandIndex = (m_CommandListIndex + 1) % m_BufferCount;

		// Retrieve signal semaphore.
		RHI_Semaphore* signalSemaphore = m_ImageAcquiredSemaphores[nextCommandIndex].get();

		// Validate semaphore state.
		AMETHYST_ASSERT(signalSemaphore->RetrieveState() == RHI_Semaphore_State::Idle); // Our semaphore should be in idle state.

		/*
			An application can acquire use of a presentable image using vkAcquireNextImageKHR. After acquiring a presentable image and before modifying it, 
			the application must use a synchronization primitive to ensure that the presentatioin engine has finished reading from the image (our signal semaphore). 
			The application can then transition the image's layout, queue rendering commands to it, etc. Finally, the application presents the image with vkQueuePresentKHR, 
			which releases the acquisition of the image.
		*/

		//Acquire an avaliable presentable image to use, and retrieve the index of that image. We must call vkAcquireNextImageKHR before using the image. 
		VkResult result = vkAcquireNextImageKHR(
			m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice,				// Device
			static_cast<VkSwapchainKHR>(m_SwapchainView),						// Non-retired swapchain from which an image is being acquired. 
			(std::numeric_limits<uint64_t>::max)(),								// Specifies how long the function waits in nanosconds, if no image is avaliable.
			static_cast<VkSemaphore>(signalSemaphore->RetrieveResource()),		// VK_NULL_HANDLE, or the semaphore to signal that an image has been acquired. This must be unsignalled.
			nullptr,															// VK_NULL_HANDLE or a fence to signal. This must be unsignalled and not associated with any other queue command that has not yet completed execution on that queue.
			&m_ImageIndex														// A pointer to a uint32_t in which the index of the next image to use (index into the array of images returned with VkGetSwapchainImagesKHR) is returned.
		);

		// Recreate swapchain with different size (if needed).
		/*
			- VK_ERROR_OUT_OF_DATE_KHR: A surface (window) has changed in way that it is no longer compatible with the swapchain, and further presentation requests using the swapchain will fail.	
			Applications must query the new surface properties and recreate their swapchain if they wish to continue presenting to the surface.

			- VK_SUBOPTIMAL_KHR: A swapchain no longer matches the surface properties exactly, but can still be used to present to the surface successfully.
		*/

		if ((result == VK_ERROR_OUT_OF_DATE_KHR) || (result == VK_SUBOPTIMAL_KHR)) 
		{
			AMETHYST_INFO("Outdated swapchain. Recreating...");

			if (!Resize(m_Width, m_Height, true)) // Fix our swapchain.
			{
				AMETHYST_ERROR("Failed to resize swapchain.");
				return false;
			}

			return AcquireNextImage(); // Once done, we will recall this function again.
		}

		// Check result.
		if (!VulkanUtility::Error::CheckResult(result))
		{
			AMETHYST_ERROR("Failed to acquire next image.");
			return false;
		}

		// Save command index.
		m_CommandListIndex = nextCommandIndex; // Current command index.

		// Update semaphore state.
		signalSemaphore->SetState(RHI_Semaphore_State::Signaled); // Update new semaphore state.

		return true;
	}

	// Present only when signalled.
	bool RHI_SwapChain::Present(RHI_Semaphore* waitSemaphore)
	{
		// Validate swapchain state.
		AMETHYST_ASSERT(m_IsPresentationEnabled);

		// Validate semaphore state.
		AMETHYST_ASSERT(waitSemaphore->RetrieveState() == RHI_Semaphore_State::Signaled);

		// Acquire next image
		if (!AcquireNextImage())
		{
			AMETHYST_ERROR("Failed to acquire next image.");
			return false;
		}

		// Present
		if (!m_RHI_Device->Queue_Present(m_SwapchainView, &m_ImageIndex, waitSemaphore)) 
		{
			AMETHYST_ERROR("Failed to present.");
			return false;
		}

		return true;
	}
}