#pragma once
#include "ImGui/imgui.h"
#include "IconLibrary.h"
#include "ImGui/imgui_internal.h"

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
}