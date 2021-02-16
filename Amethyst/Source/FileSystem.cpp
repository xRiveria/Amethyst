#include "FileSystem.h"
#include <filesystem>
#include <windows.h>
#include <WinUser.h>
#include <stringapiset.h>
#include <shellapi.h>
#include <fstream>
#include <sstream>
#include <regex>

namespace Amethyst
{
	void FileSystem::CreateTextFile(const std::string& filePath, const std::string& text)
	{
		std::ofstream outputFile(filePath);
		outputFile << text;

		//When you write data to a stream, it is not written immediately, and it is buffered. Thus, using flush will ensure that all your data from buffer is written.
		//We need to make sure that all the writes are completed before we close the stream, and that is why flush() is called in file/buffered writer's close().
		//It flushes the output stream and forces any buffered output bytes to be written out. The general contract of flush is that calling it is an indication that,
		//if any bytes previously written have been buffered by the implementation of the output stream, such bytes should immediately be written to their intended destination.
		outputFile.flush(); 
		outputFile.close();
	}

	bool FileSystem::IsEmptyOrContainsWhitespace(const std::string& value)
	{
		//Check if the string is empty.
		if (value.empty())
		{
			return true;
		}

		//Check if it's made out of whitespace characters.
		for (char _char : value)
		{
			if (!std::isspace(_char)) //Checks if the given character is whitespace character as classified by the currently installed C locale. Includes \n, ' '.
			{
				return false;
			}
		}
		return true;
	}

	bool FileSystem::IsAlphanumeric(const std::string& value)
	{
		if (IsEmptyOrContainsWhitespace(value))
		{
			return false;
		}

		for (char _char : value)
		{
			if (!std::isalnum(_char)) //Checks if the given character is an alphanumeric character as classified by the current C locale. Includes digits, lowercase and uppercase letters.
			{
				return false;
			}
		}
		return true;
	}

	std::string FileSystem::ConvertToUppercaseString(const std::string& lowercaseString)
	{
		const std::locale locale;
		std::string uppercaseString;

		for (const char& character : lowercaseString)
		{
			uppercaseString += std::toupper(character, locale);
		}

		return uppercaseString;
	}

	std::wstring FileSystem::StringToWString(const std::string& string)
	{
		const auto stringLength = static_cast<int>(string.length() + 1); //+1 for index 0.
		const auto wStringLength = MultiByteToWideChar(CP_ACP, 0, string.c_str(), stringLength, nullptr, 0);
	    wchar_t* const buffer = new wchar_t[wStringLength];
		MultiByteToWideChar(CP_ACP, 0, string.c_str(), stringLength, buffer, wStringLength);

		std::wstring result(buffer);
		delete[] buffer;
		return result;
	}

	std::string FileSystem::RetrieveStringBetweenExpressions(const std::string& string, const std::string& expressionA, const std::string& expressionB)
	{
		//The regular expressions library provides a class that represents regular expressions, which are a kind of mini-language used to perform pattern matching within strings.
		const std::regex baseRegex(expressionA + "(.*)" + expressionB);

		//std::smatch is an instantiation of the match_results class template for matches on string objects. 
		//str() - retrieve the text that was matched.
		//position() - the starting position.
		//length() - length of the match relative to the subject string.
		std::smatch baseMatch;

		//regex_search will search for anything in the input string that matches the regex. The whole string does not have to match, just part of it.
		//If you were to use regex_match, then the entire string must match.
		if (std::regex_search(string, baseMatch, baseRegex)) //string is the sentence you wish to search in, baseMatch is the retrieved match result, baseRegex determines the search pattern.

		{
			//The first sub_match is the whole string; the next sub_match is the first parenthesized expression.
			if (baseMatch.size() == 2)
			{
				return baseMatch[1].str(); 
			}
		}

		return string;
	}

