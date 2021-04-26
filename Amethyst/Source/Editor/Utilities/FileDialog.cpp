#include "FileDialog.h"

namespace Amethyst
{

#define OPERATION_NAME (m_Operation == FileDialog_Operation_Open) ? "Open" : (m_Operation == FileDialog_Operation_Load) ? "Load" : (m_Operation == FileDialog_Operation_Save) ? "Save" : "View"
#define FILTER_NAME (m_Filter == FileDialog_Filter_All) ? "All (*.*)" : (m_Filter == FileDialog_Filter_Model) ? "Model (*.*)" : "World (*.world)"

	FileDialog::FileDialog(bool isStandaloneWindow, FileDialog_Type type, FileDialog_Operation operation, FileDialog_Filter filter)
	{
		m_Type = type;
		m_Filter = filter;
		m_Operation = operation;
		m_OperationName = OPERATION_NAME;
		m_IsWindow = isStandaloneWindow;
		m_DialogItemSize = { 90.0f, 90.0f }; //Default Size
		m_IsDirty = true;
		m_SelectionMade = false;

		m_OnItemClickedCallback = nullptr;
		m_OnItemDoubleClickedCallback = nullptr;

		m_Navigation.NavigateTo(FileSystem::RetrieveWorkingDirectory());
	}

	void FileDialog::SetOperation(FileDialog_Operation operation)
	{
		m_Operation = operation;
		m_OperationName = OPERATION_NAME;
	}

	bool FileDialog::ShowDialog(bool* isVisible, std::string* directory, std::string* filePath)
	{
		if (!(*isVisible)) //If the dialog isn't visible...
		{
			m_IsDirty = true; //Set as dirty as things can change until the next time the dialog becomes visible.
			return false;
		}

		m_SelectionMade = false;
		m_IsHoveringItem = false;
		m_IsHoveringWindow = false;

		ShowDirectoriesUI();
		ImGui::SameLine();
		ImGui::BeginChild("##Hello");

		ShowTopUI(isVisible); //Top Menu
		ShowMiddleUI(); //Contents of the current directory.
		ShowBottomUI(isVisible); //Bottom Menu

		ImGui::EndChild();
		

		if (m_IsWindow) //If we are in loading mode...
		{
			ImGui::End(); //Ends our UI sequences above.
		}

		if (m_IsDirty) //If changes were detected...
		{
			DialogUpdateFromDirectory(m_Navigation.m_CurrentPath);
			m_IsDirty = false;
		}

		if (m_SelectionMade)
		{
			if (directory)
			{
				(*directory) = m_Navigation.m_CurrentPath; ///
			}

			if (filePath)
			{
				(*filePath) = m_Navigation.m_CurrentPath + "/" + std::string(m_InputBox); ///
			}
		}

		AssetBrowserContextMenu();

		return m_SelectionMade; 
	}

	void FileDialog::ShowDirectoriesUI()
	{
		//Remove Border
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);
		ImGui::BeginChild("Directories##1", ImVec2(200, 300), true, ImGuiWindowFlags_HorizontalScrollbar);

