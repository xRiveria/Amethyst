#pragma once
#include "../Widget.h"

class QuickDiagnostics : public Widget
{
public:
	QuickDiagnostics(Editor* editor);
	void OnVisibleTick() override;
};
