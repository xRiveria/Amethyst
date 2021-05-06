#include "Amethyst.h"
#include "Engine.h"
#include "Context.h"
#include "Timer.h"
#include "../Runtime/ECS/World.h"

namespace Amethyst
{
	Engine::Engine()
	{
		//Flags
		m_EngineFlags |= Engine_Physics;
		m_EngineFlags |= Engine_Game;

		//Create our Context.
		m_Context = std::make_shared<Context>();
		m_Context->m_Engine = this;

		//Register Subsystems.
		m_Context->RegisterSubsystem<Timer>();
		m_Context->RegisterSubsystem<World>(TickType::Smoothed);

		//Initialize all our above subsystems.
		m_Context->InitializeSubsystems();

		//Timer.
		m_Timer = m_Context->RetrieveSubsystem<Timer>();
	}

	Engine::~Engine()
	{
	}

	void Engine::OnUpdate() const
	{
		m_Context->OnUpdate(TickType::Variable, static_cast<float>(m_Timer->RetrieveDeltaTimeInMilliseconds()));
		m_Context->OnUpdate(TickType::Smoothed, static_cast<float>(m_Timer->RetrieveDeltaTimeSmoothedInMilliseconds()));
	}
}