#pragma once
#include <string>
#include <vector>
#include "../../RHI/RHI_Texture.h"

namespace Amethyst
{
	enum IconType
	{
		Icon_NotAssigned,
		Icon_Custom,
		Icon_Console_Info,
		Icon_Console_Warning,
		Icon_Console_Error,
		Icon_Toolbar_Play,
		Icon_Toolbar_Pause,
		Icon_Toolbar_Stop
	};

	struct Icon
	{

	};

	class IconLibrary
	{
	public:
		static IconLibrary& RetrieveIconLibrary()
		{
			static IconLibrary m_LibraryInstance;
			return m_LibraryInstance;
		}

		IconLibrary();
		~IconLibrary();

		void InitializeIconLibrary();

		RHI_Texture* RetrieveTextureByType(IconType iconType);
		RHI_Texture* RetrieveTextureByFilePath(const std::string& filePath);
		RHI_Texture* RetrieveTextureByIcon(const Icon& icon);
		const Icon& LoadIcon(const std::string& filePath, IconType iconType = IconType::Icon_Custom, int iconSize = 100);

	private:
		const Icon& RetrieveIconByType(IconType iconType);
		std::vector<Icon> m_Icons;
	};
}