#pragma once
#include "../Widget.h"
#include <deque>
#include <vector>
#include <glm/glm.hpp>

namespace Amethyst
{
	//Self populated.
	struct LogPackage
	{
		LogPackage(const std::string& text, const int& errorLevel) : m_Text(text), m_LogLevel(errorLevel)
		{

		}

		std::string EditorConsoleText()
		{
			return m_Timestamp + m_Text;
		}

		std::string LevelToString()
		{
			return m_LogLevel == 0 ? "[INFO] " : m_LogLevel == 1 ? "[WARNING] " : "[ERROR] ";
		}

		std::string m_Text;
		std::string m_Timestamp = "[19:50:45] ";
		std::string m_ErrorSource = "Toolbar::CreateConsole() (at Assets/Scripts/PlacingSystem.cpp:41)";
		unsigned int m_LogLevel = 0;
	};

	/*
		- Expand with Clear on Play.
	*/

	class Console : public Widget
	{
	public:
		Console(Editor* editorContext);

		void OnVisibleTick() override;

		//Console Specific
		void AddLogPackage(const LogPackage& logPackage);
		void ClearConsole();
		void ImplementStatusBar();

	private:
		std::deque<LogPackage> m_Logs;
		uint32_t m_LogMaximumCount = 1000;

		//Log Types
		uint32_t m_LogTypeCount[3] = { 0, 0, 0 }; //Info, Warning & Error Respectively.
		const std::vector<glm::vec4> m_LogTypeColor =
		{
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),      // Info
			glm::vec4(1.0f, 1.0f, 0.4f, 1.0f),      // Warning
			glm::vec4(1.0f, 0.0f, 0.0f, 1.0f)       // Error
		};
		
		//Filtering
		bool m_ScrollToBottom = false;
		bool m_LogTypeVisibilityState[3] = { true, true, true }; //For filtering purposes.
		ImGuiTextFilter m_LogFilter;
	};
}