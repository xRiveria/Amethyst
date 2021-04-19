#include "Widget.h"
#include "ImGui/imgui.h"

namespace Amethyst
{
	Widget::Widget()
	{

	}

	void Widget::OnUpdate()
	{
		OnTickAlways();

		ImGui::Begin(m_WidgetName.c_str());

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