#pragma once
#include "../Widget.h"

namespace Amethyst
{
	class QuickDiagnostics : public Widget
	{
	public:
		QuickDiagnostics(Editor* editor);
		void OnVisibleTick() override;
	};
}