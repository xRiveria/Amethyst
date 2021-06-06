#include "Amethyst.h"
#include "AssetBrowser.h"

namespace AssetBrowserStatics
{
	static bool g_ShowFileDialogView = true;
	static bool g_ShowFileDialogLoad = false;
}

AssetBrowser::AssetBrowser(Editor* editor) : Widget(editor)
{
	m_WidgetName = "Asset Browser";
	m_FileDialogView = std::make_unique<FileDialog>(false, FileDialog_Type_Browser, FileDialog_Operation_Load, FileDialog_Filter_All);
	m_FileDialogLoad = std::make_unique<FileDialog>(true, FileDialog_Type_FileSelection, FileDialog_Operation_Load, FileDialog_Filter_Model);
	m_WidgetFlags |= ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse;

	//Just clicked, not selected.
	m_FileDialogView->SetCallbackOnItemClicked([this](const std::string& string) { OnPathClicked(string); });
}

void AssetBrowser::OnVisibleTick()
{

	//View
	m_FileDialogView->ShowDialog(&AssetBrowserStatics::g_ShowFileDialogView);

	//Import

}

void AssetBrowser::OnPathClicked(const std::string& filePath) const
{
}
