#pragma once
#include <string>
#include <vector>
#include <memory>
#include "../../RHI/RHI_Texture2D.h"
#include "../Source/Threading/Threading.h"

class RHI_Texture;

enum IconType
{
	Icon_NotAssigned,
	Icon_Custom,
	Icon_Console_Info,
	Icon_Console_Warning,
	Icon_Console_Error,
	Icon_AssetBrowser_Folder,
	Icon_AssetBrowser_Script,
	Icon_Toolbar_Play,
	Icon_Toolbar_Pause,
	Icon_Toolbar_Stop,
	Icon_ObjectPanel_Cube,
	Icon_Wallpaper,
	Icon_File_Default
};

namespace Amethyst { class Context; }

struct Icon
{
	Icon() = default;
	Icon(IconType iconType, std::shared_ptr<Amethyst::RHI_Texture> texture, const std::string& texturePath)
	{
		this->m_IconType = iconType;
		this->m_Texture = texture;
		this->m_FilePath = texturePath;
	}

	IconType m_IconType = IconType::Icon_NotAssigned;
	std::shared_ptr<Amethyst::RHI_Texture> m_Texture;
	std::string m_FilePath;
};

class IconLibrary
{
public:
	static IconLibrary& RetrieveIconLibraryInstance()
	{
		static IconLibrary m_LibraryInstance;
		return m_LibraryInstance;
	}

	IconLibrary();
	~IconLibrary();

	void Initialize(Amethyst::Context* engineContext);

	Amethyst::RHI_Texture* RetrieveTextureByType(IconType iconType);
	Amethyst::RHI_Texture* RetrieveTextureByFilePath(const std::string& filePath);
	Amethyst::RHI_Texture* RetrieveTextureByIcon(const Icon& icon);
	const Icon& LoadIcon_(const std::string& filePath, IconType iconType = IconType::Icon_Custom, int iconSize = 100);

private:
	const Icon& RetrieveIconByType(IconType iconType);

private:
	std::vector<Icon> m_Icons;
	Amethyst::Context* m_EngineContext;
};
