#pragma once
#include <string>
#include "LogPackage.h"

namespace Amethyst
{
	class ILogger
	{
	public:
		virtual ~ILogger() = default;
		virtual void LogMessage(const std::string& logMessage, const std::string& logSource, const LogType logMessageType) = 0;
	};
}