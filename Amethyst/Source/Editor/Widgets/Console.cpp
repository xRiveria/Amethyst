#include "Console.h"
#include "../Utilities/IconLibrary.h"
#include "../Utilities/EditorExtensions.h"
#include "../../Source/Core/FileSystem.h"

namespace Amethyst
{
	Console::Console(Editor* editorContext) : Widget(editorContext)
	{
		m_WidgetName = "Console";

		AddLogPackage({ "Initializing ImGui...", 0 });
		AddLogPackage({ "Initializing GLFW...", 0 });
		AddLogPackage({ "Initializing Editor...", 0 });
		AddLogPackage({ "There are outdated libraries. Please try to install the latest versions as soon as possible.", 1 });
		AddLogPackage({ "Debug Build Activated.", 2 });
	}

	void Console::OnVisibleTick()
	{
		//Clear Button
		if (ImGui::Button("Clear Console")) { ClearConsole(); } ImGui::SameLine();

		//Lambda for Info, Warning & Error Filter Buttons.
		const auto LogTypeVisibilityToggle = [this](const IconType& iconType, uint32_t filterIndex)
		{
			bool& logVisibility = m_LogTypeVisibilityState[filterIndex];
			ImGui::PushStyleColor(ImGuiCol_Button, logVisibility ? ImGui::GetStyle().Colors[ImGuiCol_FrameBgHovered] : ImGui::GetStyle().Colors[ImGuiCol_FrameBg]);
			if (ImGuiExtensions::ImageButton(iconType, 15.5f))
			{
				logVisibility = !logVisibility;
				m_ScrollToBottom = true;
			}
			ImGui::PopStyleColor();
			ImGui::SameLine();
			ImGui::Text("%d", m_LogTypeCount[filterIndex]);
			ImGui::SameLine(); //For our text filter.
		};

		LogTypeVisibilityToggle(IconType::Icon_Console_Info, 0); //To replace with Icons.
		LogTypeVisibilityToggle(IconType::Icon_Console_Warning, 1);
		LogTypeVisibilityToggle(IconType::Icon_Console_Error, 2);

		//Text Filter
		const float labelWidth = 37.0f;
		m_LogFilter.Draw("Filter", ImGui::GetContentRegionAvail().x - labelWidth);
		ImGui::Separator();

		//Content Properties
		static const ImGuiTableFlags tableFlags =
			ImGuiTableFlags_RowBg		 |
			ImGuiTableFlags_BordersOuter |
			ImGuiTableFlags_ScrollX		 |
			ImGuiTableFlags_ScrollY;

		static const ImVec2 size = ImVec2(-1.0f, -1.0f);

		//Content
		if (ImGui::BeginTable("#WidgetConsoleContent", 2, tableFlags, size))
		{
			for (uint32_t row = 0; row < m_Logs.size(); row++)
			{
				LogPackage& logPackage = m_Logs[row];

				//Text and Visibility Filtering. We will show the log accordingly if the log's text passes the filter and its level is toggled.
				if (m_LogFilter.PassFilter(logPackage.m_Text.c_str()) && m_LogTypeVisibilityState[logPackage.m_LogLevel]) 
				{
					//Switch Row
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					{					
						ImGui::Dummy(ImVec2(0.0f, 5.0f)); //To central-align our column icons.
						ImGui::Image((void*)IconLibrary::RetrieveIconLibrary().RetrieveTextureByType(logPackage.m_LogLevel == 0 ? IconType::Icon_Console_Info : logPackage.m_LogLevel == 1 ? IconType::Icon_Console_Warning : IconType::Icon_Console_Error)->RetrieveTextureID(), ImVec2(20.0f, 20.0f));
					}
					ImGui::TableSetColumnIndex(1);

					//Log
					ImGui::PushID(row);
					{
						ImGui::PushStyleColor(ImGuiCol_Text, m_LogTypeColor[logPackage.m_LogLevel]);
						ImGui::TextUnformatted(logPackage.EditorConsoleText().c_str());
						ImGui::TextUnformatted(logPackage.m_ErrorSource.c_str());
						ImGui::PopStyleColor(1);

						//Context Menu (Right Clicking a Row)
						if (ImGui::BeginPopupContextItem("##WidgetConsoleContext"))
						{
							if (ImGui::MenuItem("Copy"))
							{
								ImGui::LogToClipboard();
								ImGui::LogText("%s", logPackage.m_Text.c_str());
								ImGui::LogFinish();
							}

							ImGui::Separator();

							if (ImGui::MenuItem("Search"))
							{
								FileSystem::OpenDirectoryWindow("https://www.google.com/search?q=" + logPackage.m_Text);
							}

							ImGui::EndPopup();
						}
					}
					ImGui::PopID();
				}
			}

			//Scroll content to the latest entry (towards the bottom) if the specified log level is enabled whenever a new log is added.
			if (m_ScrollToBottom)
			{
				ImGui::SetScrollHereY(); 
				m_ScrollToBottom = false;
			}

			ImGui::EndTable();
		}

		ImplementStatusBar();
	}

	void Console::ImplementStatusBar()
	{
		ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_MenuBar;
		float height = ImGui::GetFrameHeight() + 1.0f; //Add a little padding here so things look nicer.

		if (ImGui::BeginViewportSideBar("##MainStatusBar", nullptr, ImGuiDir_Down, height, windowFlags)) //Specifies that this will be pipped at the top of the window, below the main menu bar.
		{
			if (ImGui::BeginMenuBar()) 
			{
				if (!m_Logs.empty())
				{
					ImGui::PushStyleColor(ImGuiCol_Text, m_LogTypeColor[m_Logs.back().m_LogLevel]);
					ImGui::TextUnformatted(m_Logs.back().m_Text.c_str());
					ImGui::PopStyleColor(1);
				}
				else
				{
					ImGui::TextUnformatted("");
				}

				ImGui::EndMenuBar();
			}
			ImGui::End();
		}
	}

	void Console::AddLogPackage(const LogPackage& logPackage)
	{
		//Save to Dequeue
		m_Logs.push_back(logPackage);
		if (static_cast<uint32_t>(m_Logs.size()) > m_LogMaximumCount)
		{
			m_Logs.pop_front(); 
		}

		//Update Count
		m_LogTypeCount[logPackage.m_LogLevel]++;

		if (m_LogTypeVisibilityState[logPackage.m_LogLevel])
		{
			m_ScrollToBottom = true;
		}
	}

	void Console::ClearConsole()
	{
		m_Logs.clear();
		m_Logs.shrink_to_fit();

		m_LogTypeCount[0] = 0;
		m_LogTypeCount[1] = 0;
		m_LogTypeCount[2] = 0;
	}
}