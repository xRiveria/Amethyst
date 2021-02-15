#pragma once
#include <vector>
#include <string>

namespace Amethyst
{
	class FileSystem
	{
	public:
		//File Creation
		static void CreateTextFile(const std::string& filePath, const std::string& text);

		//Strings
		static bool IsEmptyOrContainsWhitespace(const std::string& value);
		static bool IsAlphanumeric(const std::string& value);
		static std::string ConvertToUppercaseString(const std::string& lowercaseString);
		static std::wstring StringToWString(const std::string& string);

		//Paths
		static void OpenDirectoryWindow(const std::string& directoryPath);
		static bool CreateDirectory_(const std::string& directoryPath);
		static bool Delete(const std::string& filePath);
		static bool Exists(const std::string& filePath);
		static bool IsDirectory(const std::string& directoryPath);
		static bool IsFile(const std::string& filePath);
		static bool CopyFileFromTo(const std::string& source, const std::string& destination);

		//Retrievals
		static std::string RetrieveFileNameFromFilePath(const std::string& filePath);
		static std::string RetrieveFileNameWithNoExtensionFromFilePath(const std::string& filePath);
		static std::string RetrieveDirectoryFromFilePath(const std::string& filePath);
		static std::string RetrieveFilePathWithoutExtension(const std::string& filePath);
		static std::string ReplaceExtention(const std::string& filePath, const std::string& fileExtension);
		static std::string RetrieveExtensionFromFilePath(const std::string& filePath);
		static std::string NatifyFilePath(const std::string& filePath);
		static std::string RetrieveRelativeFilePath(const std::string& filePath);
		static std::string RetrieveWorkingDirectory();
		static std::string RetrieveRootDirectory(const std::string& filePath);
		static std::string RetrieveParentDirectory(const std::string& filePath);
		static std::vector<std::string> RetrieveDirectoriesInDirectory(const std::string& directoryPath);
		static std::vector<std::string> RetrieveFilesInDirectory(const std::string& directoryPath);
	};
}