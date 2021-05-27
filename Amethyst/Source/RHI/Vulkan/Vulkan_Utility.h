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

	namespace Device
	{
		inline uint32_t RetrieveQueueFamilyIndex(VkQueueFlagBits queueFlags, const std::vector<VkQueueFamilyProperties>& queueFamilyProperties, uint32_t* index)
		{
			// Dedicated Queue for Compute Operations - Attempts to find a queue family index that supports compute operations but not graphics operations.
			if (queueFlags & VK_QUEUE_COMPUTE_BIT)
			{
				for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
				{
					if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0))
					{
						*index = i;
						return true;
					}
				}
			}

			// Dedicated Queue for Transfer Operations - Attempts to find a queue family index that supports transfer operations but not graphics/compute operations.
			if (queueFlags & VK_QUEUE_TRANSFER_BIT)
			{
				for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
				{
					if ((queueFamilyProperties[i].queueFlags & queueFlags) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) == 0) && ((queueFamilyProperties[i].queueFlags & VK_QUEUE_COMPUTE_BIT) == 0))
					{
						*index = i;
						return true;
					}
				}
			}

			// For other queue types or if no seperate compute queue is present, return the first one to support the requested flag.
			for (uint32_t i = 0; i < static_cast<uint32_t>(queueFamilyProperties.size()); i++)
			{
				if (queueFamilyProperties[i].queueFlags & queueFlags)
				{
					*index = i;
					return true;
				}
			}

			return false;
		}

		inline bool RetrieveQueueFamilyIndices(const VkPhysicalDevice& physicalDevice)
		{
			uint32_t queueFamilyCount = 0;
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, nullptr);

			std::vector<VkQueueFamilyProperties> queueFamilyProperties(queueFamilyCount);
			vkGetPhysicalDeviceQueueFamilyProperties(physicalDevice, &queueFamilyCount, queueFamilyProperties.data());

			//Return false immediately if the graphics queue isn't supported - we can't render anything...
			if (!RetrieveQueueFamilyIndex(VK_QUEUE_GRAPHICS_BIT, queueFamilyProperties, &Globals::g_RHI_Context->m_Queue_GraphicsIndex))
			{
				AMETHYST_ERROR("Graphics queue is not supported by the device.");
				return false;
			}

			if (!RetrieveQueueFamilyIndex(VK_QUEUE_TRANSFER_BIT, queueFamilyProperties, &Globals::g_RHI_Context->m_Queue_TransferIndex))
			{
				AMETHYST_WARNING("Transfer queue is not supported by the device, using graphics queue instead.");
				Globals::g_RHI_Context->m_Queue_TransferIndex = Globals::g_RHI_Context->m_Queue_GraphicsIndex;
			}
			
			if (!RetrieveQueueFamilyIndex(VK_QUEUE_COMPUTE_BIT, queueFamilyProperties, &Globals::g_RHI_Context->m_Queue_ComputeIndex))
			{
				AMETHYST_WARNING("Compute queue is not supported by the device, using graphics queue instead.");
				Globals::g_RHI_Context->m_Queue_ComputeIndex = Globals::g_RHI_Context->m_Queue_GraphicsIndex;
			}

			return true;
		}

		inline bool SelectPhysicalDevice(void* windleHandle)
		{
			// Register all Physical Devices
			uint32_t deviceCount = 0;
			if (!Error::CheckResult(vkEnumeratePhysicalDevices(Globals::g_RHI_Context->m_VulkanInstance, &deviceCount, nullptr)))
			{
				return false;
			}

			if (deviceCount == 0)
			{
				AMETHYST_ERROR("There are no avaliable devices.");
				return false;
			}

			std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
			if (!Error::CheckResult(vkEnumeratePhysicalDevices(Globals::g_RHI_Context->m_VulkanInstance, &deviceCount, physicalDevices.data())))
			{
				return false;
			}

			// Go through all the devices.
			for (const VkPhysicalDevice& physicalDevice : physicalDevices)
			{
				// Retrieve device properties.
				VkPhysicalDeviceProperties deviceProperties = {};
				vkGetPhysicalDeviceProperties(physicalDevice, &deviceProperties);

				VkPhysicalDeviceMemoryProperties deviceMemoryProperties = {};
				vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);

				RHI_PhysicalDevice_Type deviceType = RHI_PhysicalDevice_Unknown;
				if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU) deviceType == RHI_PhysicalDevice_Type::RHI_PhysicalDevice_Integrated;
				if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU)   deviceType == RHI_PhysicalDevice_Type::RHI_PhysicalDevice_Discrete;
				if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_VIRTUAL_GPU)	   deviceType == RHI_PhysicalDevice_Type::RHI_PhysicalDevice_Virtual;
				if (deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_CPU)			   deviceType == RHI_PhysicalDevice_Type::RHI_PhysicalDevice_CPU;

				// Let the engine know about it as it will sort all the devices from best to worst.
				VulkanUtility::Globals::g_RHI_Device->RegisterPhysicalDevice(PhysicalDevice
				(
					deviceProperties.apiVersion,											// API Verison
					deviceProperties.driverVersion,											// Driver Version
					deviceProperties.vendorID,												// Vendor ID	
					deviceType,																// Device Type
					&deviceProperties.deviceName[0],										// Device Name
					static_cast<uint64_t>(deviceMemoryProperties.memoryHeaps[0].size),		// Memory Size
					static_cast<void*>(physicalDevice)										// Our Device Data
				));
			}

			//Go through all the devices (sorted from best to worse based on their properties).
			for (uint32_t deviceIndex = 0; deviceIndex < Globals::g_RHI_Device->RetrievePhysicalDevices().size(); deviceIndex++)
			{
				const PhysicalDevice& physicalDevice_Engine = Globals::g_RHI_Device->RetrievePhysicalDevices()[deviceIndex];
				VkPhysicalDevice physicalDevice_Vulkan = static_cast<VkPhysicalDevice>(physicalDevice_Engine.RetrieveData());

				//Retrieve the first device that has a graphics, a compute and a transfer queue.
				if (RetrieveQueueFamilyIndices(physicalDevice_Vulkan))
				{
					Globals::g_RHI_Device->SetPrimaryPhysicalDevice(deviceIndex);
					Globals::g_RHI_Context->m_PhysicalDevice = physicalDevice_Vulkan;
					break;
				}
			}

			return true;
		}
	}

	// Extension functions are not loaded automatically by default. Thus, in order to load them, we have to look up their addresses ourselves.
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

		/*
			Any parameter that is a structure containing a void* pNext member must have a value of pNext that is either NULL or points to a valid structure that is defined by an enabled extension. 
			In short, the pNext member is for extensions. Each extension data structure will not only have its own internal sType field, but it will no doubt also have 
			its own pNext field. Thus, multiple extensions can extend the same data structures.
		*/

		static PFN_vkCreateDebugUtilsMessengerEXT CreateMessenger; // Creates a debug messenger. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkCreateDebugUtilsMessengerEXT.html
		static PFN_vkDestroyDebugUtilsMessengerEXT DestroyMessenger; // Destroys a debug messenger object. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkDestroyDebugUtilsMessengerEXT.html
		static PFN_vkSetDebugUtilsObjectTagEXT SetObjectTag; // Attaches arbitrary data to an object. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkSetDebugUtilsObjectTagEXT.html
		static PFN_vkSetDebugUtilsObjectNameEXT SetObjectName; // Gives a user-friendly name to an object. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/vkSetDebugUtilsObjectNameEXT.html
		static PFN_vkCmdBeginDebugUtilsLabelEXT MarkerBegin; // Open a command buffer debug label region. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkDebugUtilsLabelEXT.html
		static PFN_vkCmdEndDebugUtilsLabelEXT MarkerEnd; // Ends a command buffer debug label region. 
		static PFN_vkGetPhysicalDeviceMemoryProperties2KHR RetrievePhysicalDeviceMemoryProperties_2; // Reports memory information for the specified physical device. This behaves simiarly to vkGetPhysicalDeviceProperties, with the ability to return extended information in a pNext chain of output structs.

		static VkDebugUtilsMessengerEXT Messenger; // Our messenger object. 
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

				ExtensionFunctions::CreateMessenger(instance, &creationInfo, nullptr, &ExtensionFunctions::Messenger);
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

		static void SetObjectName(uint64_t object, VkObjectType objectType, const char* name)
		{
			if (!ExtensionFunctions::SetObjectName)
			{
				return;
			}

			VkDebugUtilsObjectNameInfoEXT nameInfo = {};
			nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
			nameInfo.pNext = nullptr;
			nameInfo.objectType = objectType;
			nameInfo.objectHandle = object; // The object to be named.
			nameInfo.pObjectName = name;

			ExtensionFunctions::SetObjectName(Globals::g_RHI_Context->m_LogicalDevice, &nameInfo);
		}

		static void SetVulkanObjectName(VkSemaphore semaphore, const char* name)
		{
			SetObjectName((uint64_t)semaphore, VK_OBJECT_TYPE_SEMAPHORE, name); ///
		}

		static void SetVulkanObjectName(VkFence fence, const char* name)
		{
			SetObjectName((uint64_t)fence, VK_OBJECT_TYPE_FENCE, name);
		}

		static void SetVulkanObjectName(VkDescriptorSet descriptorSet, const char* name)
		{
			SetObjectName((uint64_t)descriptorSet, VK_OBJECT_TYPE_DESCRIPTOR_SET, name);
		}

		static void SetVulkanObjectName(VkDescriptorSetLayout descriptorSetLayout, const char* name)
		{
			SetObjectName((uint64_t)descriptorSetLayout, VK_OBJECT_TYPE_DESCRIPTOR_SET_LAYOUT, name);
		}

		static void SetVulkanObjectName(VkPipelineLayout pipelineLayout, const char* name)
		{
			SetObjectName((uint64_t)pipelineLayout, VK_OBJECT_TYPE_PIPELINE_LAYOUT, name);
		}
	};

	/*
		- VK_DESCRIPTOR_TYPE_STORAGE_IMAGE: A storage image is a descriptor type associated with an image resource via an image view that load, store and atomic operations can be performed on.
		- VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE: A sampled image is a descriptor type associated with an image resource via an image view that sampling operations can be performed on.
		- VK_DESCRIPTOR_TYPE_SAMPLER: A sampler descriptor is a descriptor type associated with a sampler object, used to control the behavior of sampling operations performed on a sampled image.
		- VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER: A uniform buffer is a descriptor type associated with a buffer resource directly, described in a shader as a structure with various members that load operations can be performed on.
		- VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC: A dynamic uniform buffer is almost identical to a uniform buffer, and differs only in how the offset into the buffer is specified.
		The base offset calculated by the VkDescriptorBufferInfo when initially updating the descriptor set is added to a dynamic offset when binding the descriptor set. 
	*/

	static VkDescriptorType ToVulkanDescriptorType(const RHI_Descriptor& descriptor)
	{
		if (descriptor.m_DescriptorType == RHI_Descriptor_Type::ConstantBuffer)
		{
			return descriptor.m_IsDynamicConstantBuffer ? VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC : VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		}

		if (descriptor.m_DescriptorType == RHI_Descriptor_Type::Texture)
		{
			return descriptor.m_IsStorage ? VK_DESCRIPTOR_TYPE_STORAGE_IMAGE : VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;
		}

		if (descriptor.m_DescriptorType == RHI_Descriptor_Type::Sampler)
		{
			return VK_DESCRIPTOR_TYPE_SAMPLER;
		}

		AMETHYST_ERROR("Invalid descriptor type.");
		return VK_DESCRIPTOR_TYPE_MAX_ENUM;
	}
}
