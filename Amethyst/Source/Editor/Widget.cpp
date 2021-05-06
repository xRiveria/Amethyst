#include "Amethyst.h"
#include "Widget.h"

namespace Amethyst
{
	Widget::Widget(Editor* editorContext) : m_EditorContext(editorContext)
	{

	}

	void Widget::OnUpdate()
	{
		OnTickAlways();

		if (!m_IsWindowedWidget)
		{
			return;
		}

		if (!m_IsWidgetVisible) { return; }

			
		//Begin
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
}