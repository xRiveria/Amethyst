#include "Amethyst.h"
#include "Widget.h"
#include "Editor.h"

Widget::Widget(Editor* editorContext) : m_EditorContext(editorContext)
{
	//m_Context = m_EditorContext->RetrieveContext();
}

void Widget::OnUpdate()
{
	// Constant Widgets - Has its own ImGui:Begin and ImGui::End. Self contained and is always rendering.
	OnTickAlways(); 

	if (!m_IsWindowedWidget)
	{
		return;
	}

	if (!m_IsWidgetVisible) { return; }

		
	// Individual Widgets - Can be closed or open depending on the user. These widgets's ImGui::Begin and ImGui::End are nested here in the parent Widget class.
	ImGui::Begin(m_WidgetName.c_str(), &m_IsWidgetVisible, m_WidgetFlags);
	OnVisibleTick();
	ImGui::End();
}

void Widget::OnTickAlways()
{

}

void Widget::OnVisibleTick()
{

}
