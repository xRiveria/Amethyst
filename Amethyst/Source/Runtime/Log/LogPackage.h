#pragma once
#include <string>

namespace Amethyst
{
	enum class LogType
	{
		Info,
		Warning,
		Error
	};

	struct LogPackage
	{
		LogPackage() {}
		LogPackage(const std::string& text, LogType logType) : m_Text(text), m_LogLevel(logType)
		{

		}

		LogPackage(const std::string& text, const std::string& logSource, LogType logType) : m_Text(text), m_LogSource(logSource), m_LogLevel(logType)
		{

		}

		std::string EditorConsoleText()
		{
			return m_Timestamp + m_Text;
		}

		std::string LevelToString()
		{
			return m_LogLevel == LogType::Info ? "[INFO] " : m_LogLevel == LogType::Warning ? "[WARNING] " : "[ERROR] ";
		}

		std::string m_Text;
		std::string m_Timestamp = "[19:50:45] ";
		std::string m_LogSource = "Toolbar::CreateConsole() (at Assets/Scripts/PlacingSystem.cpp:41)";
		LogType m_LogLevel = LogType::Info;
	};
}