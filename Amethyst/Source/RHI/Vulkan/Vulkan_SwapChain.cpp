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
		uint32_t flags,
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

	}
}