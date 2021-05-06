#include "Amethyst.h"
#include "Viewport.h"

namespace Amethyst
{
	Viewport::Viewport(Editor* editor) : Widget(editor)
	{
		m_WidgetName = "Viewport";
		m_WidgetSize = Math::Vector2(450, 250);
		m_WidgetFlags |= ImGuiWindowFlags_NoScrollbar;
		//m_WindowPadding = Math::Vector2(4.0f);

		//Context Class Creation.
	}

	void Viewport::OnVisibleTick()
	{

	}
}