#pragma once
#include <string>

namespace Amethyst
{
	class Widget
	{
	public:
		Widget();

		void OnUpdate(); //Called across all Widgets.

		virtual void OnTickAlways();  //Always called.
		virtual void OnVisibleTick(); //Called only when the widget is visible.

	protected:
		std::string m_WidgetName = "Widget";
	};
}