#include <iostream>
#include <string>
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Backend/WindowManager.h"
#include "Backend/Editor.h"
#include "ImGui/imgui.h"
#include "Source/FileSystem.h"
#include "ImGui/imgui_internal.h"

Amethyst::WindowManager s_WindowManager;
Amethyst::Editor s_Editor;
char directoryBufferWithExtensions[256] = "Source/";
char directoryBufferWithoutExtensions[256] = "Source/";

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

int selectedIndex = -1;
std::string currentlySelectedFile = "Select File";

void RenderEditor()
{
	s_Editor.BeginEditorRenderLoop();
	s_Editor.RenderDockingContext(); //This begins a Begin().

	ImGui::Begin("File System");
	if (ImGui::CollapsingHeader("Directory Listing"))
	{
		if (ImGui::Button("Create"))
		{
			Amethyst::FileSystem::CreateTextFile("Source/File.txt", "Hello!");
		}
		ImGui::SameLine();
		if (ImGui::Button("Delete"))
		{
			Amethyst::FileSystem::Delete(currentlySelectedFile);
			currentlySelectedFile = "Select File";
		}
		
		ImGui::InputText("Directory##withExtensions", directoryBufferWithExtensions, sizeof(directoryBufferWithExtensions));
		if (Amethyst::FileSystem::Exists(directoryBufferWithExtensions))
		{
			std::vector<std::string> filesInDirectory = Amethyst::FileSystem::RetrieveFilesInDirectory(directoryBufferWithExtensions);
			if (!filesInDirectory.empty())
			{
				for (int i = 0; i < filesInDirectory.size(); i++)
				{
					std::string label = "- " + filesInDirectory[i];
					if (ImGui::Selectable(label.c_str(), selectedIndex == i))
					{
						selectedIndex = i;
						currentlySelectedFile = filesInDirectory[i];
					}
				}
			}
		}
	}

	ImGui::Spacing();

	if (ImGui::CollapsingHeader("Directory Listing (Without Extensions)"))
	{
		ImGui::InputText("Directory##withoutExtensions", directoryBufferWithoutExtensions, sizeof(directoryBufferWithoutExtensions));
		if (Amethyst::FileSystem::Exists(directoryBufferWithoutExtensions))
		{
			for (const std::string& file : Amethyst::FileSystem::RetrieveFilesInDirectory(directoryBufferWithoutExtensions))
			{
				ImGui::Text(Amethyst::FileSystem::RetrieveFileNameWithNoExtensionFromFilePath(file).c_str());
			}
		}
	}
	ImGui::End();

	ImGui::Begin("Visualization");	

	ImGui::End();

	s_Editor.EndEditorRenderLoop();
}