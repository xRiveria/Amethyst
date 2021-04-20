#pragma once
#include <string>
#include "ImGui/imgui.h"

namespace Amethyst
{
	class Editor;

	class Widget
	{
	public:
		Widget(Editor* editorContext);
		virtual ~Widget() = default;

		void OnUpdate(); //Called across all Widgets.

		virtual void OnTickAlways();  //Always called for all widgets, regardless of whether its a window or permanent asthetic.
		virtual void OnVisibleTick(); //Called only when the widget is visible.

	protected:
		bool m_IsWindowedWidget = true; //All widgets are initialized to be capable of becoming individual windows unless its a permanent editor widget (such as the menu bar).
		bool m_IsWidgetVisible = true;
		std::string m_WidgetName = "Widget";
		int m_WidgetFlags = ImGuiWindowFlags_NoCollapse; //Disable double clicking to collapse window.

		Editor* m_EditorContext = nullptr;
	};
};