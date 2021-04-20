#include "MenuBar.h"
#include <ImGui/imgui_internal.h>

namespace Amethyst
{
	MenuBar::MenuBar(Editor* editorContext) : Widget(editorContext)
	{
		m_WidgetName = "Menu Bar";
		m_IsWindowedWidget = false;
	}

	void MenuBar::OnTickAlways()
	{
		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				ImGui::MenuItem("New Scene", "Ctrl + N");
				ImGui::MenuItem("Open Scene", "Ctrl + O");
				ImGui::Separator();
				ImGui::MenuItem("Save");
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Edit"))
			{
				ImGui::Separator();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Assets"))
			{
				ImGui::Separator();
				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Object"))
			{

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Windows"))
			{

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("Help"))
			{

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
}