	std::vector<std::string> FileSystem::RetrieveIncludeFiles(const std::string& filePath)
	{
		//Read the file.
		std::ifstream inputFile(filePath);
		std::stringstream streamBuffer;
		streamBuffer << inputFile.rdbuf(); //This returns a pointer to the string buffer for the current string stream. 

		std::string source = streamBuffer.str();
		std::string fileDirectory = RetrieveDirectoryFromFilePath(filePath);
		std::string directiveSymbol = "#include \"";
		std::vector<std::string> filePaths;

		//Early exit if there is no include directive.
		if (source.find(directiveSymbol) == std::string::npos)
		{
			return filePaths;
		}

		//Scan for include directives.
		std::ifstream stream(source);
		std::string includeDirective;
		while (std::getline(stream, includeDirective))
		{
			if (includeDirective.find(directiveSymbol) != std::string::npos)
			{
				//Construct file path and save it.
				std::string fileName = RetrieveStringBetweenExpressions(includeDirective, directiveSymbol, "\"");
				filePaths.emplace_back(fileDirectory + fileName);
			}
		}

		//If any file path contains more file paths inside, start resolving them recursively.
		std::vector<std::string> filePathsCopy = filePaths; //Copy the file paths to avoid modification while iterating.
		for (const std::string& filePath : filePathsCopy)
		{
			//Read the file.
			std::ifstream _inputFile(filePath);
			std::stringstream _streamBuffer;
			_streamBuffer << _inputFile.rdbuf();

			//Check for include directive.
			std::string _source = _streamBuffer.str();
			if (_source.find(directiveSymbol) != std::string::npos)
			{
				std::vector<std::string> newIncludes = RetrieveIncludeFiles(filePath);
				filePaths.insert(filePaths.end(), newIncludes.begin(), newIncludes.end());
			}
		}

		//At this point, everything should be resolved.
		return filePaths;
	}

	void FileSystem::OpenDirectoryWindow(const std::string& directoryPath)
	{
		ShellExecute(nullptr, nullptr, StringToWString(directoryPath).c_str(), nullptr, nullptr, SW_SHOW);
	}

	bool FileSystem::CreateDirectory_(const std::string& directoryPath)
	{
		try
		{
			if (std::filesystem::create_directories(directoryPath))
			{
				return true;
			}
		}
		catch (std::filesystem::filesystem_error& error)
		{
			
		}

		return false;
	}

	bool FileSystem::Delete(const std::string& filePath)
	{
		try
		{
			if (std::filesystem::exists(filePath) && std::filesystem::remove_all(filePath))
			{
				return true;
			}
		}
		catch (std::filesystem::filesystem_error& error)
		{

		}

		return false;
	}

	bool FileSystem::Exists(const std::string& filePath)
	{
		try
		{
			if (std::filesystem::exists(filePath))
			{
				return true;
			}
		}
		catch (std::filesystem::filesystem_error& error)
		{

		}

		return false;
	}

	bool FileSystem::IsDirectory(const std::string& directoryPath)
	{
		try
		{
			if (std::filesystem::exists(directoryPath) && std::filesystem::is_directory(directoryPath))
			{

			}
		}
		catch (std::filesystem::filesystem_error& error)
		{

		}

		return false;
	}

	bool FileSystem::IsFile(const std::string& filePath)
	{
		if (filePath.empty())
		{
			return false;
		}

		try
		{
			if (std::filesystem::exists(filePath) && std::filesystem::is_regular_file(filePath))
			{
				return true;
			}
		}
		catch (std::filesystem::filesystem_error& error)
		{

		}

		return false;
	}

	bool FileSystem::CopyFileFromTo(const std::string& source, const std::string& destination)
	{
		if (source == destination)
		{
			return true;
		}

		//If the destination path does not exist, create it.
		if (!Exists(RetrieveDirectoryFromFilePath(destination)))
		{
			CreateDirectory_(RetrieveDirectoryFromFilePath(destination));
		}

		try
		{
			return std::filesystem::copy_file(source, destination, std::filesystem::copy_options::overwrite_existing);
		}
		catch (std::filesystem::filesystem_error& error)
		{
			
			return true;
		}
	}

	//=================================================================================

	std::string FileSystem::RetrieveFileNameFromFilePath(const std::string& filePath)
	{
		return std::filesystem::path(filePath).filename().generic_string();
	}

