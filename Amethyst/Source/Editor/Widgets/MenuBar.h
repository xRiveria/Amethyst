#pragma once
#include "../Widget.h"
#include <memory>

namespace Amethyst
{
	class MenuBar : public Widget
	{
	public:
		MenuBar(Editor* editorContext);

		void OnTickAlways() override;
	};
}