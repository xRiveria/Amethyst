#pragma once
#include "Core/AmethystObject.h"
#include "FileSystem.h"

namespace Amethyst
{
	enum class ResourceType
	{
		Unknown,
		Texture,
		Texture2D,
		TextureCube,
		Audio,
		Material,
		Mesh,
		Model,
		Cubemap,
		Animation,
		Font,
		Shader
	};

	enum class LoadState
	{
		Idle,
		Started,
		Completed,
		Failed
	};

	class Resource : public AmethystObject
	{
	public:
		Resource(const ResourceType resourceType);
		virtual ~Resource() = default;

		void SetResourceFilePath(const std::string& filePath)
		{
			const bool isNativeEngineFile = FileSystem::IsEngineMaterialFile(filePath) || FileSystem::IsEngineModelFile(filePath);

			//If this is a native engine file, don't do a file check as no actual foreign material exists (it was created on the fly).
			if (!isNativeEngineFile)
			{
				if (!FileSystem::IsFile(filePath))
				{
					//Log that it is not a valid file path.
					return;
				}
			}

			const std::string relativeFilePath = FileSystem::RetrieveRelativeFilePath(filePath);

			//Foreign File
			if (!FileSystem::IsEngineFile(filePath))
			{
				m_ResourceFilePathForeign = relativeFilePath;
				m_ResourceFilePathNative = FileSystem::NatifyFilePath(relativeFilePath);
			}
			else
			{
				m_ResourceFilePathForeign.clear();
				m_ResourceFilePathNative = relativeFilePath;
			}

			m_ResourceName = FileSystem::RetrieveFileNameWithNoExtensionFromFilePath(relativeFilePath);
			m_ResourceDirectory = FileSystem::RetrieveDirectoryFromFilePath(relativeFilePath);
		}

		ResourceType RetrieveResourceType() const { return m_ResourceType; }
		const char* RetrieveResourceTypeCString() const { return typeid(*this).name(); }
		bool HasFilePathNative() const { return !m_ResourceFilePathNative.empty(); }

		const std::string& RetrieveResourceFilePath() const { return m_ResourceFilePathForeign; }
		const std::string& RetrieveResourceFilePathNative() const { return m_ResourceFilePathNative; }
		const std::string& RetrieveResourceName() const { return m_ResourceName; }
		const std::string& RetrieveResourceFileName() { return m_ResourceName; }
		const std::string& RetrieveResourceDirectory() { return m_ResourceDirectory; }

		//Misc
		LoadState RetrieveLoadState() const { return m_ResourceLoadState; }

		//IO
		virtual bool SaveToFile(const std::string& filePath) { return true; }
		virtual bool LoadFromFile(const std::string& filePath) { return true; }

		//Type
		template<typename T>
		static constexpr ResourceType TypeToEnum();

	protected:
		ResourceType m_ResourceType = ResourceType::Unknown;
		LoadState m_ResourceLoadState = LoadState::Idle;

	private:
		std::string m_ResourceName;
		std::string m_ResourceDirectory;
		std::string m_ResourceFilePathNative;
		std::string m_ResourceFilePathForeign;
	};
}
