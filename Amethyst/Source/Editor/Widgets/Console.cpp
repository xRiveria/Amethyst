#include "Console.h"

namespace Amethyst
{
	Console::Console() : Widget()
	{
		m_WidgetName = "Console";
	}

	void Console::OnVisibleTick()
	{
		//Temporary
		if (ImGui::Button("Add Log"))
		{
			AddLogPackage({ "Hello World", 0 });
			AddLogPackage({ "Hello World Oopopos", 1 });
			AddLogPackage({ "Hello World Headwdaw", 2 });
		}
		
		//Clear Button
		if (ImGui::Button("Clear Console")) { ClearConsole(); }
		ImGui::SameLine();

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
		if (ImGui::BeginTable("#WidgetConsoleContent", 1, tableFlags, size))
		{
			for (uint32_t row = 0; row < m_Logs.size(); row++)
			{
				LogPackage& logPackage = m_Logs[row];

				//Text and Visibility Filtering. We will show the log accordingly if the log's text passes the filter and its level is toggled.
				if (m_LogFilter.PassFilter(logPackage.m_Text.c_str()) && m_LogTypeVisibilityState[logPackage.m_ErrorLevel]) 
				{
					//Switch Row
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);

					//Log
					ImGui::PushID(row);
					{
						ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(m_LogTypeColor[logPackage.m_ErrorLevel].x, m_LogTypeColor[logPackage.m_ErrorLevel].y, m_LogTypeColor[logPackage.m_ErrorLevel].z, m_LogTypeColor[logPackage.m_ErrorLevel].z));
						ImGui::TextUnformatted(logPackage.m_Text.c_str());
						ImGui::PopStyleColor(1);

						//Context Menu
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

							}
							ImGui::EndPopup();
						}
					}
					ImGui::PopID();
				}
			}
			ImGui::EndTable();
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
		m_LogTypeCount[logPackage.m_ErrorLevel]++;
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