		if (ImGui::CollapsingHeader(FileSystem::RetrieveFileNameFromFilePath(m_Navigation.m_CurrentPath).c_str(), ImGuiTreeNodeFlags_DefaultOpen))
		{
			for (int i = 0; i < FileSystem::RetrieveDirectoriesInDirectory(m_Navigation.m_CurrentPath).size(); i++)
			{
				std::string fileDirectory = FileSystem::RetrieveDirectoriesInDirectory(m_Navigation.m_CurrentPath)[i];
				if (ImGui::TreeNodeEx(FileSystem::RetrieveFileNameFromFilePath(fileDirectory).c_str()))
				{
					ImGui::TreePop();
				}
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();

		ImGui::SameLine(); //Our seperator.
		ImGui::SeparatorEx(ImGuiSeparatorFlags_Vertical);
		ImGui::SameLine(); //Our asset browser.
	}

	void FileDialog::ShowTopUI(bool* isVisible)
	{
		if (m_IsWindow) //If we are in loading mode, we open a new window.
		{
			ImGui::SetNextWindowSize(ImVec2(400.0f, 400.0f), ImGuiCond_FirstUseEver);
			ImGui::SetNextWindowSizeConstraints(ImVec2(350.0f, 250.0f), ImVec2(FLT_MAX, FLT_MAX));
			ImGui::Begin(m_OperationName.c_str(), isVisible, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoDocking);
			ImGui::SetWindowFocus();
		}

		//Directory Navigation Buttons
		{
			//Backwards
			if (ImGui::Button("<"))
			{
				m_IsDirty = m_Navigation.Backward(); //If going backwards a directory isn't possible, return false, else set to true and update file dialog.
			}

			//Forward
			ImGui::SameLine();
			if (ImGui::Button(">"))
			{
				m_IsDirty = m_Navigation.Forward(); 
			}

			/*
			//Individual Directory Buttons
			for (uint32_t i = 0; i < m_Navigation.m_PathHierarchy.size(); i++)
			{
				ImGui::SameLine();
				if (ImGui::Button(m_Navigation.m_PathHierarchyLabels[i].c_str()))
				{
					m_IsDirty = m_Navigation.NavigateTo(m_Navigation.m_PathHierarchy[i]);
				}
			}
			*/
		}
		
		//Search Filter
		ImGui::SameLine();
		const float labelWidth = 170.0f;

		m_SearchFilter.Draw("##SearchBar", labelWidth);

		ImGui::SameLine();
		ImGui::Text(FileSystem::RetrieveDirectoryFromFilePath(m_Navigation.m_CurrentPath).c_str());

		ImGui::Separator();
	}

	void FileDialog::ShowMiddleUI()
	{
		//Compute Some Stuff
		const ImGuiWindow* window = ImGui::GetCurrentWindowRead();
		const float contentWidth = ImGui::GetContentRegionAvail().x;
		const float contentHeight = ImGui::GetContentRegionAvail().y - m_BottomOffset;
		ImGuiContext& context = *GImGui;
		ImGuiStyle& style = ImGui::GetStyle();
		const float fontSize = context.FontSize;
		const float labelSize = fontSize;
		const float textOffset = 3.0f;
		bool newLine = true;
		m_DisplayedItemCount = 0;
		ImRect rectButton;
		ImRect rectLabel;
		float penXMinimum = 0.0f;
		float penX = 0.0f;

		//Remove Border
		ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 0.0f);

		if (ImGui::BeginChild("##ContentRegion", ImVec2(contentWidth, contentHeight), true))
		{
			//At any point if the user hovers over the window space, we set our value to true, else, it keeps the previous value whether it was already true or false.
			m_IsHoveringWindow = ImGui::IsWindowHovered(ImGuiHoveredFlags_AllowWhenBlockedByPopup | ImGuiHoveredFlags_AllowWhenBlockedByActiveItem) ? true : m_IsHoveringWindow;

			//Set starting position for our items.
			float offset = ImGui::GetStyle().ItemSpacing.x;
			penXMinimum = ImGui::GetCursorPosX() + offset;
			ImGui::SetCursorPosX(penXMinimum);

			//Go through all the items.
			for (int i = 0; i < m_DialogItems.size(); i++)
			{
				//Retrieve the item to be displayed.
				FileDialogItem& item = m_DialogItems[i];

				//Apply search filter.
				if (!m_SearchFilter.PassFilter(item.RetrieveHierarchyLabel().c_str()))
				{
					continue; //Jumps to the next item to render in the asset browser if it doesn't pass our filter.
				}

				//At this point, we are sure to load the item.
				m_DisplayedItemCount++;

				//Do we start a new line?
				if (newLine)
				{
					ImGui::BeginGroup();
					newLine = false;
				}

				ImGui::BeginGroup(); //Group that makes up each of our item.
				{
					//Compute rectangles for elements that make up an item.
					{
						//Our icon button.
						rectButton = ImRect(
							ImGui::GetCursorScreenPos().x,
							ImGui::GetCursorScreenPos().y,
							ImGui::GetCursorScreenPos().x + m_DialogItemSize.x,
							ImGui::GetCursorScreenPos().y + m_DialogItemSize.y
						);

						//Our icon label.
						rectLabel = ImRect
						{
							rectButton.Min.x,
							rectButton.Max.y - labelSize - style.FramePadding.y,
							rectButton.Max.x,
							rectButton.Max.y
						};
					}

					//Icon
					{
						ImGui::PushID(i);
						//ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0, 0, 0, 0));
						ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));

						if (ImGui::Button("##Dummy", m_DialogItemSize))
						{
							//Determine type of Click.
							item.Clicked();
							const bool isSingleClick = item.RetrieveTimeSinceLastClickInMilliseconds() > 500; //If the time since last click is over 500 milliseconds.

							if (isSingleClick)
							{
								//Updated input box.
								m_InputBox = item.RetrieveHierarchyLabel();
								//Callback
								if (m_OnItemClickedCallback)
								{
									m_OnItemClickedCallback(item.RetrieveFilePath());
								}
							}
							else //Double Click
							{
								m_IsDirty = m_Navigation.NavigateTo(item.RetrieveFilePath());
								m_SelectionMade = !item.IsDirectory();

								//When in asset browsing mode, open them on double click.
								if (m_Type == FileDialog_Type_Browser)
								{
									if (!item.IsDirectory())
									{
										FileSystem::OpenDirectoryWindow(item.RetrieveFilePath());
									}
								}

								//Callback
								if (m_OnItemDoubleClickedCallback)
								{
									m_OnItemDoubleClickedCallback(m_Navigation.m_CurrentPath);
								}
							}
						}

						//Item Functionality
						{
							//Manually detect some useful states.
							if (ImGui::IsItemHovered(ImGuiHoveredFlags_RectOnly))
							{
								m_IsHoveringItem = true;
								m_HoveredItemPath = item.RetrieveFilePath();
							}

							OnItemClick(&item);
							ItemContextMenu(&item);
							OnItemDrag(&item);
						}

						//Image
						{
							//Compute Thumbnail Size
							ImVec2 imageSizeMax = ImVec2(rectButton.Max.x - rectButton.Min.x - style.FramePadding.x * 2.0f, rectButton.Max.y - rectButton.Min.y - style.FramePadding.y - labelSize - 5.0f);
							ImVec2 imageSize = item.RetrieveItemTexture() ? ImVec2(static_cast<float>(item.RetrieveItemTexture()->RetrieveTextureWidth()), static_cast<float>(item.RetrieveItemTexture()->RetrieveTextureHeight())) : imageSizeMax;
							ImVec2 imageSizeDelta = ImVec2(0.0f, 0.0f);

							//Scale the image size to fit the max avaliable size while respecting its aspect ratio.
							{
								//Clamp Width
								if (imageSize.x != imageSizeMax.x)
								{
									float scale = imageSizeMax.x / imageSize.x;
									imageSize.x = imageSizeMax.x;
									imageSize.y = imageSize.y * scale;
								}

								//Clamp Height
								if (imageSize.y != imageSizeMax.y)
								{
									float scale = imageSizeMax.y / imageSize.y;
									imageSize.y = imageSizeMax.y;
									imageSize.x = imageSize.x * scale;
								}

								imageSizeDelta.x = imageSizeMax.x - imageSize.x;
								imageSizeDelta.y = imageSizeMax.y - imageSize.y;
							}

							ImGui::SetCursorScreenPos(ImVec2(rectButton.Min.x + style.FramePadding.x + imageSizeDelta.x * 0.5f, rectButton.Min.y + style.FramePadding.y + imageSizeDelta.y * 0.5f));
							ImGui::Image((void*)item.RetrieveItemTexture()->RetrieveTextureID(), imageSize);
						}

						ImGui::PopStyleColor(1);
						ImGui::PopID();
					}

					//Label
					{
						const char* labelText = item.RetrieveHierarchyLabel().c_str();
						const ImVec2 labelSize = ImGui::CalcTextSize(labelText, nullptr, true);

						//Draw Text

						ImGui::SetCursorScreenPos(ImVec2((rectLabel.Min.x + textOffset), rectLabel.Min.y + textOffset));
						if (labelSize.x <= m_DialogItemSize.x && labelSize.y <= m_DialogItemSize.y)
						{
							ImGui::TextUnformatted(labelText);
						}
						else
						{
							ImGui::RenderTextClipped(rectLabel.Min, rectLabel.Max, labelText, nullptr, &labelSize, ImVec2(0, 0), &rectLabel);
						}
					}

					ImGui::EndGroup();
				}

				//Decide whether we should switch to the next column or row.
				penX += m_DialogItemSize.x + ImGui::GetStyle().ItemSpacing.x;
				if (penX >= contentWidth - m_DialogItemSize.x)
				{
					ImGui::EndGroup();
					penX = penXMinimum;
					ImGui::SetCursorPosX(penX);
					newLine = true;
				}
				else
				{
					ImGui::SameLine();
				}
			}

