#pragma once
#include <string>

namespace Amethyst
{
	class Serializer
	{
	protected:
		virtual ~Serializer() = default;

		virtual void SerializeToYAMLFile(const std::string& outputFilePath) = 0;
		virtual bool DeserializeFromYAMLFile(const std::string& inputFilePath) = 0;
	};
}