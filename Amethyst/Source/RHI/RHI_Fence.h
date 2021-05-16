#pragma once
#include "../Core/AmethystObject.h"
#include "RHI_Utilities.h"

namespace Amethyst
{
	class RHI_Fence : public AmethystObject
	{
	public:
		RHI_Fence(RHI_Device* rhi_Device, const char* name = nullptr);
		~RHI_Fence();

		bool IsSignaled();
		bool Wait(uint64_t timeout = std::numeric_limits<uint64_t>::max());
		bool Reset();

		void* RetrieveResource() { return m_Resource; }

	private:
		void* m_Resource = nullptr;
		RHI_Device* m_RHI_Device = nullptr;
	};
}