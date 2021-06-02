#pragma once
#include <memory>
#include <vector>
#include <array>
#include "RHI_Utilities.h"
#include "../Core/AmethystObject.h"

namespace Amethyst
{
	class Vector4;

	class RHI_SwapChain : public AmethystObject
	{
	public:
		RHI_SwapChain(
			void* windowHandle,
			const std::shared_ptr<RHI_Device>& rhi_Device,
			uint32_t width,
			uint32_t height,
			RHI_Format format		= RHI_Format::RHI_Format_R8G8B8A8_Unorm,
			uint32_t bufferCount	= 2,
			uint32_t flags	    = RHI_Present_Mode::RHI_Present_Immediate,
			const char* name	    = nullptr
		);
		~RHI_SwapChain();

		bool Resize(uint32_t width, uint32_t height, const bool forceResize = false);
		bool Present(RHI_Semaphore* waitSemaphore);

		//Misc
		uint32_t RetrieveWidth() const { return m_Width; }
		uint32_t RetrieveHeight() const { return m_Height; }
		uint32_t RetrieveBufferCount() const { return m_BufferCount; }
		uint32_t RetrieveFlags() const { return m_Flags; }
		uint32_t RetrieveCommandListIndex() const { return m_CommandListIndex; };
		uint32_t RetrieveImageIndex() const { return m_ImageIndex; }
		bool IsInitialized() const { return m_Initialized; }
		bool IsPresentationEnabled() const { return m_IsPresentationEnabled; }
		RHI_CommandList* RetrieveCommandList() { return m_CommandListIndex < static_cast<uint32_t>(m_CommandLists.size()) ? m_CommandLists[m_CommandListIndex].get() : nullptr; }
		RHI_Semaphore* RetrieveImageAcquiredSemaphore() { return m_ImageAcquiredSemaphores[m_CommandListIndex].get(); }

		//GPU Resources
		void* RetrieveResource(uint32_t i = 0) const { return m_Resource[i]; }
		void* RetrieveResourceView(uint32_t i = 0) const { return m_ResourceViews[i]; }
		void* RetrieveResourceViewRenderTarget() const { return m_ResourceViewRenderTarget; }
		void*& RetrieveCommandPool() { return m_CommandPool; }

	private:
		bool AcquireNextImage();

	private:
		bool m_Initialized = false;
		bool m_Windowed = false;
		uint32_t m_BufferCount = 0; // Amount of image buffers for this swapchain.
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_Flags = 0;
		RHI_Format m_Format = RHI_Format::RHI_Format_R8G8B8A8_Unorm; //A 4-component, 32-bit unsigned-normalized-integer format that supports 8 bits per channel including Alpha.

		//API
		void* m_SwapchainView = nullptr;			///
		void* m_ResourceViewRenderTarget = nullptr; ///
		void* m_Surface = nullptr;
		void* m_WindowHandle = nullptr;
		void* m_CommandPool = nullptr;
		bool m_IsPresentationEnabled = true;

		uint32_t m_CommandListIndex = (std::numeric_limits<uint32_t>::max)(); //Index of the current command list in m_CommandLists.
		uint32_t m_ImageIndex = 0; ///
		RHI_Device* m_RHI_Device = nullptr;
		std::vector<std::shared_ptr<RHI_CommandList>> m_CommandLists; ///
		std::array<std::shared_ptr<RHI_Semaphore>, g_RHI_MaxRenderTargetCount> m_ImageAcquiredSemaphores = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr };
		std::array<void*, g_RHI_MaxRenderTargetCount> m_ResourceViews = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }; // Amount of image views for this swapchain - should correspond to buffer count.
		std::array<void*, g_RHI_MaxRenderTargetCount> m_Resource = { nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr }; // Amount of swapchain images for this swapchain.
	};
}