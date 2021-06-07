#pragma once
#include "../Widget.h"
#include "../Utilities/EditorExtensions.h"

class ObjectsPanel : public Widget
{
public:
	ObjectsPanel(Editor* editorContext);

	void OnVisibleTick() override;

	//Objects Panel Specific
	void ObjectDrag(const std::string& objectType) const;

private:
	float m_IconSize = 20.0f;

	//Internal
	mutable ImGuiExtensions::DragDropPayload m_DragDropPayload; //Currently operating payload.
};
