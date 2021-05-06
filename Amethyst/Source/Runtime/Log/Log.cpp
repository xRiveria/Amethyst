#include "Amethyst.h"
#include "../ECS/Entity.h"
#include "../../Core/FileSystem.h"
#include <iostream>
#include <stdarg.h>

namespace Amethyst
{
	std::weak_ptr<ILogger> Log::m_Logger;
	std::ofstream Log::m_Stream;
	std::string Log::m_LogFileName = "Log.txt";
	std::mutex Log::m_MutexLog;
	std::vector<LogPackage> Log::m_LogPackages;
	                               
	bool Log::m_IsFirstLog = true; //Start logging to file. 
	bool Log::m_LogToFileEnabled = true;
	bool Log::m_ConsoleLoggingEnabled = true;

	HANDLE Log::m_OutHandle;
	CONSOLE_SCREEN_BUFFER_INFO Log::m_DefaultConsoleState;

	//Everything resolves to this.
	void Log::WriteLog(const char* logMessage, const LogType logType)
	{
		if (!logMessage)
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return;
		}

		std::lock_guard<std::mutex> lockGuard(m_MutexLog);

		const bool logToFile = m_Logger.expired() || m_LogToFileEnabled; //Log to file if enabled or if our logger expires.

		std::string logText = std::string(logMessage);

		std::size_t sourceInformationIndex = logText.find("Source: ");
		std::string logTextExtracted = logText.substr(0, sourceInformationIndex);
		std::string logSource = logText.substr(sourceInformationIndex + 8, logText.length());

		if (logToFile)
		{
			m_LogPackages.emplace_back(logTextExtracted, logSource, logType);
			LogToFile(logTextExtracted.c_str(), logType);

			//As we can begin logging in the console before our engine logger is established, we will do a check here to ensure it exists.
			if (!m_Logger.expired())
			{
				LogString(logTextExtracted.c_str(), logSource, logType);
			}
		}
		else
		{
			FlushBuffer();
		}

