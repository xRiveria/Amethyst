#pragma once
#include "../RHI/RHI_Implementation.h"
#include "../RHI/RHI_Device.h"
#include "../RHI/RHI_Texture.h"
#include "../RHI/RHI_SwapChain.h"
#include "../RHI/RHI_DepthStencilState.h"
#include "../RHI/RHI_Descriptor.h"
#include "../Runtime/Log/Log.h"
#include "../Runtime/Math/Vector4.h"
//Display

namespace Amethyst::VulkanUtility
{
	struct Globals
	{
		static inline RHI_Device* g_RHI_Device;
		static inline RHI_Context* g_RHI_Context;
		static inline std::string g_RHI_DriverVersion;
		static inline std::string g_RHI_SDKVersion;
	};

	namespace Extensions
	{
		inline bool IsDeviceExtensionPresent(const char* extensionName, VkPhysicalDevice physicalDevice)
		{
			uint32_t extensionCount = 0;
			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateDeviceExtensionProperties(physicalDevice, nullptr, &extensionCount, extensions.data());

			for (const VkExtensionProperties& extension : extensions)
			{
				if (strcmp(extensionName, extension.extensionName) == 0)
				{
					return true;
				}
			}

			return false;
		}

		inline std::vector<const char*> RetrieveSupportedDeviceExtensions(const std::vector<const char*>& extensions, VkPhysicalDevice physicalDevice)
		{
			std::vector<const char*> extensionsSupported;

			for (const char* extension : extensions)
			{
				if (IsDeviceExtensionPresent(extension, physicalDevice))
				{
					extensionsSupported.emplace_back(extension);
				}
				else
				{
					AMETHYST_ERROR("Device extension \"%s\" is not supported.", extension);
				}
			}

			return extensionsSupported;
		}

		inline bool IsInstanceExtensionPresent(const char* extensionName)
		{
			uint32_t extensionCount = 0;
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, nullptr);

			std::vector<VkExtensionProperties> extensions(extensionCount);
			vkEnumerateInstanceExtensionProperties(nullptr, &extensionCount, extensions.data());

			for (const VkExtensionProperties extension : extensions)
			{
				if (strcmp(extensionName, extension.extensionName) == 0)
				{
					return true;
				}
			}

			return false;
		}

		inline std::vector<const char*> RetrieveSupportedInstanceExtensions(const std::vector<const char*>& extensions)
		{
			std::vector<const char*> extensionsSupported;

			for (const char* extension : extensions)
			{
				if (IsInstanceExtensionPresent(extension))
				{
					extensionsSupported.emplace_back(extension);
				}
				else
				{
					AMETHYST_ERROR("Instance extension \"%s\" is not supported.", extension);
				}
			}

			return extensionsSupported;
		}
	}
}
