#pragma once
#include "EngineSupport.h"
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
		static std::string RetrieveStringBetweenExpressions(const std::string& string, const std::string& expressionA, const std::string& expressionB);
		static std::vector<std::string> RetrieveIncludeFiles(const std::string& filePath);

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
		static std::string RetrieveFileNameWithoutExtension(const std::string& filePath);
		static std::string ReplaceExtention(const std::string& filePath, const std::string& fileExtension);
		static std::string RetrieveExtensionFromFilePath(const std::string& filePath);
		static std::string NatifyFilePath(const std::string& filePath);
		static std::string RetrieveRelativeFilePath(const std::string& filePath);
		static std::string RetrieveWorkingDirectory();
		static std::string RetrieveRootDirectory(const std::string& filePath);
		static std::string RetrieveParentDirectory(const std::string& filePath);

		static std::vector<std::string> RetrieveDirectoriesInDirectory(const std::string& directoryPath);
		static std::vector<std::string> RetrieveFilesInDirectory(const std::string& directoryPath);

		//Supported Files
		static bool IsSupportedAudioFile(const std::string& filePath);
		static bool IsSupportedImageFile(const std::string& filePath);
		static bool IsSupportedModelFile(const std::string& filePath);
		static bool IsSupportedShaderFile(const std::string& filePath);
		static bool IsSupportedFontFile(const std::string& filePath);
		static bool IsEngineScriptFile(const std::string& filePath);
		static bool IsEnginePrefabFile(const std::string& filePath);
		static bool IsEngineMaterialFile(const std::string& filePath);
		static bool IsEngineMeshFile(const std::string& filePath);
		static bool IsEngineModelFile(const std::string& filePath);
		static bool IsEngineSceneFile(const std::string& filePath);
		static bool IsEngineTextureFile(const std::string& filePath);
		static bool IsEngineAudioFile(const std::string& filePath);
		static bool IsEngineShaderFile(const std::string& filePath);
		static bool IsEngineFile(const std::string& filePath);

		//Supported Files in Directory
		static std::vector<std::string> RetrieveSupportedFilesInDirectory(const std::string& directoryPath);
		static std::vector<std::string> RetrieveSupportedImageFilesFromPaths(const std::vector<std::string>& paths);
		static std::vector<std::string> RetrieveSupportedAudioFilesFromPaths(const std::vector<std::string>& paths);
		static std::vector<std::string> RetrieveSupportedScriptFilesFromPaths(const std::vector<std::string>& paths);
		static std::vector<std::string> RetrieveSupportedModelFilesFromPaths(const std::vector<std::string>& paths);
		static std::vector<std::string> RetrieveSupportedModelFilesInDirectory(const std::string& directoryPath);
		static std::vector<std::string> RetrieveSupportedSceneFilesInDirectory(const std::string& directoryPath);
	};
}