#include "Log.h"

namespace Amethyst
{
	//Everything resolves to this.
	void Log::WriteLog(const char* logMessage, const LogType logType)
	{
		if (!logMessage)
		{
			LOG_ERROR_INVALID_PARAMETER();
			return;
		}

		std::lock_guard<std::mutex> lockGuard(m_MutexLog);

		const bool logToFile = m_Logger.expired() || m_LogToFileEnabled; //Log to file if enabled or if our logger expires.

		if (logToFile)
		{
			m_LogPackages.emplace_back(logMessage, logType);
			LogToFile(logMessage, logType);
		}
		else
		{
			FlushBuffer();
			LogString(logMessage, logType);
		}

		LogToConsole(logMessage, logType);
	}

	void Log::WriteInfoLog(const char* logMessage, ...)
	{
		
	}

	void Log::WriteWarningLog(const char* logMessage, ...)
	{

	}

	void Log::WriteErrorLog(const char* logMessage, ...)
	{

	}

	void Log::WriteLog(const std::string& logMessage, const LogType logType)
	{
		WriteLog(logMessage.c_str(), logType);
	}

	void Log::WriteInfoLog(const std::string logMessage, ...)
	{

	}

	void Log::WriteWarningLog(const std::string logMessage, ...)
	{

	}

	void Log::WriteErrorLog(const std::string logMessage, ...)
	{

	}

	void Log::WriteLog(const Math::Vector2& value, LogType logType)
	{

	}

	void Log::WriteLog(const Math::Vector3& value, LogType logType)
	{

	}

	void Log::WriteLog(const Math::Vector4& value, LogType logType)
	{

	}

	void Log::WriteLog(const Math::Quarternion* value, LogType logType)
	{

	}

	void Log::WriteLog(const Math::Matrix& value, LogType logType)
	{

	}

	void Log::WriteLog(const std::weak_ptr<Entity>& entity, LogType logType)
	{

	}

	void Log::WriteLog(const std::shared_ptr<Entity>& entity, LogType logType)
	{

	}

	void Log::FlushBuffer()
	{

	}

	void Log::LogString(const char* logMessage, LogType logType)
	{

	}

	void Log::LogToConsole(const char* logMessage, LogType logType)
	{

	}

	void Log::LogToFile(const char* logMessage, LogType logType)
	{

	}
}