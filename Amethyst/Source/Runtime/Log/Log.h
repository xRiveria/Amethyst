#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include "ILogger.h"

namespace Amethyst
{
	//Macros
	#define LOG_INFO(logMessage, ...)	 { Amethyst::Log::WriteInfoLog(std::string(__FUNCTION__) + ": " + std::string(logMessage), __VA_ARGS__); }
	#define LOG_WARNING(logMessage, ...) { Amethyst::Log::WriteWarningLog(std::string(__FUNCTION__) + ": " + std::string(logMessage), __VA_ARGS__); }
	#define LOG_ERROR(logMessage, ...)	 { Amethyst::Log::WriteErrorLog(std::string(__FUNCTION__) + ": " + std::string(logMessage), __VA_ARGS__); }

	//Standard Errors
	#define LOG_ERROR_GENERIC_FAILURE()		LOG_ERROR("Failed.");
	#define LOG_ERROR_INVALID_PARAMETER()	LOG_ERROR("Invaliad Parameter.");
	#define LOG_ERROR_INVaLIAD_INTERNALS()	LOG_ERROR("Invalid Internals.");

	//Misc
	#define LOG_TO_FILE(value) { Amethyst::Log::m_LogFileName = value; }

	class Entity;

	namespace Math
	{
		class Quarternion;
		class Matrix;
		class Vector2;
		class Vector3;
		class Vector4;
	}

	enum class LogType
	{
		Info,
		Warning,
		Error
	};

	struct LogPackage
	{
		LogPackage(const std::string& logMessage, const LogType logType)
		{
			this->m_LogMessage = logMessage;
			this->m_LogType = logType;
		}

		std::string m_LogMessage;
		LogType m_LogType;
	};

	class Log
	{
		friend class ILogger;

	public:
		Log() = default;

		//Set a logger to be used.
		static void SetLogger(const std::weak_ptr<ILogger>& logger) { m_Logger = logger; }

		//Alphabetical
		static void WriteLog(const char* logMessage, const LogType logType);
		static void WriteInfoLog(const char* logMessage, ...);
		static void WriteWarningLog(const char* logMessage, ...);
		static void WriteErrorLog(const char* logMessage, ...);

		static void WriteLog(const std::string& logMessage, const LogType logType);
		static void WriteInfoLog(const std::string logMessage, ...);
		static void WriteWarningLog(const std::string logMessage, ...);
		static void WriteErrorLog(const std::string logMessage, ...);

		//Numeric
		template<typename T, typename std::enable_if<
			std::is_same<T, int>::value						||
			std::is_same<T, long>::value					||
			std::is_same<T, long long>::value				||
			std::is_same<T, unsigned>::value				||
			std::is_same<T, unsigned long>::value			||
			std::is_same<T, unsigned long long>::value		||
			std::is_same<T, float>::value					||
			std::is_same<T, double>::value					||
			std::is_same<T, long double>::value>::type>
		static void WriteLog(T value, LogType logType)
		{
			WriteLog(std::to_string(value), logType);
		}

		//Math
		static void WriteLog(const Math::Vector2& value, LogType logType);
		static void WriteLog(const Math::Vector3& value, LogType logType);
		static void WriteLog(const Math::Vector4& value, LogType logType);
		static void WriteLog(const Math::Quarternion* value, LogType logType);
		static void WriteLog(const Math::Matrix& value, LogType logType);

		//Manually Handled Types
		static void WriteLog(const bool value, const LogType logType) { WriteLog(value ? "True" : "False", logType); }
		template<typename T>
		static void WriteLog(std::weak_ptr<T> pointer, const LogType logType) { WriteLog(pointer.expired() ? "Expired" : typeid(pointer).name(), logType); }
		template<typename T>
		static void WriteLog(std::shared_ptr<T> pointer, const LogType logType) { WriteLog(pointer ? typeid(pointer).name() : "Null", logType); }
		static void WriteLog(const std::weak_ptr<Entity>& entity, LogType logType);
		static void WriteLog(const std::shared_ptr<Entity>& entity, LogType logType);

	private:
		static void FlushBuffer();
		static void LogString(const char* logMessage, LogType logType);
		static void LogToConsole(const char* logMessage, LogType logType);
		static void LogToFile(const char* logMessage, LogType logType);

	private:
		static bool m_ConsoleLoggingEnabled;
		static bool m_LogToFileEnabled;
		static std::weak_ptr<ILogger> m_Logger;

		static std::mutex m_MutexLog;
		static std::ofstream m_Stream;
		static std::string m_LogFileName;
		static bool m_IsFirstLog;
		static std::vector<LogPackage> m_LogPackages;
	};
}