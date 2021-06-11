#include "Amethyst.h"
#include "Window.h"
#include "SDL/SDL.h"
#include "SDL/SDL_syswm.h"

// Linking - Statically linking SDL2 requires that we link to all the libraries it uses. We will use this when we convert the Editor into a seperate project.
// Right now, the Window is set to our SDL context itself. Perhaps next time we can create a window context and use it launch off multiple windows.

namespace Amethyst
{
	Window::Window(Context* engineContext) : ISubsystem(engineContext)
	{
		// Initialize video subsystem (if needed).
		if (SDL_WasInit(SDL_INIT_VIDEO) != 1) // If not initialized...
		{
			if (SDL_InitSubSystem(SDL_INIT_VIDEO) != 0)
			{
				AMETHYST_ERROR("Failed to initialize SDL Video subsystem: %s.", SDL_GetError());
				return;
			}
		}

		// Initialize events subsystem (if needed).
		if (SDL_WasInit(SDL_INIT_EVENTS) != 1) // If not initialized...
		{
			if (SDL_InitSubSystem(SDL_INIT_EVENTS) != 0)
			{
				AMETHYST_ERROR("Failed to initialize SDL Events subsystem: %s.", SDL_GetError());
				return;
			}
		}

		// Create Window.
		uint32_t flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_MAXIMIZED; // Window is Visible, Resizable and Maximized at creation.
		m_Window = SDL_CreateWindow(
			m_WindowTitle.c_str(),		// Window Title
			SDL_WINDOWPOS_UNDEFINED,	// Initial X Position
			SDL_WINDOWPOS_UNDEFINED,	// Initial Y Position
			m_Width,					// Width in Pixels
			m_Height,					// Height in Pixels
			flags						// Flags
		);

		if (!m_Window)
		{
			AMETHYST_ERROR("Engine could not create window %s.", SDL_GetError());
			return;
		}

		/// Register Library
	}

	Window::~Window()
	{
		// Destroy Window
		SDL_DestroyWindow(m_Window);

		// Shutdown SDL
		SDL_Quit();
	}

	void Window::OnUpdate(float deltaTime)
	{
		// Process Events
		SDL_Event sdl_Event;

		while (SDL_PollEvent(&sdl_Event))
		{
			if (sdl_Event.type == SDL_WINDOWEVENT) // Window state changes.
			{
				switch (sdl_Event.window.event)
				{
					case SDL_WINDOWEVENT_SHOWN:
						m_Shown = true;
						break;

					case SDL_WINDOWEVENT_HIDDEN:
						m_Shown = false;
						break;

					case SDL_WINDOWEVENT_EXPOSED:		// Window has been exposed and should be redrawn.
						break;

					case SDL_WINDOWEVENT_MOVED:			// Window has been moved to data1, data2.
						break;

					case SDL_WINDOWEVENT_RESIZED:
						m_Width = static_cast<uint32_t>(sdl_Event.window.data1);
						m_Height = static_cast<uint32_t>(sdl_Event.window.data2);
						break;

					case SDL_WINDOWEVENT_SIZE_CHANGED:
						m_Width = static_cast<uint32_t>(sdl_Event.window.data1);
						m_Height = static_cast<uint32_t>(sdl_Event.window.data2);
						break;

					case SDL_WINDOWEVENT_MINIMIZED:
						m_Minimized = true;
						m_Maximized = false;
						break;

					case SDL_WINDOWEVENT_MAXIMIZED:
						m_Minimized = false;
						m_Maximized = true;
						break;

					case SDL_WINDOWEVENT_RESTORED:
						break;

					case SDL_WINDOWEVENT_ENTER:			// Window has gained mouse focus.
						break;

					case SDL_WINDOWEVENT_LEAVE:			// Window has lost mouse focus.
						break;

					case SDL_WINDOWEVENT_FOCUS_GAINED:  // Window has gained keyboard focus.
						break;

					case SDL_WINDOWEVENT_FOCUS_LOST:    // Window has lost keyboard focus.
						break;

					case SDL_WINDOWEVENT_CLOSE:
						m_Close = true;
						break;

					case SDL_WINDOWEVENT_TAKE_FOCUS:
						break;

					case SDL_WINDOWEVENT_HIT_TEST:
						break;

					default:
						AMETHYST_ERROR("Unhandled Window Event.");
						break;
				}
			}

			FIRE_EVENT_DATA(EventType::EventSDL, &sdl_Event);
		}
	}

	void Window::Show()
	{
		AMETHYST_ASSERT(m_Window != nullptr);

		SDL_ShowWindow(m_Window);
	}

	void Window::Hide()
	{
		AMETHYST_ASSERT(m_Window != nullptr);

		SDL_HideWindow(m_Window);
	}

	void Window::Focus()
	{
		AMETHYST_ASSERT(m_Window != nullptr);

		SDL_RaiseWindow(m_Window);
	}

	void Window::FullScreen()
	{
		AMETHYST_ASSERT(m_Window != nullptr);

		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN); // SDL_WINDOW_FULLSCREEN for a "real" full-screen experience with video-mode changes.
		m_FullScreen = true;
	}

	void Window::FullScreenBorderless()
	{
		AMETHYST_ASSERT(m_Window != nullptr);

		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_FULLSCREEN_DESKTOP); // SDL_WINDOW_FULLSCREEN_DESKTOP for a fake fullscreen that takes the size of the desktop.
		m_FullScreen = true;
	}

	void Window::Minimize()
	{
		AMETHYST_ASSERT(m_Window != nullptr);
		
		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_MINIMIZED);
	}

	void Window::Maximize()
	{
		AMETHYST_ASSERT(m_Window != nullptr);

		SDL_SetWindowFullscreen(m_Window, SDL_WINDOW_MAXIMIZED);
	}

	void Window::SetSize(const uint32_t width, const uint32_t height)
	{
		AMETHYST_ASSERT(m_Window != nullptr);

		SDL_SetWindowSize(m_Window, static_cast<int>(m_Width), static_cast<int>(m_Height));
	}

	uint32_t Window::RetrieveWidth()
	{
		AMETHYST_ASSERT(m_Window != nullptr);
		
		int width = 0;
		SDL_GetWindowSize(m_Window, &width, nullptr);
		return static_cast<uint32_t>(width);
	}

	uint32_t Window::RetrieveHeight()
	{
		AMETHYST_ASSERT(m_Window != nullptr);
		
		int height = 0;
		SDL_GetWindowSize(m_Window, nullptr, &height);
		return static_cast<uint32_t>(height);
	}

	void* Window::RetrieveHWND()
	{
		AMETHYST_ASSERT(m_Window != nullptr);

		SDL_SysWMinfo systemInfo;
		SDL_VERSION(&systemInfo.version);
		SDL_GetWindowWMInfo(m_Window, &systemInfo);

		return static_cast<void*>(systemInfo.info.win.window);
	}
}