#pragma once
#include "RHI_Utilities.h"
#include <string>

namespace Amethyst
{
	class PhysicalDevice
	{
	public:
		PhysicalDevice(const uint32_t apiVersion, const uint32_t driverVersion, const uint32_t vendorID, const RHI_PhysicalDevice_Type deviceType, const char* deviceName, const uint32_t deviceMemory, void* data)
		{	
			this->m_VendorID = vendorID;
			this->m_DeviceType = deviceType;
			this->m_DeviceName = deviceName;
			this->m_DeviceMemory = deviceMemory / 1024 / 1024; //As querying is done in bytes, we divide by 1024 twice to retrieve the memory size in MB.
			this->m_Data = data;
			this->m_APIVersion = DecodeDriverVersion(apiVersion);
			this->m_DriverVersion = DecodeDriverVersion(driverVersion);
		}

		/*	Vendor IDs - https://devicehunt.com/

			0x10DE - Nvidia
			0x8086 - Intel
			0x1002 - AMD
		*/

		bool IsNvidiaDevice() const { return m_VendorID == 0x10DE || m_DeviceName.find("Nvidia") != std::string::npos; }
		bool IsAMDDevice() const	{ return m_VendorID == 0x1022 || m_VendorID == 0x1002 || m_DeviceName.find("Amd") != std::string::npos; }
		bool IsIntelDevice() const  { return m_VendorID == 0x8086 || m_VendorID == 0x163C || m_VendorID == 0x8087 || m_DeviceName.find("Intel") != std::string::npos; }

		const std::string& RetrieveDeviceName() const { return m_DeviceName; }
		const std::string& RetrieveDriverVersion() const { return m_DriverVersion; }
		const std::string& RetrieveAPIVersion() const { return m_APIVersion; }
		uint32_t RetrieveDeviceMemory() const { return m_DeviceMemory; }
		void* RetrieveData() const { return m_Data; }

	private:
		std::string DecodeDriverVersion(const uint32_t version) //Stores driver versions in a readable format.
		{
			char buffer[256];

			if (IsNvidiaDevice())
			{
				
			}

			return buffer;
		}

	private:
		std::string m_APIVersion = "Unknown"; //Version of the API supported by the device.
		std::string m_DriverVersion = "Unknown"; //Vendor-specified version of the driver.
		std::string m_DeviceName = "Unknown";
		uint32_t m_DeviceMemory = 0;
		uint32_t m_VendorID = 0; //Unique identifier of the vendor.
		RHI_PhysicalDevice_Type m_DeviceType = RHI_PhysicalDevice_Unknown;
		void* m_Data = nullptr;
	};
}