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

	namespace Error
	{
		inline const char* ToString(const VkResult result)
		{
			switch (result)
			{
				case VK_SUCCESS:                                            return "VK_SUCCESS";
				case VK_NOT_READY:                                          return "VK_NOT_READY";
				case VK_TIMEOUT:                                            return "VK_TIMEOUT";
				case VK_EVENT_SET:                                          return "VK_EVENT_SET";
				case VK_EVENT_RESET:                                        return "VK_EVENT_RESET";
				case VK_INCOMPLETE:                                         return "VK_INCOMPLETE";
				case VK_ERROR_OUT_OF_HOST_MEMORY:                           return "VK_ERROR_OUT_OF_HOST_MEMORY";
				case VK_ERROR_OUT_OF_DEVICE_MEMORY:                         return "VK_ERROR_OUT_OF_DEVICE_MEMORY";
				case VK_ERROR_INITIALIZATION_FAILED:                        return "VK_ERROR_INITIALIZATION_FAILED";
				case VK_ERROR_DEVICE_LOST:                                  return "VK_ERROR_DEVICE_LOST";
				case VK_ERROR_MEMORY_MAP_FAILED:                            return "VK_ERROR_MEMORY_MAP_FAILED";
				case VK_ERROR_LAYER_NOT_PRESENT:                            return "VK_ERROR_LAYER_NOT_PRESENT";
				case VK_ERROR_EXTENSION_NOT_PRESENT:                        return "VK_ERROR_EXTENSION_NOT_PRESENT";
				case VK_ERROR_FEATURE_NOT_PRESENT:                          return "VK_ERROR_FEATURE_NOT_PRESENT";
				case VK_ERROR_INCOMPATIBLE_DRIVER:                          return "VK_ERROR_INCOMPATIBLE_DRIVER";
				case VK_ERROR_TOO_MANY_OBJECTS:                             return "VK_ERROR_TOO_MANY_OBJECTS";
				case VK_ERROR_FORMAT_NOT_SUPPORTED:                         return "VK_ERROR_FORMAT_NOT_SUPPORTED";
				case VK_ERROR_FRAGMENTED_POOL:                              return "VK_ERROR_FRAGMENTED_POOL";
				case VK_ERROR_OUT_OF_POOL_MEMORY:                           return "VK_ERROR_OUT_OF_POOL_MEMORY";
				case VK_ERROR_INVALID_EXTERNAL_HANDLE:                      return "VK_ERROR_INVALID_EXTERNAL_HANDLE";
				case VK_ERROR_SURFACE_LOST_KHR:                             return "VK_ERROR_SURFACE_LOST_KHR";
				case VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:                     return "VK_ERROR_NATIVE_WINDOW_IN_USE_KHR";
				case VK_SUBOPTIMAL_KHR:                                     return "VK_SUBOPTIMAL_KHR";
				case VK_ERROR_OUT_OF_DATE_KHR:                              return "VK_ERROR_OUT_OF_DATE_KHR";
				case VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:                     return "VK_ERROR_INCOMPATIBLE_DISPLAY_KHR";
				case VK_ERROR_VALIDATION_FAILED_EXT:                        return "VK_ERROR_VALIDATION_FAILED_EXT";
				case VK_ERROR_INVALID_SHADER_NV:                            return "VK_ERROR_INVALID_SHADER_NV";
				case VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT: return "VK_ERROR_INVALID_DRM_FORMAT_MODIFIER_PLANE_LAYOUT_EXT";
				case VK_ERROR_FRAGMENTATION_EXT:                            return "VK_ERROR_FRAGMENTATION_EXT";
				case VK_ERROR_NOT_PERMITTED_EXT:                            return "VK_ERROR_NOT_PERMITTED_EXT";
				case VK_ERROR_INVALID_DEVICE_ADDRESS_EXT:                   return "VK_ERROR_INVALID_DEVICE_ADDRESS_EXT";
				case VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:          return "VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT";
				case VK_ERROR_UNKNOWN:                                      return "VK_ERROR_UNKNOWN";
				case VK_RESULT_MAX_ENUM:                                    return "VK_RESULT_MAX_ENUM";
			}

			return "Unknown Error Code.";
		}

		inline bool CheckResult(VkResult result)
		{
			if (result == VK_SUCCESS)
			{
				return true;
			}
		}

		inline void _AssertResult(VkResult result)
		{
			AMETHYST_ASSERT(result == VK_SUCCESS);
		}
	}

	namespace Layer
	{
		inline bool IsInstanceLayerPresent(const char* layerName)
		{
			uint32_t layerCount;
			vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

			std::vector<VkLayerProperties> layers(layerCount);
			vkEnumerateInstanceLayerProperties(&layerCount, layers.data());

			for (const VkLayerProperties& layer : layers)
			{
				if (strcmp(layerName, layer.layerName) == 0)
				{
					return true;
				}
			}

			return false;
		}

		inline std::vector<const char*> RetrieveSupportedInstanceLayers(const std::vector<const char*>& layers)
		{
			std::vector<const char*> layersSupported;

			for (const char* layer : layers)
			{
				if (IsInstanceLayerPresent(layer))
				{
					layersSupported.emplace_back(layer);
				}
				else
				{
					AMETHYST_ERROR("Instance layer \"%s\" is not supported.", layer);
				}
			}

			return layersSupported;
		}
	}

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

	//Extension functions are not loaded automatically by default. Thus, in order to load them, we have to look up their addresses ourselves.
	class ExtensionFunctions
	{
	public:
		ExtensionFunctions() = default;
		~ExtensionFunctions() = default;

		static void InitializeExtensionFunctions()
		{
			#define RetrieveFunction(variable, definition) \
			variable = reinterpret_cast<PFN_##definition>(vkGetInstanceProcAddr(static_cast<VkInstance>(VulkanUtility::Globals::g_RHI_Context->m_VulkanInstance), #definition)); \
			if (!variable) AMETHYST_ERROR("Failed to retrieve Function Pointer for %s.", #definition); \
			
			RetrieveFunction(RetrievePhysicalDeviceMemoryProperties_2, vkGetPhysicalDeviceMemoryProperties2);

			if (VulkanUtility::Globals::g_RHI_Context->m_IsDebuggingEnabled)
			{
				// VK_EXT_debug_utils
				RetrieveFunction(CreateMessenger, vkCreateDebugUtilsMessengerEXT);
				RetrieveFunction(DestroyMessenger, vkDestroyDebugUtilsMessengerEXT);
				RetrieveFunction(MarkerBegin, vkCmdBeginDebugUtilsLabelEXT);
				RetrieveFunction(MarkerEnd, vkCmdEndDebugUtilsLabelEXT);

				// VK_EXT_debug_marker
				RetrieveFunction(SetObjectTag, vkSetDebugUtilsObjectTagEXT);
				RetrieveFunction(SetObjectName, vkSetDebugUtilsObjectNameEXT);
			}
		}

		//Put descriptions of each.
		static PFN_vkCreateDebugUtilsMessengerEXT CreateMessenger;
		static PFN_vkDestroyDebugUtilsMessengerEXT DestroyMessenger;
		static PFN_vkSetDebugUtilsObjectTagEXT SetObjectTag;
		static PFN_vkSetDebugUtilsObjectNameEXT SetObjectName;
		static PFN_vkCmdBeginDebugUtilsLabelEXT MarkerBegin;
		static PFN_vkCmdEndDebugUtilsLabelEXT MarkerEnd;
		static PFN_vkGetPhysicalDeviceMemoryProperties2KHR RetrievePhysicalDeviceMemoryProperties_2;

		static VkDebugUtilsMessengerEXT Messenger;
	};

	class Debug
	{
	public:
		Debug() = default;
		~Debug() = default;

		static VKAPI_ATTR VkBool32 VKAPI_CALL MessengerCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity, VkDebugUtilsMessageTypeFlagsEXT messageType, const VkDebugUtilsMessengerCallbackDataEXT* p_CallbackData, void* p_UserData)
		{
			std::string message = "Vulkan: " + std::string(p_CallbackData->pMessage);

			if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT)
			{
				Log::WriteLog(message.c_str(), LogType::Info);
			}
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT)
			{
				Log::WriteLog(message.c_str(), LogType::Info);
			}
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT)
			{
				Log::WriteLog(message.c_str(), LogType::Warning);
			}
			else if (messageSeverity & VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT)
			{
				Log::WriteLog(message.c_str(), LogType::Error);
			}

			return VK_FALSE; //Indicates if the Vulkan call that triggered the validation layer message should be aborted. If set to true, then it is aborted with the VK_ERROR_VALIDATION_FAILED_EXT error.
		}

		static void InitializeDebugMessenger(VkInstance instance)
		{
			if (ExtensionFunctions::CreateMessenger) // If our function address exists...
			{
				VkDebugUtilsMessengerCreateInfoEXT creationInfo = {};
				creationInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
				creationInfo.messageSeverity = VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT;
				creationInfo.messageType = VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT | VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
				creationInfo.pfnUserCallback = MessengerCallback; //Our messenger callback function.
			}
		}

		static void ShutdownDebugMessenger(VkInstance instance)
		{
			if (!ExtensionFunctions::DestroyMessenger) // If our function address does not exist...
			{
				return;
			}

			ExtensionFunctions::DestroyMessenger(instance, ExtensionFunctions::Messenger, nullptr);
		}
	};
}
