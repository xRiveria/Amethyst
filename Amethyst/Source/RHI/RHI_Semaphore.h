#pragma once
#include "../Core/AmethystObject.h"
#include "RHI_Utilities.h"

namespace Amethyst
{
	class RHI_Semaphore : public AmethystObject
	{
	public:
		RHI_Semaphore(RHI_Device* rhi_Device, bool isTimeline = false, const char* name = nullptr);
		~RHI_Semaphore();

		//Timeline
		bool IsTimelineSemaphore() const { return m_IsTimelineSemaphore; }
		bool Wait(const uint64_t value, const uint64_t timeout = std::numeric_limits<uint64_t>::max());
		bool Signal(const uint64_t value);
		uint64_t RetrieveValue();

		RHI_Semaphore_State RetrieveState() const { return m_State; }
		void SetState(const RHI_Semaphore_State state) { m_State = state; }
		void* RetrieveResource() { return m_Resource; }

	private:
		void* m_Resource = nullptr;
		bool m_IsTimelineSemaphore = false;
		RHI_Semaphore_State m_State = RHI_Semaphore_State::Idle;

		RHI_Device* m_RHI_Device = nullptr;
	};
}