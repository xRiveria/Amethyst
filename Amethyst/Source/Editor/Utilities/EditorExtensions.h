#pragma once
#include "ImGui/imgui.h"
#include "IconLibrary.h"
#include "ImGui/imgui_internal.h"
#include <variant>
#include <iostream>

class EditorHelper
{
public:
	static EditorHelper& RetrieveEditorHelperInstance()
	{
		static EditorHelper m_EditorHelperInstance;
		return m_EditorHelperInstance;
	}
};

namespace ImGuiExtensions
{
	static const ImVec4 g_DefaultTint = { 255, 255, 255, 255 };

	inline void Image(const Amethyst::IconType iconType, const float iconSize)
	{
		ImGui::Image(
			(void*)(Amethyst::IconLibrary::RetrieveIconLibrary().RetrieveTextureByType(iconType)->RetrieveTextureID()),
			ImVec2(iconSize, iconSize),
			ImVec2(0, 0),
			ImVec2(1, 1),
			g_DefaultTint,
			ImColor(0, 0, 0, 0) //Border
		);
	}

	inline void Image(const Amethyst::IconType iconType, const float iconSizeX, const float iconSizeY)
	{
		ImGui::Image(
			(void*)(Amethyst::IconLibrary::RetrieveIconLibrary().RetrieveTextureByType(iconType)->RetrieveTextureID()),
			ImVec2(iconSizeX, iconSizeY),
			ImVec2(0, 0),
			ImVec2(1, 1),
			g_DefaultTint,
			ImColor(0, 0, 0, 0) //Border
		);
	}

	inline bool ImageButton(const Amethyst::IconType iconType, const float iconSize)
	{
		return ImGui::ImageButton(
			(void*)(Amethyst::IconLibrary::RetrieveIconLibrary().RetrieveTextureByType(iconType)->RetrieveTextureID()),
			ImVec2(iconSize, iconSize),
			ImVec2(0, 0),
			ImVec2(1, 1),
			-1,
			ImColor(0, 0, 0, 0),
			g_DefaultTint
		);
	}

	//Drag & Drop
	enum DragPayloadType
	{
		DragPayload_Unknown,
		DragPayload_Texture,
		DragPayload_Entity,
		DragPayload_Model,
		DragPayload_Audio,
		DragPayload_Script,
		DragPayload_Material
	};

	struct DragDropPayload
	{
		typedef std::variant<const char*, unsigned int> DataVariant; //Holds either a const char* or unsigned integer identifier.

		DragDropPayload(const DragPayloadType payloadType = DragPayload_Unknown, const DataVariant data = nullptr)
		{
			this->m_PayloadType = payloadType;
			this->m_Data = data;
		}

		DragPayloadType m_PayloadType;
		DataVariant m_Data;
	};

	inline void CreateDragPayload(const DragDropPayload& payload)
	{
		ImGui::SetDragDropPayload(reinterpret_cast<const char*>(&payload.m_PayloadType), reinterpret_cast<const void*>(&payload), sizeof(payload), ImGuiCond_Once);
	}

	inline DragDropPayload* ReceiveDragPayload(DragPayloadType payloadType)
	{
		if (ImGui::BeginDragDropTarget()) //Remember that BeginDragDropTarget() uses the last item in the window.
		{
			if (const auto payload = ImGui::AcceptDragDropPayload(reinterpret_cast<const char*>(&payloadType)))
			{
				std::cout << "Got Data!";
			}

			ImGui::EndDragDropTarget();
		}

		return nullptr;
	}
}