		LogToConsole(logTextExtracted.c_str(), logType);
	}

	void Log::LogString(const char* logMessage, const std::string& logSource, LogType logType)
	{
		if (!logMessage)
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return;
		}

		m_Logger.lock()->LogMessage(std::string(logMessage), logSource, logType);
	}

	void Log::LogToConsole(const char* logMessage, LogType logType)
	{
		if (!m_OutHandle)
		{
			m_OutHandle = GetStdHandle(STD_OUTPUT_HANDLE);
			GetConsoleScreenBufferInfo(m_OutHandle, &m_DefaultConsoleState);
		}

		SetConsoleTextColor(logType);
		std::cout << logMessage << "\n";
		RestoreConsoleAttributes();
	}

	void Log::LogToFile(const char* logMessage, LogType logType)
	{
		if (!logMessage)
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return;
		}

		const std::string logPrefix = (logType == LogType::Info) ? "Info: " : (logType == LogType::Warning) ? "Warning: " : "Error: ";
		const std::string finalLog = logPrefix + logMessage;

		//Delete the previous log file (if it exists).
		if (m_IsFirstLog)
		{
			FileSystem::Delete(m_LogFileName);
			m_IsFirstLog = false;
		}

		//Open or Create a new log file to write the log message to.
		m_Stream.open(m_LogFileName, std::ofstream::out | std::ofstream::app); //Output | Append

		if (m_Stream.is_open())
		{
			//Write out the error message to the file.
			m_Stream << finalLog << std::endl;

			//Close the file.
			m_Stream.close();
		}
	}

	void Log::WriteInfoLog(const char* logMessage, ...)
	{
		char buffer[2048];

		//va_list is a complete object type suitable for holding the information needed by the va_ macros. If a va_list instance is created, passed to another function and
		//used via va_arg in that function, then any subsequent use in the calling function should be preceded by a call to va_end.
		va_list arguments;

		//The va_start macro enables access to the variable arguments following the named argument. va_start should be invoked with an instance to a valid va_list object before any calls.
		va_start(arguments, logMessage); 

		//Composes a string with the same text that would be printed if the content was used on printf, but using the elements in the variable argument list identified 
		//by the va_list arg instead of additional function arguments, storing the resulting content as a C-string buffer in a buffer. Internally, the function should 
		//have been initialized by va_start at some point before the call, and is expected to be released by va_end at some point after the call.
		auto w = vsnprintf(buffer, sizeof(buffer), logMessage, arguments);
	
		//The va_end macro performs cleanup for an object initialized by a call to va_start or va_copy. If there is no corresponding call to va_start or va_copy, 
		//or if va_end is not called before a function that calls va_start or va_copy returns, the behavior is undefined.
		va_end(arguments);

		WriteLog(buffer, LogType::Info);
	}

	void Log::WriteWarningLog(const char* logMessage, ...)
	{
		char buffer[2048];

		va_list arguments;
		va_start(arguments, logMessage);
		auto w = vsnprintf(buffer, sizeof(buffer), logMessage, arguments);
		va_end(arguments);

		WriteLog(buffer, LogType::Warning);
	}

	void Log::WriteErrorLog(const char* logMessage, ...)
	{
		char buffer[2048];

		va_list arguments;
		va_start(arguments, logMessage);
		auto w = vsnprintf(buffer, sizeof(buffer), logMessage, arguments);
		va_end(arguments);

		WriteLog(buffer, LogType::Error);
	}

	void Log::WriteLog(const std::string& logMessage, const LogType logType)
	{
		WriteLog(logMessage.c_str(), logType);
	}

	void Log::WriteInfoLog(const std::string logMessage, ...)
	{
		char buffer[2048];

		va_list arguments;
		va_start(arguments, logMessage); 
		auto w = vsnprintf(buffer, sizeof(buffer), logMessage.c_str(), arguments);
		va_end(arguments);

		WriteLog(buffer, LogType::Info);
	}

	void Log::WriteWarningLog(const std::string logMessage, ...)
	{
		char buffer[2048];

		va_list arguments;
		va_start(arguments, logMessage);
		auto w = vsnprintf(buffer, sizeof(buffer), logMessage.c_str(), arguments);
		va_end(arguments);

		WriteLog(buffer, LogType::Warning);
	}

	void Log::WriteErrorLog(const std::string logMessage, ...)
	{
		char buffer[2048];

		va_list arguments;
		va_start(arguments, logMessage);
		auto w = vsnprintf(buffer, sizeof(buffer), logMessage.c_str(), arguments);
		va_end(arguments);

		WriteLog(buffer, LogType::Error);
	}

	void Log::WriteLog(const Math::Vector2& value, LogType logType)
	{
		WriteLog(value.ToString(), logType);
	}

	void Log::WriteLog(const Math::Vector3& value, LogType logType)
	{
		WriteLog(value.ToString(), logType);
	}

	void Log::WriteLog(const Math::Vector4& value, LogType logType)
	{
		WriteLog(value.ToString(), logType);
	}

	void Log::WriteLog(const Math::Quarternion* value, LogType logType)
	{
		return;
	}

	void Log::WriteLog(const Math::Matrix& value, LogType logType)
	{
		return;
	}

	void Log::WriteLog(const std::weak_ptr<Entity>& entity, LogType logType)
	{
		/*
			.lock() creates a new std::shared_ptr that shares ownership of the managed object. If there is no managed object, i.e. *this is empty, then the returned 
			std::shared_ptr is also empty. 
		*/
		entity.expired() ? WriteLog("Null", logType) : WriteLog(entity.lock()->RetrieveName(), logType);
	}

	void Log::WriteLog(const std::shared_ptr<Entity>& entity, LogType logType)
	{
		entity ? WriteLog(entity->RetrieveName(), logType) : WriteLog("Null", logType);
	}

	void Log::FlushBuffer()
	{
		if (m_Logger.expired() || m_LogPackages.empty())
		{
			return;
		}

		//Log everything from memory to the logger implementation.
		for (const LogPackage& logPackage : m_LogPackages)
		{
			LogString(logPackage.m_Text.c_str(), logPackage.m_LogSource, logPackage.m_LogLevel);
		}

		m_LogPackages.clear();
	}

	void Log::SetConsoleTextColor(LogType logType)
	{
		logType == LogType::Info ? SetConsoleTextAttribute(m_OutHandle, WhiteConsoleTextColor) : logType == LogType::Warning ? SetConsoleTextAttribute(m_OutHandle, YellowConsoleTextColor) : SetConsoleTextAttribute(m_OutHandle, RedConsoleTextColor);
	}

	void Log::RestoreConsoleAttributes()
	{
		SetConsoleTextAttribute(m_OutHandle, m_DefaultConsoleState.wAttributes);
	}
}