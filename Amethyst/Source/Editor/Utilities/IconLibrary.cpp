#include "Amethyst.h"
#include "IconLibrary.h"
#include "../../Source/Core/FileSystem.h"

static Icon g_NoIcon;

IconLibrary::IconLibrary()
{
	m_EngineContext = nullptr;
}

IconLibrary::~IconLibrary()
{
	/*
		If the only instance of a shared pointer is the instance in the vector, then erasing it out of the vector will lead to the destructor of the shared pointer instance 
		running. It will thus free the associated memory for the object the shared pointer owns. Otherwise, it will remove its reference count by 1.
	*/
	m_Icons.clear();
}

void IconLibrary::Initialize(Amethyst::Context* engineContext)
{
	m_EngineContext = engineContext;
	/// Retrieve resource directory from a Resource Cache?

	//Console
	LoadIcon_("Resources/Icons/Console_Info.png", IconType::Icon_Console_Info);
	LoadIcon_("Resources/Icons/Console_Warning.png", IconType::Icon_Console_Warning);
	LoadIcon_("Resources/Icons/Console_Error.png", IconType::Icon_Console_Error);

	//Toolbar
	LoadIcon_("Resources/Icons/Toolbar_Play.png", IconType::Icon_Toolbar_Play);
	LoadIcon_("Resources/Icons/Toolbar_Pause.png", IconType::Icon_Toolbar_Pause);
	LoadIcon_("Resources/Icons/Toolbar_Stop.png", IconType::Icon_Toolbar_Stop);

	//Assets
	LoadIcon_("Resources/Icons/Assets_Cube.png", IconType::Icon_ObjectPanel_Cube);
	LoadIcon_("Resources/Icons/Wallpaper.png", IconType::Icon_Wallpaper);
	
	//Asset Browser
	LoadIcon_("Resources/Icons/AssetBrowser_Folder.png", IconType::Icon_AssetBrowser_Folder);
	LoadIcon_("Resources/Icons/AssetBrowser_Script.png", IconType::Icon_AssetBrowser_Script);
	
	//Misc
	LoadIcon_("Resources/Icons/AssetBrowser_Unknown.png", IconType::Icon_Custom);
}			

Amethyst::RHI_Texture* IconLibrary::RetrieveTextureByType(IconType iconType)
{
	return LoadIcon_("", iconType).m_Texture.get();
}

Amethyst::RHI_Texture* IconLibrary::RetrieveTextureByFilePath(const std::string& filePath)
{
	return LoadIcon_(filePath).m_Texture.get();
}

Amethyst::RHI_Texture* IconLibrary::RetrieveTextureByIcon(const Icon& icon)
{
	for (const Icon& storedIcon : m_Icons)
	{
		if (storedIcon.m_Texture->RetrieveLoadState() != Amethyst::LoadState::Completed)
		{
			continue;
		}

		if (storedIcon.m_Texture->RetrieveObjectID() == icon.m_Texture->RetrieveObjectID())
		{
			return storedIcon.m_Texture.get();
		}
	}

	return nullptr;
}

const Icon& IconLibrary::LoadIcon_(const std::string& filePath, IconType iconType, int iconSize)
{
	// Check if we already have this thumbnail (by type).
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
	else // Check if we already have this thumbnail (by path).
	{
		for (Icon& icon : m_Icons)
		{
			if (icon.m_FilePath == filePath)
			{
				return icon;
			}
		}
	}

	// Deduce file path type

	if (Amethyst::FileSystem::IsDirectory(filePath))                            return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Model																 
	if (Amethyst::FileSystem::IsSupportedModelFile(filePath))                   return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Audio																 
	if (Amethyst::FileSystem::IsSupportedAudioFile(filePath))                   return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Material																 
	if (Amethyst::FileSystem::IsEngineMaterialFile(filePath))                   return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Shader																 
	if (Amethyst::FileSystem::IsSupportedShaderFile(filePath))                  return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Scene																 
	if (Amethyst::FileSystem::IsEngineSceneFile(filePath))                      return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Script																 
	if (Amethyst::FileSystem::IsEngineScriptFile(filePath))                     return RetrieveIconByType(IconType::Icon_AssetBrowser_Script);
	// Font																	 
	if (Amethyst::FileSystem::IsSupportedFontFile(filePath))                    return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
																			 
	// Xml
	if (Amethyst::FileSystem::RetrieveExtensionFromFilePath(filePath) == ".xml")    return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Dll																		 
	if (Amethyst::FileSystem::RetrieveExtensionFromFilePath(filePath) == ".dll")    return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Txt																		
	if (Amethyst::FileSystem::RetrieveExtensionFromFilePath(filePath) == ".txt")    return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Ini																		
	if (Amethyst::FileSystem::RetrieveExtensionFromFilePath(filePath) == ".ini")    return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);
	// Exe																		
	if (Amethyst::FileSystem::RetrieveExtensionFromFilePath(filePath) == ".exe")    return RetrieveIconByType(IconType::Icon_AssetBrowser_Folder);

	//Texture
	if (Amethyst::FileSystem::IsSupportedImageFile(filePath) || Amethyst::FileSystem::IsEngineTextureFile(filePath))
	{
		// Make a cheap texture.
		bool shouldGenerateMipmaps = false;
		std::shared_ptr<Amethyst::RHI_Texture2D> texture = std::make_shared<Amethyst::RHI_Texture2D>(m_EngineContext, shouldGenerateMipmaps);
		texture->SetWidth(iconSize);
		texture->SetHeight(iconSize);

		// Load it asynchronously.
		m_EngineContext->RetrieveSubsystem<Amethyst::Threading>()->AddTask([texture, filePath]()
		{
			texture->LoadFromFile(filePath); ///
		});

		m_Icons.emplace_back(iconType, texture, filePath);
		return m_Icons.back();
	}

	return RetrieveIconByType(IconType::Icon_Custom); //Can't find anything... We now desperately retrieve the default one.
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
