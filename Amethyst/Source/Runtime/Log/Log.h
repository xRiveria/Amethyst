#pragma once
#include <string>
#include <memory>
#include <mutex>
#include <vector>
#include "../../Core/FileSystem.h"
#include "ILogger.h"

namespace Amethyst
{
	//Macros
	#define AMETHYST_INFO(logMessage, ...)	  {  Amethyst::Log::WriteInfoLog	(std::string(logMessage + std::string("Source: ") + std::string(__FUNCTION__) + "() (at " + Amethyst::FileSystem::RetrieveFilePathRelativeToProject(static_cast<std::string>(__FILE__), "Amethyst") + ")") + ":" + std::to_string(__LINE__), __VA_ARGS__); }
	#define AMETHYST_WARNING(logMessage, ...) {  Amethyst::Log::WriteWarningLog (std::string(logMessage + std::string("Source: ") + std::string(__FUNCTION__) + "() (at " + Amethyst::FileSystem::RetrieveFilePathRelativeToProject(static_cast<std::string>(__FILE__), "Amethyst") + ")") + ":" + std::to_string(__LINE__), __VA_ARGS__); }
	#define AMETHYST_ERROR(logMessage, ...)	  {  Amethyst::Log::WriteErrorLog	(std::string(logMessage + std::string("Source: ") + std::string(__FUNCTION__) + "() (at " + Amethyst::FileSystem::RetrieveFilePathRelativeToProject(static_cast<std::string>(__FILE__), "Amethyst") + ")") + ":" + std::to_string(__LINE__), __VA_ARGS__); }
	
	//Standard Errors
	#define AMETHYST_ERROR_GENERIC_FAILURE()	AMETHYST_ERROR("Failed.");
	#define AMETHYST_ERROR_INVALID_PARAMETER()	AMETHYST_ERROR("Invalid Parameter.");
	#define AMETHYST_ERROR_INVALID_INTERNALS()	AMETHYST_ERROR("Invalid Internals.");

	//Misc
	#define LOG_TO_FILE(value) { Amethyst::Log::m_LogToFileEnabled = value; }

	class Entity;

	namespace Math
	{
		class Quaternion;
		class Matrix;
		class Vector2;
		class Vector3;
		class Vector4;
	}

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
		static void WriteLog(T value, const std::string& logSource, LogType logType)
		{
			WriteLog(std::to_string(value), logSource, logType);
		}

		//Math
		static void WriteLog(const Math::Vector2& value, LogType logType);
		static void WriteLog(const Math::Vector3& value, LogType logType);
		static void WriteLog(const Math::Vector4& value, LogType logType);
		static void WriteLog(const Math::Quaternion* value, LogType logType);
		static void WriteLog(const Math::Matrix& value, LogType logType);

		//Manually Handled Types
		static void WriteLog(const bool value, const LogType logType) { WriteLog(value ? "True" : "False", logType); }
		template<typename T>
		static void WriteLog(std::weak_ptr<T> pointer, const LogType logType) { WriteLog(pointer.expired() ? "Expired" : typeid(pointer).name(), logType); }
		template<typename T>
		static void WriteLog(std::shared_ptr<T> pointer, const LogType logType) { WriteLog(pointer ? typeid(pointer).name() : "Null", logType); }
		static void WriteLog(const std::weak_ptr<Entity>& entity, LogType logType);
		static void WriteLog(const std::shared_ptr<Entity>& entity, LogType logType);

	public:
		static bool m_LogToFileEnabled;

	private:
		static void FlushBuffer();
		static void LogString(const char* logMessage, const std::string& logSource, LogType logType);
		static void LogToConsole(const char* logMessage, LogType logType);
		static void LogToFile(const char* logMessage, LogType logType);

		//Console
		static void SetConsoleTextColor(LogType logType);
		static void RestoreConsoleAttributes();

	private:
		static bool m_ConsoleLoggingEnabled;
		static std::weak_ptr<ILogger> m_Logger;

		static std::mutex m_MutexLog;
		static std::ofstream m_Stream;
		static std::string m_LogFileName;
		static bool m_IsFirstLog;
		static std::vector<LogPackage> m_LogPackages;

		//Console
		static HANDLE m_OutHandle;
		static CONSOLE_SCREEN_BUFFER_INFO m_DefaultConsoleState;
	};
}