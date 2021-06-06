#pragma once
#include <string>
#include <functional>
#include "Engine.h"

struct SDL_Window;

namespace Amethyst
{
	class Window : public ISubsystem
	{
	public:

		void* RetrieveSDLHandle() const { return m_Window; }

	private:
		SDL_Window* m_Window = nullptr;
	};
}