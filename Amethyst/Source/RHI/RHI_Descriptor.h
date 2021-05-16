#pragma once
#include "RHI_Utilities.h"
#include "../Utilities/Hash.h"

namespace Amethyst
{
	struct RHI_Descriptor
	{
		RHI_Descriptor() = default;

		RHI_Descriptor(const RHI_Descriptor& descriptor)
		{
			m_DescriptorType			 = descriptor.m_DescriptorType;
			m_Slot						 = descriptor.m_Slot;
			m_Stage						 = descriptor.m_Stage;
			m_IsStorage					 = descriptor.m_IsStorage;
			m_IsDynamicConstantBuffer	 = descriptor.m_IsDynamicConstantBuffer;
			m_Name						 = descriptor.m_Name;
		}

		RHI_Descriptor(const std::string& name, const RHI_Descriptor_Type descriptorType, const uint32_t slot, const uint32_t stage, const bool isStorage, const bool isDynamicConstantBuffer)
		{
			this->m_DescriptorType			 = descriptorType;
			this->m_Slot					 = slot;
			this->m_Stage					 = stage;
			this->m_IsStorage				 = isStorage;
			this->m_IsDynamicConstantBuffer  = isDynamicConstantBuffer;
			this->m_Name					 = name;
		}

		//Hash.


		uint32_t m_Slot							= 0;
		uint32_t m_Stage						= 0;
		uint64_t m_Offset						= 0;
		uint64_t m_Range						= 0;
		RHI_Descriptor_Type m_DescriptorType    = RHI_Descriptor_Type::Undefined;
		RHI_Image_Layout m_Layout				= RHI_Image_Layout::Undefined;
		bool m_IsStorage						= false;
		bool m_IsDynamicConstantBuffer			= false;
		std::string m_Name						= "";
		
		void* m_Resource						= nullptr;
	};
}