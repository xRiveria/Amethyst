#pragma once
#include "IconLibrary.h"
#include "EditorExtensions.h"
#include "../../Source/Core/FileSystem.h"
#include "../Source/Core/AmethystObject.h"
#include <chrono>
#include <functional>

namespace Amethyst
{
	enum FileDialog_Type 
	{
		FileDialog_Type_Browser,			//Standard docked asset browser.
		FileDialog_Type_FileSelection		//Standalone window for file selection.
	};

	enum FileDialog_Operation
	{
		FileDialog_Operation_Open,
		FileDialog_Operation_Load,
		FileDialog_Operation_Save
	};

	enum FileDialog_Filter 
	{
		FileDialog_Filter_All,
		FileDialog_Filter_Scene,
		FileDialog_Filter_Model
	};

	//Keeps track of directory navigation.
	class FileDialogNavigation
	{
	public:
		bool NavigateTo(std::string directory, bool updateHistory = true)
		{
			if (!FileSystem::IsDirectory(directory))
			{
				return false;
			}

			//If the directory ends with a slash, remove it to simplify operations below.
			if (directory.back() == '/')
			{
				directory = directory.substr(0, directory.size() - 1);
			}

			//Ensure we don't re-navigate.
			if (m_CurrentPath == directory)
			{
				return false;
			}

			//Update current path.
			m_CurrentPath = directory;

			//Update History
			if (updateHistory)
			{
				m_PathHistory.emplace_back(m_CurrentPath);
				m_PathHistoryIndex++;
			}

			m_PathHierarchy.clear();
			m_PathHierarchyLabels.clear();

			//Is there a slash?
			std::size_t slashPositionIndex = m_CurrentPath.find('/');

			//If there are no slashes, then there is no nesting, and we are done.
			if (slashPositionIndex == std::string::npos) //Or -1
			{
				m_PathHierarchy.emplace_back(m_CurrentPath);
			}
			else //If there is a slash, get the individual directories between slashes.
			{
				std::size_t previousPosition = 0;
				while (true)
				{
					//Save every path before the slash.
					m_PathHierarchy.emplace_back(m_CurrentPath.substr(0, slashPositionIndex));

					//Attempt to find a slash after the one we already found.
					previousPosition = slashPositionIndex;
					slashPositionIndex = m_CurrentPath.find('/', slashPositionIndex + 1);

					//If there are no more slashes...
					if (slashPositionIndex == std::string::npos)
					{
						//Save the complete path to this directory.
						m_PathHierarchy.emplace_back(m_CurrentPath);
						break;
					}
				}
			}

			//Create a proper looking label to show in the editor for each path. The label count will always follow the hierarchy count.
			for (const std::string& filePath : m_PathHierarchy)
			{
				slashPositionIndex = filePath.find('/');
				if (slashPositionIndex == std::string::npos)
				{
					m_PathHierarchyLabels.emplace_back(filePath + " >");
				}
				else
				{
					m_PathHierarchyLabels.emplace_back(filePath.substr(filePath.find_last_of('/') + 1) + " >");
				}
			}

			return true;
		}

		bool Backward()
		{
			if (m_PathHistory.empty() || (m_PathHistoryIndex - 1) < 0) //If no previous paths exist.
			{
				return false;
			}

			NavigateTo(m_PathHistory[--m_PathHistoryIndex], false);

			return true;
		}

		bool Forward()
		{
			if (m_PathHistory.empty() || (m_PathHistoryIndex + 1) >= static_cast<int>(m_PathHistory.size())) //If no next paths exist.
			{
				return false;
			}

			NavigateTo(m_PathHistory[++m_PathHistoryIndex], false);

			return true;
		}

	public:
		std::string m_CurrentPath;
		std::vector<std::string> m_PathHierarchy; //The list of paths to the current directory.
		std::vector<std::string> m_PathHierarchyLabels; //A label to show in the hierarchy for each path.
		std::vector<std::string> m_PathHistory; 
		int m_PathHistoryIndex = -1; //An index for each path saved in the history.
	};

	class FileDialogItem
	{
	public:
		FileDialogItem(const std::string& filePath, const Icon& itemIcon)
		{
			this->m_FilePath = filePath;
			this->m_Icon = itemIcon;
			this->m_ID = AmethystObject::GenerateObjectID();
			this->m_IsDirectory = FileSystem::IsDirectory(filePath);
			this->m_HierarchyLabel = FileSystem::RetrieveFileNameFromFilePath(filePath);
		}

