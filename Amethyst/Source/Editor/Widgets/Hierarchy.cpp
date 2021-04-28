#include "Hierarchy.h"

namespace Amethyst
{
	Hierarchy::Hierarchy(Editor* editor) : Widget(editor)
	{
		m_WidgetName = "Hierarchy";
		m_WidgetFlags |= ImGuiWindowFlags_HorizontalScrollbar;


	}

	void Hierarchy::OnVisibleTick()
	{
		ShowTree();
	}

	void Hierarchy::ShowTree()
	{
		OnTreeBegin();

		if (ImGui::TreeNodeEx("Root", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
		{
			

			ImGui::TreePop();
		}

		OnTreeEnd();
	}

	void Hierarchy::OnTreeBegin()
	{
	}

	void Hierarchy::OnTreeEnd()
	{
	}

	void Hierarchy::AddTreeEntity()
	{
	}
}