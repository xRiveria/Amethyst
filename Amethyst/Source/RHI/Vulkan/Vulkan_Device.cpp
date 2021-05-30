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
					// Detect and log version.
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

			// Retrieve the supported extensions out of the requested extensions.
			std::vector<const char*> supportedExtensions = VulkanUtility::Extensions::RetrieveSupportedInstanceExtensions(m_RHI_Context->m_InstanceExtensions);

			VkInstanceCreateInfo creationInfo = {};
			creationInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
			creationInfo.pApplicationInfo = &applicationInfo;
			creationInfo.enabledExtensionCount = static_cast<uint32_t>(supportedExtensions.size());
			creationInfo.ppEnabledExtensionNames = supportedExtensions.data();
			creationInfo.enabledLayerCount = 0; //Deprecated. Instance and device layers are no longer seperate. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#extendingvulkan-layers-devicelayerdeprecation

			// Validation Features - See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkValidationFeatureEnableEXT.html
			VkValidationFeaturesEXT validationFeatures = {};
			validationFeatures.sType = VK_STRUCTURE_TYPE_VALIDATION_FEATURES_EXT;
			validationFeatures.enabledValidationFeatureCount = static_cast<uint32_t>(m_RHI_Context->m_ValidationExtensions.size());
			validationFeatures.pEnabledValidationFeatures = m_RHI_Context->m_ValidationExtensions.data();

			if (m_RHI_Context->m_IsDebuggingEnabled)
			{
				// Enable Validation Layer.
				if (VulkanUtility::Layer::IsInstanceLayerPresent(m_RHI_Context->m_ValidationLayers.front())) // VK_LAYER_KHRONOS_validation
				{
					// Validation Layers
					creationInfo.enabledLayerCount = static_cast<uint32_t>(m_RHI_Context->m_ValidationLayers.size());
					creationInfo.ppEnabledLayerNames = m_RHI_Context->m_ValidationLayers.data();
					creationInfo.pNext = &validationFeatures; //Allows us to debug issues with instance creation/deletion. 
				}
				else
				{
					AMETHYST_ERROR("Validation layer \"%s\" was requested, but not avaliable.", m_RHI_Context->m_ValidationLayers.front());
				}
			}

			if (!VulkanUtility::Error::CheckResult(vkCreateInstance(&creationInfo, nullptr, &m_RHI_Context->m_VulkanInstance)));
			{
				return;
			}
		}

		// Retrieve Function Pointers (From Extensions)
		VulkanUtility::ExtensionFunctions::InitializeExtensionFunctions();

		// Debug
		if (m_RHI_Context->m_IsDebuggingEnabled)
		{
			VulkanUtility::Debug::InitializeDebugMessenger(m_RHI_Context->m_VulkanInstance);
		}

		// Find a Physical Device.
		if (!VulkanUtility::Device::SelectPhysicalDevice(nullptr)) /// To patch.
		{
			AMETHYST_ERROR("Failed to find a suitable physical device.");
			return;
		}

		// Device
		std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
		{
			// Queue Creation Info
			std::vector<uint32_t> uniqueQueueFamilies =
			{
				m_RHI_Context->m_Queue_GraphicsIndex,
				m_RHI_Context->m_Queue_TransferIndex,
				m_RHI_Context->m_Queue_ComputeIndex
			};

			float queuePriority = 1.0f;

			for (const uint32_t& queueFamilyIndex : uniqueQueueFamilies)
			{
				VkDeviceQueueCreateInfo queueCreateInfo = {};
				queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
				queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
				queueCreateInfo.queueCount = 1;
				queueCreateInfo.pQueuePriorities = &queuePriority; //Allows us to assign priorities to queues to influence the scheduling of command buffer execution. Required even if there is only a single queue.

				queueCreateInfos.push_back(queueCreateInfo);
			}
		}

		// Retrieve Device Properties
		vkGetPhysicalDeviceProperties(static_cast<VkPhysicalDevice>(m_RHI_Context->m_PhysicalDevice), &m_RHI_Context->m_PhysicalDeviceProperties);

		// Resource Limits
		RHI_Context::m_Texture2D_DimensionsMax = m_RHI_Context->m_PhysicalDeviceProperties.limits.maxImageDimension2D;

		// Disable profiler if timestamps are not supported.
		//
		//
		//

		// Retrieve Device Features
		VkPhysicalDeviceVulkan12Features deviceFeatures_1_2_Enabled = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
		VkPhysicalDeviceFeatures2 deviceFeaturesEnabled = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &deviceFeatures_1_2_Enabled };
		{
			// A macro to make enabling features a little easier.
			#define ENABLE_FEATURE(deviceFeatures, enabledFeatures, feature)											\
			if (deviceFeatures.feature) /* If the feature is part of the avaliable features list for  1.2... */			\
			{																											\
				enabledFeatures.feature = VK_TRUE;	/* We set the feature in our enabled features list to true.	*/		\
			}																											\
			else																										\
			{																											\
				AMETHYST_WARNING("Requested device feature "#feature" is not supported by the physical device.");		\
			} 

			// Retrieve
			vkGetPhysicalDeviceFeatures2(m_RHI_Context->m_PhysicalDevice, &m_RHI_Context->m_PhysicalDeviceFeatures);

			// Enable
			ENABLE_FEATURE(m_RHI_Context->m_PhysicalDeviceFeatures.features, deviceFeaturesEnabled.features, samplerAnisotropy) // Enables Anisotropic Filtering.
			ENABLE_FEATURE(m_RHI_Context->m_PhysicalDeviceFeatures.features, deviceFeaturesEnabled.features, fillModeNonSolid)  // Enables Point/Wireframe Fill Modes.
			ENABLE_FEATURE(m_RHI_Context->m_PhysicalDeviceFeatures.features, deviceFeaturesEnabled.features, wideLines)			// Enables lines with width other than 1.0.
			ENABLE_FEATURE(m_RHI_Context->m_PhysicalDeviceFeatures.features, deviceFeaturesEnabled.features, imageCubeArray);	// Enables VkImageViewType of VK_IMAGE_VIEW_TYPE_CUBE_ARRAY to be created.
			ENABLE_FEATURE(m_RHI_Context->m_PhysicalDeviceFeatures1_2, deviceFeatures_1_2_Enabled, timelineSemaphore)			// Enables Timeline Semaphores.
		}

		// Determine enabled graphics shader stages.
		m_EnabledGraphicsShaderStages = VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		if (deviceFeaturesEnabled.features.geometryShader)
		{
			m_EnabledGraphicsShaderStages |= VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT;
		}
		if (deviceFeaturesEnabled.features.tessellationShader)
		{
			m_EnabledGraphicsShaderStages |= VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT | VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT;
		}

		// Retrieve the supported extensions out of the requested extensions.
		std::vector<const char*> extensionsSupported = VulkanUtility::Extensions::RetrieveSupportedDeviceExtensions(m_RHI_Context->m_DeviceExtensions, m_RHI_Context->m_PhysicalDevice);

		// Device Creation Info
		VkDeviceCreateInfo creationInfo = {};
		{
			creationInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
			creationInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
			creationInfo.pQueueCreateInfos = queueCreateInfos.data();
			creationInfo.pNext = &deviceFeaturesEnabled;
			creationInfo.enabledExtensionCount = static_cast<uint32_t>(extensionsSupported.size());
			creationInfo.ppEnabledExtensionNames = extensionsSupported.data();

			if (m_RHI_Context->m_IsDebuggingEnabled)
			{
				creationInfo.enabledLayerCount = static_cast<uint32_t>(m_RHI_Context->m_ValidationLayers.size());
				creationInfo.ppEnabledExtensionNames = m_RHI_Context->m_ValidationLayers.data();
			}
			else
			{
				creationInfo.enabledLayerCount = 0;
			}
		}

		// Creation
		if (!VulkanUtility::Error::CheckResult(vkCreateDevice(m_RHI_Context->m_PhysicalDevice, &creationInfo, nullptr, &m_RHI_Context->m_LogicalDevice)))
		{
			return;
		}

		// Retrieve queues that are created along with the logical device and store them in the handles we have for future interfacing.
		vkGetDeviceQueue(m_RHI_Context->m_LogicalDevice, m_RHI_Context->m_Queue_GraphicsIndex, 0, reinterpret_cast<VkQueue*>(&m_RHI_Context->m_Queue_Graphics));
		vkGetDeviceQueue(m_RHI_Context->m_LogicalDevice, m_RHI_Context->m_Queue_TransferIndex, 0, reinterpret_cast<VkQueue*>(&m_RHI_Context->m_Queue_Transfer));
		vkGetDeviceQueue(m_RHI_Context->m_LogicalDevice, m_RHI_Context->m_Queue_ComputeIndex, 0, reinterpret_cast<VkQueue*>(&m_RHI_Context->m_Queue_Compute));
	}


	RHI_Device::~RHI_Device()
	{
		if (!m_RHI_Context || !m_RHI_Context->m_Queue_Graphics)
		{
			return;
		}

		// Release Resources
		if (Queue_WaitAll()) // Finish all ongoing operations.
		{
			m_RHI_Context->DestroyAllocator();

			if (m_RHI_Context->m_IsDebuggingEnabled)
			{
				VulkanUtility::Debug::ShutdownDebugMessenger(m_RHI_Context->m_VulkanInstance);
			}

			vkDestroyDevice(m_RHI_Context->m_LogicalDevice, nullptr);
			vkDestroyInstance(m_RHI_Context->m_VulkanInstance, nullptr);
		}
	}

	//Remember that Fences are mainly used to synchronize your application itself with rendering operations, whereas semaphores are used to synchronize operations within or across command queues.
	bool RHI_Device::Queue_Present(void* swapchainView, uint32_t* imageIndex, RHI_Semaphore* waitSemaphore /*= nullptr*/) const
	{
		// Validate Semaphore State
		if (waitSemaphore) AMETHYST_ASSERT(waitSemaphore->RetrieveState() == RHI_Semaphore_State::Signaled); //Ensure that our semaphore has been signalled.

		// Retrieve Semaphore (Vulkan Resource)
		void* vkWaitSemaphore = waitSemaphore ? waitSemaphore->RetrieveResource() : nullptr;
		
		// Queue our image for presentation.
		VkPresentInfoKHR presentInfo = {};
		{
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			presentInfo.waitSemaphoreCount = waitSemaphore ? 1 : 0;
			presentInfo.pWaitSemaphores = waitSemaphore ? reinterpret_cast<VkSemaphore*>(&vkWaitSemaphore) : nullptr;
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = reinterpret_cast<VkSwapchainKHR*>(&swapchainView);
			presentInfo.pImageIndices = imageIndex;
		}
		
		std::lock_guard<std::mutex> lock(m_QueueMutex);
		if (!VulkanUtility::Error::CheckResult(vkQueuePresentKHR(static_cast<VkQueue>(m_RHI_Context->m_Queue_Graphics), &presentInfo)))
		{
			return false;
		}

		// Update Semaphore State
		if (waitSemaphore)
		{
			waitSemaphore->SetState(RHI_Semaphore_State::Idle);
		}

		return true;
	}

	bool RHI_Device::Queue_Submit(const RHI_Queue_Type queueType, const uint32_t waitFlags, void* commandBuffer, RHI_Semaphore* waitSemaphore /*= nullptr*/, RHI_Semaphore* signalSemaphore /*= nullptr*/, RHI_Fence* signalFence /*= nullptr*/) const
	{
		// Validate Input
		AMETHYST_ASSERT(commandBuffer != nullptr);

		// Validate Semaphore States
		if (waitSemaphore)	 AMETHYST_ASSERT(waitSemaphore->RetrieveState() == RHI_Semaphore_State::Signaled); //Ensure that our wait semaphore is signalled.
		if (signalSemaphore) AMETHYST_ASSERT(signalSemaphore->RetrieveState() == RHI_Semaphore_State::Idle);   //Ensure that our signal semaphore is still idle.

		// Retrieve Semaphores (Vulkan Device)
		void* vkWaitSemaphore = waitSemaphore ? waitSemaphore->RetrieveResource() : nullptr;
		void* vkSignalSemaphore = signalSemaphore ? signalSemaphore->RetrieveResource() : nullptr;

		// Submits our sequence of semaphores/command buffers to the queue.
		VkSubmitInfo submitInfo = {};
		{
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.pNext = nullptr;
			submitInfo.waitSemaphoreCount = waitSemaphore ? 1 : 0; // Semaphores to wait on before executing the command buffers for the batch.
			submitInfo.pWaitSemaphores = waitSemaphore ? reinterpret_cast<VkSemaphore*>(&waitSemaphore) : nullptr;
			submitInfo.signalSemaphoreCount = signalSemaphore ? 1 : 0; // Semaphores to signal once the command buffers in pCommandBuffers have completed execution.
			submitInfo.pSignalSemaphores = signalSemaphore ? reinterpret_cast<VkSemaphore*>(signalSemaphore) : nullptr;
			submitInfo.pWaitDstStageMask = &waitFlags; // An array of pipeline stages at which each corresponding semaphore wait will occur.
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = reinterpret_cast<VkCommandBuffer*>(&commandBuffer);
		}

		// Retrieve Signal Fence
		void* vkSignalFence = signalFence ? signalFence->RetrieveResource() : nullptr; // Fence to be signalled once all submitted command buffers have completed exeuction.

		std::lock_guard<std::mutex> lock(m_QueueMutex);
		if (!VulkanUtility::Error::CheckResult(vkQueueSubmit(static_cast<VkQueue>(Queue_Retrieve(queueType)), 1, &submitInfo, static_cast<VkFence>(vkSignalFence))))
		{
			return false;
		}

		// Update Semaphore States
		if (waitSemaphore)   waitSemaphore->SetState(RHI_Semaphore_State::Idle);		// Ensure our wait semaphore is idle.
		if (signalSemaphore) signalSemaphore->SetState(RHI_Semaphore_State::Signaled);  // Ensure our signal semaphore is now in signalled state.

		return true;
	}

	bool RHI_Device::Queue_Wait(const RHI_Queue_Type queueType) const
	{
		std::lock_guard<std::mutex> lock(m_QueueMutex);

		// Wait on the host for the completion of outstanding queue operations for the given queue. Equivalent to submitting a fence to a queue and waiting with an infinite timeout for that fence to signal.
		return VulkanUtility::Error::CheckResult(vkQueueWaitIdle(static_cast<VkQueue>(Queue_Retrieve(queueType))));
	}
}