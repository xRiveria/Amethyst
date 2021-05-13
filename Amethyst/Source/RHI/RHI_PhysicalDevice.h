#pragma once
#include "RHI_Utilities.h"
#include <string>

namespace Amethyst
{
	class PhysicalDevice
	{
	public:


	private:
		std::string m_APIVersion = "Unknown"; //Version of the API supported by the device.
		std::string m_DriverVersion = "Unknown"; //Vendor-specified version of the driver.
		uint32_t m_VendorID = 0; //Unique identifier of the vendor.
		std::string m_DeviceName = "Unknown";
		uint32_t m_Memory = 0;
		void* m_Data = nullptr;
	};
}