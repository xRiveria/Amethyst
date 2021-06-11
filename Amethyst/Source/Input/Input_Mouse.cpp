#include "Amethyst.h"
#include "Input.h"
#include "SDL/SDL.h"
#include "Window.h"

namespace Amethyst
{
	void Input::PollMouse()
	{
		// Retrieve State
		int x, y;
		Uint32 keyStates = SDL_GetGlobalMouseState(&x, &y);
		Math::Vector2 position = Math::Vector2(static_cast<float>(x), static_cast<float>(y));

		// Retrieve Delta
		m_MouseDelta = position - m_MousePosition;

		// Retrieve Position
		m_MousePosition = position;

		// Retrieve Keys
		m_Keys[m_StartIndexMouse]	  = (keyStates & SDL_BUTTON(SDL_BUTTON_LEFT))   != 0;  // Left button pressed.
		m_Keys[m_StartIndexMouse + 1] = (keyStates & SDL_BUTTON(SDL_BUTTON_MIDDLE)) != 0;  // Middle button pressed.
		m_Keys[m_StartIndexMouse + 2] = (keyStates & SDL_BUTTON(SDL_BUTTON_RIGHT))  != 0;  // Right button pressed.
	}

	void Input::OnEventMouse(void* eventMouse)
	{
		// Validate Event
		AMETHYST_ASSERT(eventMouse != nullptr);

		SDL_Event* sdlEvent = static_cast<SDL_Event*>(eventMouse);
		Uint32 eventType = sdlEvent->type;

		// Wheel
		if (eventType == SDL_MOUSEWHEEL)
		{
			if (sdlEvent->wheel.x > 0)
			{
				m_MouseWheelDelta.m_X += 1;
			}

			if (sdlEvent->wheel.x < 0)
			{
				m_MouseWheelDelta.m_X -= 1;
			}

			if (sdlEvent->wheel.y > 0)
			{
				m_MouseWheelDelta.m_Y += 1;
			}

			if (sdlEvent->wheel.y < 0)
			{
				m_MouseWheelDelta.m_Y -= 1;
			}
		}
	}

	void Input::SetMouseCursorVisible(const bool isVisible)
	{
		if (isVisible == m_MouseCursorVisible)
		{
			return;
		}

		if (isVisible)
		{
			if (SDL_ShowCursor(SDL_ENABLE) != 0) // Returns 0 if the mouse cursor is shown.
			{
				AMETHYST_ERROR("Failed to show cursor.");
				return;
			}
		}
		else
		{
			if (SDL_ShowCursor(SDL_DISABLE) != 1) // Returns 1 if the mouse cursor is hidden.
			{
				AMETHYST_ERROR("Failed to hide cursor.");
				return;
			}
		}

		m_MouseCursorVisible = isVisible;
	}

	void Input::SetMousePosition(const Math::Vector2& position)
	{
		if (SDL_WarpMouseGlobal(static_cast<int>(position.m_X), static_cast<int>(position.m_Y)) != 0)
		{
			AMETHYST_ERROR("Failed to set mouse position.");
			return;
		}

		m_MousePosition = position;
	}

	const Math::Vector2 Input::GetMousePositionRelativeToWindow() const
	{
		SDL_Window* window = static_cast<SDL_Window*>(m_EngineContext->RetrieveSubsystem<Window>()->RetrieveSDLHandle());
		int windowX, windowY;
		SDL_GetWindowPosition(window, &windowX, &windowY); // Retrieves the position of the window.

		return Math::Vector2(static_cast<float>(m_MousePosition.m_X - windowX), static_cast<float>(m_MousePosition.m_Y - windowY));
	}

	const Math::Vector2 Input::GetMousePositionRelativeToEditorViewport() const
	{
		return GetMousePositionRelativeToWindow() - m_EditorViewportOffset;
	}
 }