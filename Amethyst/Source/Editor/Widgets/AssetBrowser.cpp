#include "AssetBrowser.h"

namespace Amethyst
{
	AssetBrowser::AssetBrowser(Editor* editor) : Widget(editor)
	{
		m_WidgetName = "Asset Browser";

		m_WidgetFlags |= ImGuiWindowFlags_NoScrollbar;

		//Just clicked, not selected.
	}

	void AssetBrowser::OnVisibleTick()
	{
	}

	void AssetBrowser::OnPathClicked(const std::string& filePath) const
	{
	}
}