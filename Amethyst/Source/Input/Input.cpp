#include "Amethyst.h"
#include "Input.h"
#include "SDL/SDL.h"

namespace Amethyst
{
	Input::Input(Context* engineContext) : ISubsystem(engineContext)
	{
		// Initialize events subsystem if needed.
		if (SDL_WasInit(SDL_INIT_EVENTS) != 1)
		{	
			if (SDL_InitSubSystem(SDL_INIT_EVENTS) != 0)
			{
				AMETHYST_ERROR("Failed to initialize SDL Events Subsystem: %s.", SDL_GetError());
				return;
			}
		}

		/// Initialize controller subsystem if needed.

		// All keys are set false.
		m_Keys.fill(false);
		m_KeysPreviousFrame.fill(false);

		// Retrieve events from the main Window's event processing loop.
		SUBSCRIBE_TO_EVENT(EventType::EventSDL, EVENT_HANDLER_VARIANT(OnEvent));
	}

	void Input::OnUpdate(float deltaTime)
	{
		m_KeysPreviousFrame = m_Keys;

		PollMouse();
		PollKeyboard();
	}

	void Input::OnPostUpdate()
	{
		m_MouseWheelDelta = Math::Vector2::Zero;
	}

	void Input::OnEvent(const Variant& eventVariant)
	{
		SDL_Event* eventSDL = eventVariant.RetrieveValue<SDL_Event*>();
		Uint32 eventType = eventSDL->type; // Uint32 is an SDL type that is inherently a uint32_t.

		if (eventType == SDL_MOUSEWHEEL)
		{
			OnEventMouse(eventSDL);
		}

		/// Controller Event Types.
	}
}