#include "Amethyst.h"
#include "WindowManager.h"
#include <iostream>

namespace Amethyst
{
	WindowManager::WindowManager()
	{
		InitializeWindowManager();
	}

	WindowManager::~WindowManager()
	{
	}

	GLFWwindow* WindowManager::CreateNewWindow(const WindowProperties& windowProperties)
	{
		GLFWwindow* newWindow = glfwCreateWindow((int)windowProperties.m_WindowWidth, (int)windowProperties.m_WindowHeight, windowProperties.m_WindowName.c_str(), nullptr, nullptr);
		m_Windows[newWindow] = windowProperties;
		AMETHYST_INFO("Created Window Named %s", windowProperties.m_WindowName.c_str());

		return newWindow;
	}

	void WindowManager::SetWindowContext(GLFWwindow* window)
	{
		for (const auto& storedWindow : m_Windows)
		{
			if (storedWindow.first == window)
			{
				glfwMakeContextCurrent(window);
				AMETHYST_INFO("Set Window Context for %s", m_Windows[0].m_WindowName.c_str());
				return;
			}
		}
	}

	void WindowManager::InitializeWindowManager()
	{
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
		glfwWindowHint(GLFW_RESIZABLE, true);
		glfwWindowHint(GLFW_SAMPLES, 64);

		if (!glfwInit())
		{

		}
		else
		{
			//AMETHYST_INFO("Initialized GLFW.");
		}
	}
}