	std::string FileSystem::RetrieveFileNameWithNoExtensionFromFilePath(const std::string& filePath)
	{
		const std::string fileName = RetrieveFileNameFromFilePath(filePath);
		const size_t lastIndex = fileName.find_last_of('.');

		if (lastIndex != std::string::npos)
		{
			return fileName.substr(0, lastIndex); 
		}

		return "";
	}

	std::string FileSystem::RetrieveDirectoryFromFilePath(const std::string& filePath)
	{
		const size_t lastIndex = filePath.find_last_of("\\/");

		if (lastIndex != std::string::npos)
		{
			return filePath.substr(0, lastIndex + 1); //To include the / itself.
		}

		return "";
	}

	std::string FileSystem::RetrieveFilePathWithoutExtension(const std::string& filePath)
	{
		return RetrieveDirectoryFromFilePath(filePath) + RetrieveFileNameFromFilePath(filePath);
	}

	std::string FileSystem::ReplaceExtention(const std::string& filePath, const std::string& fileExtension)
	{
		return RetrieveDirectoryFromFilePath(filePath) + RetrieveFileNameFromFilePath(filePath) + fileExtension;
	}

	std::string FileSystem::RetrieveExtensionFromFilePath(const std::string& filePath)
	{
		std::string fileExtension;

		//A system error is possible if the characters are something that cannot be converted, like Russian.
		try
		{
			fileExtension = std::filesystem::path(filePath).extension().generic_string();
		}
		catch (std::system_error& error)
		{

		}

		return fileExtension;
	}

	std::string FileSystem::NatifyFilePath(const std::string& filePath)
	{
		return std::string();
	}

	std::string FileSystem::RetrieveRelativeFilePath(const std::string& filePath)
	{
		if (std::filesystem::path(filePath).is_relative())
		{
			return filePath;
		}

		//Create absolute paths (complete system path).
		const std::filesystem::path p = std::filesystem::absolute(filePath);
		const std::filesystem::path r = std::filesystem::absolute(RetrieveWorkingDirectory());

		//If root paths are different, return absolute path.
		if (p.root_path() != r.root_path())
		{
			return p.generic_string();
		}

		//Initialize relative path.
		std::filesystem::path result;

		//Find out where the two paths diverge.
		std::filesystem::path::const_iterator iteratorPath = p.begin();
		std::filesystem::path::const_iterator iteratorRelativeTo = r.begin();

		while (*iteratorPath == *iteratorRelativeTo && iteratorPath != p.end() && iteratorRelativeTo != r.end())
		{
			++iteratorPath;
			++iteratorRelativeTo;
		}

		//Add "../" for each remaining token in relative to.
		if (iteratorRelativeTo != r.end())
		{
			++iteratorRelativeTo;
			while (iteratorRelativeTo != r.end())
			{
				result /= "..";
				++iteratorRelativeTo;
			}
		}

		//Add remaining path.
		while (iteratorPath != p.end())
		{
			result /= *iteratorPath;
			++iteratorPath;
		}

		return result.generic_string();
	}

	std::string FileSystem::RetrieveWorkingDirectory()
	{
		return std::filesystem::current_path().generic_string();
	}

	std::string FileSystem::RetrieveRootDirectory(const std::string& filePath)
	{
		return std::filesystem::path(filePath).root_directory().generic_string();
	}

	std::string FileSystem::RetrieveParentDirectory(const std::string& filePath)
	{
		return std::filesystem::path(filePath).parent_path().generic_string();
	}

	std::vector<std::string> FileSystem::RetrieveDirectoriesInDirectory(const std::string& directoryPath)
	{
		std::vector<std::string> directories;

		const std::filesystem::directory_iterator iteratorEnd;
		for (std::filesystem::directory_iterator iterator(directoryPath); iterator != iteratorEnd; ++iterator)
		{
			if (!std::filesystem::is_directory(iterator->status()))
			{
				continue;
			}

			std::string directoryPath;

			//A system error is possible if the characters are something that cannot be converted, like Russian.
			try
			{
				directoryPath = iterator->path().string();
			}
			catch (std::system_error& error)
			{

			}

			if (!directoryPath.empty())
			{
				//Finally, we save it.
				directories.emplace_back(directoryPath);
			}
		}

		return directories;
	}

