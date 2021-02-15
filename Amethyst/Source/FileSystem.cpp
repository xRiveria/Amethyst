#include "FileSystem.h"
#include <filesystem>
#include <windows.h>
#include <WinUser.h>
#include <stringapiset.h>
#include <shellapi.h>
#include <fstream>

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
}