#include "ObjectsPanel.h"
#include "../Utilities/EditorExtensions.h"
#include "../Utilities/IconLibrary.h"
#include <iostream>

namespace Amethyst
{
	ObjectsPanel::ObjectsPanel(Editor* editorContext) : Widget(editorContext)
	{
		m_EditorContext = editorContext;
		m_WidgetName = "Objects";
	}

	void ObjectsPanel::OnVisibleTick()
	{
		const auto ObjectEntry = [this](const std::string& objectName)
		{
			//Make work with our syntax.
			ImGui::Spacing();
			ImGuiExtensions::Image(IconType::Icon_ObjectPanel_Cube, m_IconSize);
			ImGui::SameLine();
			ImGui::Text(objectName.c_str());

			ObjectDrag(objectName);
		};

		bool derp = true;
		ImGui::ShowDemoWindow(&derp);

		ImGuiTabBarFlags tarBarFlags = ImGuiTabBarFlags_None;
		if (ImGui::BeginTabBar("##Items", tarBarFlags))
		{
			if (ImGui::BeginTabItem("Shapes"))
			{
				ObjectEntry("Cube");
				ObjectEntry("Capsule");
				ObjectEntry("Sphere");
				ObjectEntry("Cylinder");
				ObjectEntry("Torus");
				ObjectEntry("Plane");

				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Lighting"))
			{
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}
	}

	void TestPayload(ImGuiExtensions::DragPayloadType payloadType)
	{
		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload(reinterpret_cast<const char*>(&payloadType)))
			{
				std::cout << "Derp";
			}
			ImGui::EndDragDropTarget();
		}
	}

	void ObjectsPanel::ObjectDrag(const std::string& objectType) const
	{
		if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
		{
			const auto SetPayload = [this](const ImGuiExtensions::DragPayloadType payloadType, const std::string& filePath)
			{
				m_DragDropPayload.m_PayloadType = payloadType;
				m_DragDropPayload.m_Data = filePath.c_str();
				ImGuiExtensions::CreateDragPayload(m_DragDropPayload);
			};
			
			SetPayload(ImGuiExtensions::DragPayloadType::DragPayload_Model, "Derp...");

			//Preview
			ImGuiExtensions::Image(IconType::Icon_ObjectPanel_Cube, 50.0f);
			ImGui::Text(objectType.c_str());

			ImGui::EndDragDropSource();
		}
	}
}