	std::vector<std::string> FileSystem::RetrieveFilesInDirectory(const std::string& directoryPath)
	{
		std::vector<std::string> filePaths;

		const std::filesystem::directory_iterator iteratorEnd;
		for (std::filesystem::directory_iterator iterator(directoryPath); iterator != iteratorEnd; ++iterator)
		{
			if (!std::filesystem::is_regular_file(iterator->status()))
			{
				continue;
			}

			try
			{
				//A system error is possible if the characters are something that cannot be converted, like Russian.
				filePaths.emplace_back(iterator->path().string());
			}
			catch (std::system_error& error)
			{

			}
		}

		return filePaths;
	}

	bool FileSystem::IsSupportedAudioFile(const std::string& filePath)
	{
		const std::string extension = RetrieveExtensionFromFilePath(filePath);

		for (const std::string& format : SupportedAudioFormats)
		{
			if (extension == format || extension == ConvertToUppercaseString(format))
			{
				return true;
			}
		}

		return false;
	}

	bool FileSystem::IsSupportedImageFile(const std::string& filePath)
	{
		const std::string extension = RetrieveExtensionFromFilePath(filePath);

		for (const std::string& format : SupportedImageFormats)
		{
			if (extension == format || extension == ConvertToUppercaseString(format))
			{
				return true;
			}
		}

		if (RetrieveExtensionFromFilePath(filePath) == Extension_Texture)
		{
			return true;
		}

		return false;
	}

	bool FileSystem::IsSupportedModelFile(const std::string& filePath)
	{
		const std::string extension = RetrieveExtensionFromFilePath(filePath);

		for (const std::string& format : SupportedModelFormats)
		{
			if (extension == format || extension == ConvertToUppercaseString(format))
			{
				return true;
			}
		}

		return false;
	}

	bool FileSystem::IsSupportedShaderFile(const std::string& filePath)
	{
		const std::string extension = RetrieveExtensionFromFilePath(filePath);

		for (const std::string& format : SupportedShaderFormats)
		{
			if (extension == format || extension == ConvertToUppercaseString(format))
			{
				return true;
			}
		}

		return false;
	}

	bool FileSystem::IsSupportedFontFile(const std::string& filePath)
	{
		const std::string extension = RetrieveExtensionFromFilePath(filePath);

		for (const std::string& format : SupportedFontFormats)
		{
			if (extension == format || extension == ConvertToUppercaseString(format))
			{
				return true;
			}
		}

		return false;
	}

	bool FileSystem::IsEngineScriptFile(const std::string& filePath)
	{
		const std::string extension = RetrieveExtensionFromFilePath(filePath);

		for (const std::string& format : SupportedScriptFormats)
		{
			if (extension == format || extension == ConvertToUppercaseString(format))
			{
				return true;
			}
		}

		return false;
	}

	bool FileSystem::IsEnginePrefabFile(const std::string& filePath)
	{
		return RetrieveExtensionFromFilePath(filePath) == Extension_Prefab;
	}

	bool FileSystem::IsEngineMaterialFile(const std::string& filePath)
	{
		return RetrieveExtensionFromFilePath(filePath) == Extension_Material;
	}

	bool FileSystem::IsEngineMeshFile(const std::string& filePath)
	{
		return RetrieveExtensionFromFilePath(filePath) == Extension_Mesh;
	}

	bool FileSystem::IsEngineModelFile(const std::string& filePath)
	{
		return RetrieveExtensionFromFilePath(filePath) == Extension_Model;
	}

	bool FileSystem::IsEngineSceneFile(const std::string& filePath)
	{
		return RetrieveExtensionFromFilePath(filePath) == Extension_World;
	}

	bool FileSystem::IsEngineTextureFile(const std::string& filePath)
	{
		return RetrieveExtensionFromFilePath(filePath) == Extension_Texture;
	}

	bool FileSystem::IsEngineAudioFile(const std::string& filePath)
	{
		return RetrieveExtensionFromFilePath(filePath) == Extension_Audio;
	}

