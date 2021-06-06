#pragma once
#include <string>
#include "ImGui/Source/imgui.h"

class Editor;
class Context;

class Widget
{
public:
	Widget(Editor* editorContext);
	virtual ~Widget() = default;

	void OnUpdate(); // Called across all Widgets.

	virtual void OnTickAlways();  //Always called for all widgets, regardless of whether its a window or permanent asthetic.
	virtual void OnVisibleTick(); //Called only when the widget is visible.

	//Properties
	void SetWidgetVisibility(bool isVisible) { m_IsWidgetVisible = isVisible; }

protected:
	bool m_IsWindowedWidget = true; //All widgets are initialized to be capable of becoming individual windows unless its a permanent editor widget (such as the menu bar).
	bool m_IsWidgetVisible = true;
	std::string m_WidgetName = "Widget";
	Math::Vector2 m_WidgetSize = Math::Vector2(-1.0f);
	Math::Vector2 m_WidgetPadding = Math::Vector2(-1.0f);

	int m_WidgetFlags = ImGuiWindowFlags_NoCollapse; //Disable double clicking to collapse window.

	Context* m_Context = nullptr;
	Editor* m_EditorContext = nullptr;
};
