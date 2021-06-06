#include "Amethyst.h"
#include "Renderer.h"
#include "../Runtime/ECS/Entity.h"
#include "../RHI/RHI_Device.h"

namespace Amethyst
{
	Renderer::Renderer(Context* context) : ISubsystem(context)
	{
		// Options - Rendering
		m_Options |= Renderer_Option::Render_ReverseZ;

		// Options - Debugging
		m_Options |= Renderer_Option::RenderDebug_Transform;
		m_Options |= Renderer_Option::RenderDebug_Grid;
		m_Options |= Renderer_Option::RenderDebug_Lights;
		//m_Options |= Renderer_Option::RenderDebug_Physics;

		// Option Values - Rendering
		
		// Subscribe to events.
		SUBSCRIBE_TO_EVENT(EventType::WorldResolved, EVENT_HANDLER_VARIANT(RenderablesAcquire)); ///
		SUBSCRIBE_TO_EVENT(EventType::WorldClear, EVENT_HANDLER(Clear));						 ///
	}

	Renderer::~Renderer()
	{
		// Unsubscribe from events.
		UNSUBSCRIBE_FROM_EVENT(EventType::WorldResolved, EVENT_HANDLER_VARIANT(RenderablesAcquire));

		m_Entities.clear();
		m_Camera = nullptr;

		// Log to file as the renderer is no more.
		LOG_TO_FILE(true);
	}

	bool Renderer::InitializeSubsystem()
	{
		//Retrieve required systems.

		//Resolution, viewport andd swapchain default to whatever the window size is.

		//Set resolution.

		//Set viewport.

		//Create Device.
		m_RHI_Device = std::make_shared<RHI_Device>(m_Context);
		if (!m_RHI_Device->IsInitialized())
		{
			AMETHYST_INFO("Failed to create RHI Device.");
			return false;
		}

		//Create Pipeline Cache
		//Create Descriptor Set Layout Cache.

		//Create Swapchain


		return false;
	}

	void Renderer::OnUpdate(float deltaTime)
	{
	}
}