	bool FileSystem::IsEngineShaderFile(const std::string& filePath)
	{
		return RetrieveExtensionFromFilePath(filePath) == Extension_Shader;
	}

	bool FileSystem::IsEngineFile(const std::string& filePath)
	{
		return IsEngineScriptFile(filePath)		||
			   IsEnginePrefabFile(filePath)		||
			   IsEngineModelFile(filePath)		||
			   IsEngineMaterialFile(filePath)	||
			   IsEngineMeshFile(filePath)		||
			   IsEngineSceneFile(filePath)		||
			   IsEngineTextureFile(filePath)	||
			   IsEngineAudioFile(filePath)		||
			   IsEngineShaderFile(filePath);
	}

	std::vector<std::string> FileSystem::RetrieveSupportedFilesInDirectory(const std::string& directoryPath)
	{
		const std::vector<std::string> filesInDirectory = RetrieveFilesInDirectory(directoryPath);
		std::vector<std::string> imagesInDirectory = RetrieveSupportedImageFilesFromPaths(filesInDirectory);
		std::vector<std::string> scriptsInDirectory = RetrieveSupportedScriptFilesFromPaths(filesInDirectory);
		std::vector<std::string> modelsInDirectory = RetrieveSupportedModelFilesFromPaths(filesInDirectory);
		std::vector<std::string> supportedFiles;

		//Retrieve Supported Images
		for (const std::string& imageInDirectory : imagesInDirectory)
		{
			supportedFiles.emplace_back(imageInDirectory);
		}

		//Retrieve Supported Scripts
		for (const std::string& scriptInDirectory : scriptsInDirectory)
		{
			supportedFiles.emplace_back(scriptInDirectory);
		}

		//Retrieve Supported Models
		for (const std::string& modelInDirectory : modelsInDirectory)
		{
			supportedFiles.emplace_back(modelInDirectory);
		}

		return supportedFiles;
	}

	std::vector<std::string> FileSystem::RetrieveSupportedImageFilesFromPaths(const std::vector<std::string>& paths)
	{
		std::vector<std::string> imageFiles;
		for (const std::string& path : paths)
		{
			if (!IsSupportedImageFile(path))
			{
				continue;
			}

			imageFiles.emplace_back(path);
		}

		return imageFiles;
	}

	std::vector<std::string> FileSystem::RetrieveSupportedAudioFilesFromPaths(const std::vector<std::string>& paths)
	{
		std::vector<std::string> audioFiles;
		for (const std::string& path : paths)
		{
			if (!IsEngineAudioFile(path))
			{
				continue;
			}

			audioFiles.push_back(path);
		}

		return audioFiles;
	}

	std::vector<std::string> FileSystem::RetrieveSupportedScriptFilesFromPaths(const std::vector<std::string>& paths)
	{
		std::vector<std::string> scriptFiles;
		for (const std::string& path : paths)
		{
			if (!IsEngineScriptFile(path))
			{
				continue;
			}

			scriptFiles.emplace_back(path);
		}

		return scriptFiles;
	}

	std::vector<std::string> FileSystem::RetrieveSupportedModelFilesFromPaths(const std::vector<std::string>& paths)
	{
		std::vector<std::string> imageFiles;
		for (const std::string& path : paths)
		{
			if (!IsSupportedModelFile(path))
			{
				continue;
			}

			imageFiles.emplace_back(path);
		}

		return imageFiles;
	}

	std::vector<std::string> FileSystem::RetrieveSupportedModelFilesInDirectory(const std::string& directoryPath)
	{
		return RetrieveSupportedModelFilesFromPaths(RetrieveFilesInDirectory(directoryPath));
	}

	std::vector<std::string> FileSystem::RetrieveSupportedSceneFilesInDirectory(const std::string& directoryPath)
	{
		std::vector<std::string> sceneFiles;

		std::vector<std::string> files = RetrieveFilesInDirectory(directoryPath);
		for (const std::string& file : files)
		{
			if (!IsEngineSceneFile(file))
			{
				continue;
			}

			sceneFiles.emplace_back(file);
		}

		return sceneFiles;
	}
}