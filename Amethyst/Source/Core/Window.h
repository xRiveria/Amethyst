#pragma once
#include <string>
#include <functional>
#include "Engine.h"

// Forward Declarations
struct SDL_Window;
typedef union SDL_Event SDL_Event;

namespace Amethyst
{
	class Window : public ISubsystem
	{
	public:
		Window(Context* engineContext);
		~Window();

		// Subsystem
		void OnUpdate(float deltaTime) override;

		// Window Behavior
		void Show();
		void Hide();
		void Focus();
		void FullScreen();
		void FullScreenBorderless();
		void Minimize();
		void Maximize();
		void SetSize(const uint32_t width, const uint32_t height);

		// Retrievals
		uint32_t RetrieveWidth();
		uint32_t RetrieveHeight();
		bool WantsToClose() const { return m_Close; }
		bool IsMinimized() const { return m_Minimized; }
		bool IsFullScreen() const { return m_FullScreen; }
		void* RetrieveHandle();
		void* RetrieveSDLHandle() const { return m_Window; }

	private:
		std::string m_WindowTitle = "Amethyst";
		Math::Vector2 m_Position = Math::Vector2::Zero;
		uint32_t m_Width = 640;
		uint32_t m_Height = 480;
		bool m_Shown = false;
		bool m_Close = false;
		bool m_Minimized = false;
		bool m_Maximized = false;
		bool m_FullScreen = false;

		SDL_Window* m_Window = nullptr;
	};
}