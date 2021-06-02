#include "Amethyst.h"
#include "RHI_Device.h"
#include "RHI_Implementation.h"

namespace Amethyst
{
	void RHI_Device::RegisterPhysicalDevice(const PhysicalDevice& physicalDevice)
	{
		m_PhysicalDevices.emplace_back(physicalDevice);

		//Keep devices sorted based on memory (from highest to lowest).
		std::sort(m_PhysicalDevices.begin(), m_PhysicalDevices.end(), [](const PhysicalDevice& device1, const PhysicalDevice& device2)
		{
			return device1.RetrieveDeviceMemory() > device2.RetrieveDeviceMemory();
		});

		AMETHYST_INFO("Registered Physical Device: %s (%d MB)", physicalDevice.RetrieveDeviceName().c_str(), physicalDevice.RetrieveDeviceMemory());
	}

	const PhysicalDevice* RHI_Device::RetrievePrimaryPhysicalDevice()
	{
		if (m_PrimaryPhysicalDeviceIndex >= m_PhysicalDevices.size()) //If our main device index falls outside the range of our registered devices...
		{
			return nullptr; 
		}

		return &m_PhysicalDevices[m_PrimaryPhysicalDeviceIndex];
	}

	void RHI_Device::SetPrimaryPhysicalDevice(const uint32_t index)
	{
		m_PrimaryPhysicalDeviceIndex = index;

		if (const PhysicalDevice* physicalDevice = RetrievePrimaryPhysicalDevice())
		{
			AMETHYST_INFO("Set Primary Physical Device: %s (%d MB", physicalDevice->RetrieveDeviceName().c_str(), physicalDevice->RetrieveDeviceMemory());
		}
	}

	bool RHI_Device::IsValidResolution(const uint32_t width, const uint32_t height)
	{
		return width  > 4 && width  << RHI_Context::m_Texture2D_DimensionsMax &&  ///
			   height > 4 && height << RHI_Context::m_Texture2D_DimensionsMax;
	}

	bool RHI_Device::Queue_WaitAll() const
	{
		return Queue_Wait(RHI_Queue_Graphics) && Queue_Wait(RHI_Queue_Transfer) && Queue_Wait(RHI_Queue_Compute);
	}

	void* RHI_Device::Queue_Retrieve(const RHI_Queue_Type queueType) const
	{
		if (queueType == RHI_Queue_Type::RHI_Queue_Graphics)
		{
			return m_RHI_Context->m_Queue_Graphics;
		}
		else if (queueType == RHI_Queue_Type::RHI_Queue_Transfer)
		{
			return m_RHI_Context->m_Queue_Transfer;
		}
		else if (queueType == RHI_Queue_Type::RHI_Queue_Compute)
		{
			return m_RHI_Context->m_Queue_Compute;
		}

		return nullptr;
	}

	uint32_t RHI_Device::Queue_Index(const RHI_Queue_Type queueType) const // Retrieve Queue Index.
	{
		if (queueType == RHI_Queue_Type::RHI_Queue_Graphics)
		{
			return m_RHI_Context->m_Queue_GraphicsIndex;
		}
		else if (queueType == RHI_Queue_Type::RHI_Queue_Transfer)
		{
			return m_RHI_Context->m_Queue_TransferIndex;
		}
		else if (queueType == RHI_Queue_Type::RHI_Queue_Compute)
		{
			return m_RHI_Context->m_Queue_ComputeIndex;
		}

		return 0;
	}
}