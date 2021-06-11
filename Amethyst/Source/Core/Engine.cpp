#include "Amethyst.h"
#include "Engine.h"
#include "Context.h"
#include "Window.h"
#include "../Time/Timer.h"
#include "../Resource/ResourceCache.h"
#include "../Rendering/Renderer.h"
#include "../Threading/Threading.h"
#include "../Runtime/ECS/World.h"
#include "../Input/Input.h"

namespace Amethyst
{
	Engine::Engine()
	{
		// Flags
		m_EngineFlags |= Engine_Physics;
		m_EngineFlags |= Engine_Game;

		// Create our Context.
		m_Context = std::make_shared<Context>();
		m_Context->m_Engine = this;

		// Register Subsystems.
		/// Settings
		m_Context->RegisterSubsystem<Timer>(); /// Complete
		m_Context->RegisterSubsystem<Threading>(); /// Complete
		m_Context->RegisterSubsystem<Window>(); /// Complete
		m_Context->RegisterSubsystem<Input>(TickType::Smoothed);  /// Complete
		m_Context->RegisterSubsystem<ResourceCache>(); /// Complete
		/// Audio
		/// Physics
		/// Scripting
		m_Context->RegisterSubsystem<World>(TickType::Smoothed); /// Require More Math Stuff/Model/Mesh Classes
		m_Context->RegisterSubsystem<Renderer>(); /// Require Shader Class / RHI_ImGui Implementation
		/// Profiler

		/// Initialize all our above subsystems.
		m_Context->OnInitialize();
		m_Context->OnPreUpdate();
	}

	Engine::~Engine()
	{
		m_Context->OnShutdown();

		// Does this need to become a subsystem?
		EventSystem::RetrieveInstance().Reset();
	}

	void Engine::OnUpdate() const
	{
		Timer* timer = m_Context->RetrieveSubsystem<Timer>();

		m_Context->OnUpdate(TickType::Variable, static_cast<float>(timer->RetrieveDeltaTimeInSeconds()));
		m_Context->OnUpdate(TickType::Smoothed, static_cast<float>(timer->RetrieveDeltaTimeSmoothedInSeconds()));
		m_Context->OnPostUpdate();
	}
}