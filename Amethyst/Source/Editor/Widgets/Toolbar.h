#pragma once
#include "../Widget.h"

namespace Amethyst
{
	/*
		The toolbar is to be ticked together with the Menu Bar.
	*/

	class Toolbar : public Widget
	{
	public:
		Toolbar(Editor* editorContext);

		void OnTickAlways() override;

	private:
		float m_ButtonSize = 25.0f;
	};
} 