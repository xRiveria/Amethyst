#include "Amethyst.h"
#include "../RHI_Device.h"
#include "../RHI/RHI_Implementation.h"
#include "../RHI/RHI_Semaphore.h"
#include "../RHI/RHI_Fence.h"

namespace Amethyst
{
	RHI_Device::RHI_Device(Context* context)
	{
		m_Context = context;
		m_RHI_Context = std::make_shared<RHI_Context>();

		// Pass pointer to the widely used Utility namespace.
		VulkanUtility::Globals::g_RHI_Device = this;
		VulkanUtility::Globals::g_RHI_Context = m_RHI_Context.get();

		// Create Instance
		VkApplicationInfo applicationInfo = {};
		{
			// Deduce API Version to Use
			{
				// Retrieve SDK Version
				uint32_t sdkVersion = VK_HEADER_VERSION_COMPLETE; //Current complete version number.
				// Retrieve Driver Version
				uint32_t driverVersion = 0;
				{
					// As mentioned by LunarG, if vkEnumerateInstanceVersion is not present, we are running on Vulkan 1.0.
					// https://www.lunarg.com/wp-content/uploads/2019/02/Vulkan-1.1-Compatibility-Statement_01_19.pdf
					auto enumerateInstanceVersion = reinterpret_cast<PFN_vkEnumerateInstanceVersion>(vkGetInstanceProcAddr(nullptr, "vkEnumerateInstanceVersion"));

					if (enumerateInstanceVersion)
					{
						enumerateInstanceVersion(&driverVersion);
					}
					else
					{
						driverVersion = VK_API_VERSION_1_0; //Version 1, 0, 0
					}
				}

				// Choose the version which is supported by both the SDK and the driver.
				m_RHI_Context->m_APIVersion = Math::Utilities::Min(sdkVersion, driverVersion);

				// In case the SDK is not supported by the driver, prompt the user to update.
				if (sdkVersion > driverVersion)
				{
					//Detect and log version.
					VulkanUtility::Globals::g_RHI_DriverVersion = std::to_string(VK_VERSION_MAJOR(driverVersion)) + "." + std::to_string(VK_VERSION_MINOR(driverVersion)) + "." + std::to_string(VK_VERSION_PATCH(driverVersion));
					VulkanUtility::Globals::g_RHI_SDKVersion = std::to_string(VK_VERSION_MAJOR(sdkVersion)) + "." + std::to_string(VK_VERSION_MINOR(sdkVersion)) + "." + std::to_string(VK_VERSION_PATCH(sdkVersion));

					AMETHYST_WARNING("Falling back to Vulkan %s. Please update your graphics drivers to support Vulkan %s.", VulkanUtility::Globals::g_RHI_DriverVersion.c_str(), VulkanUtility::Globals::g_RHI_SDKVersion.c_str());
				}
			}

			applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
			applicationInfo.pApplicationName = ENGINE_VERSION;
			applicationInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.pEngineName = ENGINE_VERSION;
			applicationInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
			applicationInfo.apiVersion = m_RHI_Context->m_APIVersion;

			//Retrieve the supported extensions out of the requested extensions.
			std::vector<const char*> supportedExtensions = VulkanUtility::Extensions::RetrieveSupportedInstanceExtensions(m_RHI_Context->m_InstanceExtensions);
		}
	}

	RHI_Device::~RHI_Device()
	{

	}

	bool RHI_Device::Queue_Present(void* swapchainView, uint32_t* imageIndex, RHI_Semaphore* waitSemaphore /*= nullptr*/) const
	{

	}

	bool RHI_Device::Queue_Submit(const RHI_Queue_Type queueType, const uint32_t waitFlags, void* commandBuffer, RHI_Semaphore* waitSemaphore /*= nullptr*/, RHI_Semaphore* signalSemaphore /*= nullptr*/, RHI_Fence* signalFence /*= nullptr*/) const
	{

	}

	bool RHI_Device::Queue_Wait(const RHI_Queue_Type queueType) const
	{

	}
}