#pragma once
#include <string>

namespace Amethyst
{
	class ILogger
	{
	public:
		virtual ~ILogger() = default;
		virtual void Log(const std::string& logMessage, uint32_t logMessageType) = 0;
	};
}