#include <string>
#define GLEW_STATIC
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "Backend/WindowManager.h"
#include "Source/Editor/Editor.h"
#include "ImGui/imgui.h"
#include "Source/FileSystem.h"
#include "ImGui/imgui_internal.h"
#include "yaml-cpp/yaml.h"
#include "Source/Serializable/MaterialSerializer.h"
#include "Source/Editor/Widgets/Console.h"
#include "Source/Editor/Widgets/MenuBar.h"
#include "Source/Editor/Widgets/Toolbar.h"
#include "Source/Editor/Utilities/IconLibrary.h"

Amethyst::WindowManager s_WindowManager;
Amethyst::Editor s_Editor;
Amethyst::MaterialSerializer s_MaterialSerializer;
Amethyst::Console m_Console(&s_Editor);
Amethyst::MenuBar m_MenuBar(&s_Editor);
Amethyst::Toolbar m_Toolbar(&s_Editor);

char directoryBufferWithExtensions[256] = "Source/";
char directoryBufferWithoutExtensions[256] = "Source/";
Amethyst::Material dummyModelMaterial;

void RenderEditor();
void RenderMaterialUI();

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
	Amethyst::IconLibrary::RetrieveIconLibrary().InitializeIconLibrary();

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
	m_Console.OnUpdate();
	m_MenuBar.OnUpdate();
	m_Toolbar.OnUpdate();

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

	ImGui::Begin("Viewport");
	ImGui::End();

	ImGui::End();

	RenderMaterialUI();

	s_Editor.EndEditorRenderLoop();
}

void RenderMaterialUI()
{
	ImGui::Begin("Material");

	{
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), dummyModelMaterial.m_MaterialName.c_str());

		if (ImGui::InputText("Material Name", buffer, sizeof(buffer)))
		{
			dummyModelMaterial.m_MaterialName = std::string(buffer);
		}
	}

	{
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), dummyModelMaterial.m_NormalMapFilePath.c_str());

		if (ImGui::InputText("Normal Map", buffer, sizeof(buffer)))
		{
			dummyModelMaterial.m_NormalMapFilePath = std::string(buffer);
		}
	}

	{
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), dummyModelMaterial.m_SpecularMapFilePath.c_str());

		if (ImGui::InputText("Specular Map", buffer, sizeof(buffer)))
		{
			dummyModelMaterial.m_SpecularMapFilePath = std::string(buffer);
		}
	}

	{
		char buffer[256];
		memset(buffer, 0, sizeof(buffer));
		strcpy_s(buffer, sizeof(buffer), dummyModelMaterial.m_AlbedoMapFilePath.c_str());

		if (ImGui::InputText("Albedo Map", buffer, sizeof(buffer)))
		{
			dummyModelMaterial.m_AlbedoMapFilePath = std::string(buffer);
		}
	}

	ImGui::Checkbox("Shadow Casting", &dummyModelMaterial.m_ShadowCasting);
	ImGui::Checkbox("Shadow Receiving", &dummyModelMaterial.m_ShadowReceiving);

	if (ImGui::Button("Save To Path"))
	{
		s_MaterialSerializer.SaveMaterialToFile(dummyModelMaterial, "Resources/Materials/Material1.material");
	}

	if (ImGui::Button("Load From Path"))
	{
		s_MaterialSerializer.LoadMaterialFromFile(dummyModelMaterial, "Resources/Materials/Material1.material");
	}

	ImGui::End();
}
