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

		//Misc

	private:
		std::vector<PhysicalDevice> m_PhysicalDevices;
		uint32_t m_PhysicalDeviceIndex = 0;
	};
}