			if (!newLine)
			{
				ImGui::EndGroup();
			}
		}

		ImGui::EndChild();
		ImGui::PopStyleVar();
	}

	void FileDialog::ShowBottomUI(bool* isVisible)
	{
		const auto SizeSlider = [this]()
		{
			//Size Slider
			const float sliderWidth = 150.0f;
			ImGui::SameLine(ImGui::GetWindowContentRegionWidth() - sliderWidth);
			ImGui::PushItemWidth(sliderWidth);

			const float previousWidth = m_DialogItemSize.x;
			ImGui::SetCursorPosY(ImGui::GetWindowSize().y - m_BottomOffset);
			ImGui::SliderFloat("##FileDialogSlider", &m_DialogItemSize.x, m_DialogItemMinimumSize, m_DialogItemMaximumSize, "%.4g"); //Scales the width of our dialog items accordingly.
			m_DialogItemSize.y += m_DialogItemSize.x - previousWidth; //We will scale our item height as well.

			ImGui::PopItemWidth();
		};

		if (m_Type == FileDialog_Type_Browser) //Asset Browser
		{
			//Move to the bottom of the window.
			m_BottomOffset = 30.0f;
			ImGui::SetCursorPosY((ImGui::GetWindowSize().y - m_BottomOffset) + 10.0f);

			const char* text = (m_DisplayedItemCount == 1) ? "%d Item" : "%d Items";
			ImGui::Text(text, m_DisplayedItemCount);

			SizeSlider();
		}
		else //File Selection
		{
			//Move to the bottom of the window.
			m_BottomOffset = 35.0f;
			ImGui::SetCursorPosY(ImGui::GetWindowSize().y - m_BottomOffset);

			ImGui::PushItemWidth(ImGui::GetWindowSize().x - 235);
			//ImGui::InputText("##InputBox", );
			ImGui::PopItemWidth();

			ImGui::SameLine();
			ImGui::Text(FILTER_NAME);

			ImGui::SameLine();
			if (ImGui::Button(OPERATION_NAME))
			{
				m_SelectionMade = true;
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel"))
			{
				m_SelectionMade = false;
				(*isVisible) = false;
			}

			SizeSlider();
		}
	}

	void FileDialog::OnItemDrag(FileDialogItem* dialogItem) const
	{
	}

	void FileDialog::OnItemClick(FileDialogItem* dialogItem) const
	{
		if (!dialogItem || !m_IsHoveringItem)
		{
			return;
		}

		//Item context menu on right click.
		if (ImGui::IsItemClicked(1))
		{
			m_ItemContextMenuID = dialogItem->RetrieveItemID();
			ImGui::OpenPopup("##AssetBrowser_ItemContextMenu");
		}
	}

	void FileDialog::ItemContextMenu(FileDialogItem* dialogItem)
	{
		if (m_ItemContextMenuID != dialogItem->RetrieveItemID())
		{
			return;
		}

		if (!ImGui::BeginPopup("##AssetBrowser_ItemContextMenu"))
		{
			return;
		}

		if (ImGui::MenuItem("Delete"))
		{
			if (dialogItem->IsDirectory())
			{
				FileSystem::Delete(dialogItem->RetrieveFilePath());
				m_IsDirty = true;
			}
			else
			{
				FileSystem::Delete(dialogItem->RetrieveFilePath());
				m_IsDirty = true;
			}
		}

		ImGui::Separator();
		if (ImGui::MenuItem("Open File in Explorer"))
		{
			FileSystem::OpenDirectoryWindow(dialogItem->RetrieveFilePath());
		}

		ImGui::EndPopup();
	}

	bool FileDialog::DialogUpdateFromDirectory(const std::string& directoryPath)
	{
		if (!FileSystem::IsDirectory(directoryPath))
		{
			std::cout << "Directory is invalid for asset browser.\n";
			return false;
		}

		//Delete all existing items.
		m_DialogItems.clear();
		m_DialogItems.shrink_to_fit();

		//Retrieve Directories
		std::vector<std::string> childDirectories = FileSystem::RetrieveDirectoriesInDirectory(directoryPath);
		for (const std::string& childDirectory : childDirectories)
		{
			m_DialogItems.emplace_back(childDirectory, IconLibrary::RetrieveIconLibrary().LoadIcon(childDirectory, IconType::Icon_AssetBrowser_Folder, static_cast<int>(m_DialogItemSize.x)));
		}

		return true;
	}

	void FileDialog::AssetBrowserContextMenu()
	{
		if (ImGui::IsMouseClicked(1) && m_IsHoveringWindow && !m_IsHoveringItem)
		{
			ImGui::OpenPopup("##AssetBrowser_ContentMenu");
		}

		if (!ImGui::BeginPopup("##AssetBrowser_ContentMenu"))
		{
			return;
		}

		if (ImGui::MenuItem("Create Folder"))
		{
			FileSystem::CreateDirectory_(m_Navigation.m_CurrentPath + "/New Folder");
			m_IsDirty = true;
		}

		ImGui::Separator();
		//Create Material, Script etc.

		if (ImGui::MenuItem("Open Directory in Explorer"))
		{
			FileSystem::OpenDirectoryWindow(m_Navigation.m_CurrentPath);
		}

		ImGui::EndPopup();
	}
}