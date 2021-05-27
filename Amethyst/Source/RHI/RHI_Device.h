#pragma once
#include "../Core/AmethystObject.h"
#include <memory>
#include <mutex>
#include <vector>
#include "RHI_PhysicalDevice.h"

namespace Amethyst
{
	class RHI_Device : public AmethystObject
	{
	public:
		RHI_Device(Context* context);
		~RHI_Device();

		//Physical Device
		void RegisterPhysicalDevice(const PhysicalDevice& physicalDevice);
		const PhysicalDevice* RetrievePrimaryPhysicalDevice();
		void SetPrimaryPhysicalDevice(const uint32_t index);
		const std::vector<PhysicalDevice>& RetrievePhysicalDevices() const { return m_PhysicalDevices; }

		//Queue
		bool Queue_Present(void* swapchainView, uint32_t* imageIndex, RHI_Semaphore* waitSemaphore = nullptr) const;
		bool Queue_Submit(const RHI_Queue_Type queueType, const uint32_t waitFlags, void* commandBuffer, RHI_Semaphore* waitSemaphore = nullptr, RHI_Semaphore* signalSemaphore = nullptr, RHI_Fence* signalFence = nullptr) const;
		bool Queue_Wait(const RHI_Queue_Type queueType) const;

		bool Queue_WaitAll() const;
		void* Queue_Retrieve(const RHI_Queue_Type queueType) const;
		uint32_t Queue_Index(const RHI_Queue_Type queueType) const;

		//Misc
		bool ValidateResolution(const uint32_t width, const uint32_t height) const;
		bool IsInitialized() const { return m_IsInitialized; }

		RHI_Context* RetrieveContextRHI() const { return m_RHI_Context.get(); }
		Context* RetrieveContextEngine() const { return m_Context; }
		uint32_t RetrieveEnabledGraphicsStages() const { return m_EnabledGraphicsShaderStages; }

	private:
		std::vector<PhysicalDevice> m_PhysicalDevices;
		uint32_t m_PrimaryPhysicalDeviceIndex = 0;
		uint32_t m_EnabledGraphicsShaderStages = 0;

		mutable std::mutex m_QueueMutex;
		bool m_IsInitialized = false;
		std::shared_ptr<RHI_Context> m_RHI_Context;
	};
}