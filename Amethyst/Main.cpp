#include <iostream>
#include <string>
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Backend/WindowManager.h"
#include "Backend/Editor.h"
#include "ImGui/imgui.h"

Amethyst::WindowManager s_WindowManager;
Amethyst::Editor s_Editor;

void RenderEditor();

int main()
{
	Amethyst::WindowProperties windowProperties = { "Amethyst", 1280, 720 };
	GLFWwindow* mainWindow = s_WindowManager.CreateNewWindow(windowProperties);
	s_WindowManager.SetWindowContext(mainWindow);

	if (!glewInit())
	{

	}

	//Editor
	s_Editor.InitializeEditor(std::pair<GLFWwindow*, Amethyst::WindowProperties>(mainWindow, windowProperties));
	
	while (true)
	{
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);
		RenderEditor();
		glfwSwapBuffers(mainWindow);
	}
}

void RenderEditor()
{
	s_Editor.BeginEditorRenderLoop();
	s_Editor.RenderDockingContext(); //This begins a Begin().

	ImGui::Begin("Hello");
	ImGui::End();

	s_Editor.EndEditorRenderLoop();
}