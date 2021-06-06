#pragma once
#include "../Widget.h"
#include <memory>

class MenuBar : public Widget
{
public:
	MenuBar(Editor* editorContext);

	void OnTickAlways() override;
};
