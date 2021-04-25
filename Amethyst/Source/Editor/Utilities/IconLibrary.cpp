#include "IconLibrary.h"
#include "../../Source/FileSystem.h"

namespace Amethyst
{
	static Icon g_NoIcon;

	IconLibrary::IconLibrary()
	{

	}

	IconLibrary::~IconLibrary()
	{
		/*
			If the only instance of a shared pointer is the instance in the vector, then erasing it out of the vector will lead to the destructor of the shared pointer instance 
			running. It will thus free the associated memory for the object the shared pointer owns. Otherwise, it will remove its reference count by 1.
		*/
		m_Icons.clear();
		m_Icons.shrink_to_fit();
	}

	void IconLibrary::InitializeIconLibrary()
	{
		//Console
		LoadIcon("Resources/Icons/Console_Info.png", IconType::Icon_Console_Info);
		LoadIcon("Resources/Icons/Console_Warning.png", IconType::Icon_Console_Warning);
		LoadIcon("Resources/Icons/Console_Error.png", IconType::Icon_Console_Error);

		//Toolbar
		LoadIcon("Resources/Icons/Toolbar_Play.png", IconType::Icon_Toolbar_Play);
		LoadIcon("Resources/Icons/Toolbar_Pause.png", IconType::Icon_Toolbar_Pause);
		LoadIcon("Resources/Icons/Toolbar_Stop.png", IconType::Icon_Toolbar_Stop);

		//Assets
		LoadIcon("Resources/Icons/Assets_Cube.png", IconType::Icon_ObjectPanel_Cube);
		LoadIcon("Resources/Icons/Wallpaper.png", IconType::Icon_Wallpaper);
	}

	RHI_Texture* IconLibrary::RetrieveTextureByType(IconType iconType)
	{
		return LoadIcon("", iconType).m_Texture.get();
	}

	RHI_Texture* IconLibrary::RetrieveTextureByFilePath(const std::string& filePath)
	{
		return LoadIcon(filePath).m_Texture.get();
	}

	RHI_Texture* IconLibrary::RetrieveTextureByIcon(const Icon& icon)
	{
		for (const Icon& storedIcon : m_Icons)
		{
			if (storedIcon.m_Texture->RetrieveTextureID() == icon.m_Texture->RetrieveTextureID())
			{
				return storedIcon.m_Texture.get();
			}
		}

		return nullptr;
	}

	const Icon& IconLibrary::LoadIcon(const std::string& filePath, IconType iconType, int iconSize)
	{
		//Check if we already have this thumbnail (by type).
		if (iconType != IconType::Icon_Custom)
		{
			for (Icon& icon : m_Icons)
			{
				if (icon.m_IconType == iconType)
				{
					return icon;
				}
			}
		}
		else //Check if we already have this thumbnail (by path).
		{
			for (Icon& icon : m_Icons)
			{
				if (icon.m_FilePath == filePath)
				{
					return icon;
				}
			}
		}

		//Texture
		if (FileSystem::IsSupportedImageFile(filePath) || FileSystem::IsEngineTextureFile(filePath))
		{
			//Make a cheap texture.
			bool generateMipmaps = false;
			std::shared_ptr<RHI_Texture> texture = std::make_shared<RHI_Texture>(filePath, GL_TEXTURE_2D, GL_RGBA);

			m_Icons.emplace_back(iconType, texture, filePath);
			return m_Icons.back(); //Returns the icon we just added.
		}

		return RetrieveIconByType(IconType::Icon_File_Default); //Can't find anything... We now desperately retrieve one.
	}

	const Icon& IconLibrary::RetrieveIconByType(IconType iconType)
	{
		for (Icon& icon : m_Icons)
		{
			if (icon.m_IconType == iconType)
			{
				return icon;
			}
		}

		return g_NoIcon;
	}
}