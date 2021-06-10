#pragma once
#include <memory>

namespace Amethyst
{
	class Context;
	class Timer;

	enum EngineMode : uint32_t
	{
		Engine_Physics = 1UL << 0,	//Should Physics be working?
		Engine_Game    = 1UL << 1,		//Is the game running in engine or editor mode?
	};

	class Engine
	{
	public:
		Engine();
		~Engine();

		//Performs a simulation cycle.
		void OnUpdate() const;

		//Flags
		uint32_t EngineMode_RetrieveAll() const { return m_EngineFlags; }
		void EngineMode_SetAll(const uint32_t flags) { m_EngineFlags = flags; }
		void EngineMode_Enable(const EngineMode flag) { m_EngineFlags |= flag; }
		void EngineMode_Disable(const EngineMode flag) { m_EngineFlags &= ~flag; }
		void EngineMode_Toggle(const EngineMode flag) { m_EngineFlags = !EngineMode_IsToggled(flag) ? m_EngineFlags | flag : m_EngineFlags &~ flag; }
		bool EngineMode_IsToggled(const EngineMode flag) const { m_EngineFlags & flag; }

		Context* RetrieveContext() const { return m_Context.get(); }

	private:
		uint32_t m_EngineFlags = 0;
		Timer* m_Timer = nullptr;
		std::shared_ptr<Context> m_Context;
	};
}