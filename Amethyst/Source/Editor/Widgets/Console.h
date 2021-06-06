#pragma once
#include "../Widget.h"
#include <deque>
#include <vector>
#include <glm/glm.hpp>
#include <functional>
#include <chrono>
#include <iomanip>
#include "../../Source/Runtime/Log/ILogger.h"

// Implementation of Amethyst::ILogger so the engine can log into the editor.
class ConsoleLogger : public Amethyst::ILogger
{
public:
	typedef std::function<void(Amethyst::LogPackage)> LogFunction;

	void SetCallback(LogFunction&& function)
	{
		m_LogFunction = std::forward<LogFunction>(function);
	}

	void LogMessage(const std::string& logMessage, const std::string& logSource, const Amethyst::LogType logMessageType) override
	{
		Amethyst::LogPackage logPackage;

		logPackage.m_Text = logMessage;
		logPackage.m_LogSource = logSource;
		logPackage.m_LogLevel = logMessageType;
		logPackage.m_Timestamp = RetrieveCurrentTime();

		m_LogFunction(logPackage);
	}

	std::string RetrieveCurrentTime()
	{
		std::chrono::time_point currentTimePoint = std::chrono::system_clock::now();
		time_t currentTime = std::chrono::system_clock::to_time_t(currentTimePoint);
		std::tm buffer;
		localtime_s(&buffer, &currentTime);

		std::stringstream transTime;
		transTime << std::put_time(&buffer, "[%H:%M:%S]");

		return transTime.str();
	}

private:
	LogFunction m_LogFunction;
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
	void AddLogPackage(const Amethyst::LogPackage& logPackage);
	void ClearConsole();
	void ImplementStatusBar();

private:
	std::deque<Amethyst::LogPackage> m_Logs;
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

	std::shared_ptr<ConsoleLogger> m_Logger;
};
