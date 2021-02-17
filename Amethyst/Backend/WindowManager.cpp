#include "WindowManager.h"
#include <utility>
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
		std::cout << "Created Window Named " << windowProperties.m_WindowName << "\n";

		return newWindow;
	}

	void WindowManager::SetWindowContext(GLFWwindow* window)
	{
		for (const auto& storedWindow : m_Windows)
		{
			if (storedWindow.first == window)
			{
				glfwMakeContextCurrent(window);
				std::cout << "Set Window Context for " << storedWindow.second.m_WindowName << "\n";
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
			std::cout << "Initialized GLFW." << "\n";
		}
	}
}