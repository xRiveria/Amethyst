#pragma once
#include "../Widget.h"
#include <memory>

namespace Amethyst
{
	class FileDialog;

	class AssetBrowser : public Widget
	{
	public:
		AssetBrowser(Editor* editor);

		void OnVisibleTick() override;

	private:
		void OnPathClicked(const std::string& filePath) const;

		std::unique_ptr<FileDialog> m_FileDialogView;
		std::unique_ptr<FileDialog> m_FileDialogLoad;
	};
}