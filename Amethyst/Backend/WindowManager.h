#pragma once
#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <GLFW/glfw3.h>

namespace Amethyst
{
	struct WindowProperties
	{
		WindowProperties() {}
		WindowProperties(const std::string& windowName, const uint32_t& windowWidth, const uint32_t& windowHeight) : 
			m_WindowName(windowName), m_WindowWidth(windowWidth), m_WindowHeight(windowHeight)
		{

		}

		std::string m_WindowName = "Empty Window";
		uint32_t m_WindowWidth = 1920;
		uint32_t m_WindowHeight = 1080;
	};

	class WindowManager
	{
	public:
		WindowManager();
		~WindowManager();

		GLFWwindow* CreateNewWindow(const WindowProperties& windowProperties);
		void SetWindowContext(GLFWwindow* window);

	private:
		void InitializeWindowManager();

	private:
		std::unordered_map<GLFWwindow*, WindowProperties> m_Windows;
	};
}