		const std::string& RetrieveFilePath() const { return m_FilePath; }
		const std::string& RetrieveHierarchyLabel() const { return m_HierarchyLabel; }
		Amethyst::RHI_Texture* RetrieveItemTexture() const { return IconLibrary::RetrieveIconLibrary().RetrieveTextureByIcon(m_Icon); }
		unsigned int RetrieveItemID() const { return m_ID; }
		bool IsDirectory() const { return m_IsDirectory; }
		float RetrieveTimeSinceLastClickInMilliseconds() const { return static_cast<float>(m_TimeSinceLastClick.count()); }

		void Clicked()
		{
			const std::chrono::time_point currentTimePoint = std::chrono::high_resolution_clock::now();
			m_TimeSinceLastClick = currentTimePoint - m_LastClickTime;
			m_LastClickTime = currentTimePoint;
		}

	private:
		unsigned int m_ID;
		Icon m_Icon;
		std::string m_FilePath;
		std::string m_HierarchyLabel;
		bool m_IsDirectory;

		std::chrono::duration<double, std::milli> m_TimeSinceLastClick;
		std::chrono::time_point<std::chrono::high_resolution_clock> m_LastClickTime;	
	};

	class FileDialog
	{
	public:
		FileDialog(bool isStandaloneWindow, FileDialog_Type type, FileDialog_Operation operation, FileDialog_Filter filter);

		//Type & Filter
		FileDialog_Type RetrieveType() const { return m_Type; }
		FileDialog_Filter RetrieveFilter() const { return m_Filter; }

		//Operation
		FileDialog_Operation RetrieveOperation() const { return m_Operation; }
		void SetOperation(FileDialog_Operation operation);

		//Master that encompasses ShowTop, Middle and Bottom UI. Displays the main dialog and returns true if a selection was made.
		bool ShowDialog(bool* isVisible, std::string* directory = nullptr, std::string* filePath = nullptr);

		//Callbacks
		void SetCallbackOnItemClicked(const std::function<void(const std::string&)>& callback) { m_OnItemClickedCallback = callback; }
		void SetCallbackOnItemDoubleClicked(const std::function<void(const std::string&)>& callback) { m_OnItemDoubleClickedCallback = callback; }

	private:
		void ShowDirectoriesUI();
		void ShowTopUI(bool* isVisible); //Top Menu
		void ShowMiddleUI(); //Contents of the current directory.
		void ShowBottomUI(bool* isVisible); //Bottom Menu

		//Item Functionality Handling
		void OnItemDrag(FileDialogItem* dialogItem) const;
		void OnItemClick(FileDialogItem* dialogItem) const;
		void ItemContextMenu(FileDialogItem* dialogItem); //Upon right clicking on any item in the asset browser...

		//Misc
		bool DialogUpdateFromDirectory(const std::string& directoryPath); //Update the entire dialog based on the directory path.
		void AssetBrowserContextMenu(); //Upon right clicking on any empty space within the asset browser...

	private:
		//Options
		const float m_DialogItemMinimumSize = 50.0f;
		const float m_DialogItemMaximumSize = 200.0f;
		const glm::vec4 m_ContentBackgroundColor = { 0.0f, 0.0f, 0.0f, 50.0f };

		//Flags
		bool m_IsWindow;
		bool m_SelectionMade;
		bool m_IsDirty;
		bool m_IsHoveringItem;
		bool m_IsHoveringWindow;
		std::string m_OperationName;
		FileDialogNavigation m_Navigation;
		std::string m_InputBox;
		std::string m_HoveredItemPath;
		uint32_t m_DisplayedItemCount;

		//Internal
		mutable unsigned int m_ItemContextMenuID; //Populates with each dialog item's ID to enable popups to be unique (prevents 1 edit from affecting all dialog items).
		mutable ImGuiExtensions::DragDropPayload m_DragDropPayload;
		float m_BottomOffset = 0.0f;
		FileDialog_Type m_Type;
		FileDialog_Operation m_Operation;
		FileDialog_Filter m_Filter;
		std::vector<FileDialogItem> m_DialogItems;
		glm::vec2 m_DialogItemSize;
		ImGuiTextFilter m_SearchFilter;

		//Callbacks
		std::function<void(const std::string&)> m_OnItemClickedCallback;
		std::function<void(const std::string&)> m_OnItemDoubleClickedCallback;
	};
}