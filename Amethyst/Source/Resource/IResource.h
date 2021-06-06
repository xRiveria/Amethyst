#pragma once
#include "../Runtime/Log/Log.h"
#include "../Core/AmethystObject.h"
#include "../Core/FileSystem.h"

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

	class IResource : public AmethystObject
	{
	public:
		IResource(Context* context, ResourceType resourceType);
		virtual ~IResource() = default;

		void SetResourceFilePath(const std::string& filePath)
		{
			const bool isNativeEngineFile = FileSystem::IsEngineMaterialFile(filePath) || FileSystem::IsEngineModelFile(filePath);

			//If this is a native engine file, a file check won't be needed.
			if (!isNativeEngineFile)
			{
				if (!FileSystem::IsFile(filePath))
				{
					AMETHYST_ERROR("\"%s\" is not a valid file path.", filePath.c_str());
				}
			}

			const std::string filePathRelative = FileSystem::RetrieveRelativeFilePath(filePath);

			//Foreign file.
			if (!FileSystem::IsEngineFile(filePath))
			{
				m_ResourceFilePathForeign = filePathRelative;
				m_ResourceFilePathNative = FileSystem::NatifyFilePath(filePathRelative);
			}
			else //Native file.
			{
				m_ResourceFilePathForeign.clear();
				m_ResourceFilePathNative = filePathRelative;
			}

			m_ResourceName = FileSystem::RetrieveFileNameWithoutExtension(filePathRelative);
			m_ResourceDirectory = FileSystem::RetrieveDirectoryFromFilePath(filePathRelative);

			AMETHYST_INFO("Loaded resource: %s", filePathRelative.c_str());
		}

		ResourceType RetrieveResourceType() const { return m_ResourceType; }
		const char* RetrieveResourceTypeCString() const { return typeid(*this).name(); }
		bool HasFilePathNative() const { return !m_ResourceFilePathNative.empty(); }
		const std::string& RetrieveResourceFilePath() const { return m_ResourceFilePathForeign; }
		const std::string& RetrieveResourceFilePathNative() const { return m_ResourceFilePathNative; }
		const std::string& RetrieveResourceName() const { return m_ResourceName; }
		const std::string& RetrieveResourceFileName() const { return m_ResourceName; }
		const std::string& RetrieveResourceDirectory() const { return m_ResourceDirectory; }

		//Type
		template<typename T>
		static constexpr ResourceType TypeToEnum();

		// Misc
		LoadState RetrieveLoadState() const { return m_LoadState; }

		//IO
		virtual bool SaveToFile(const std::string& filePath) { return true; }
		virtual bool LoadFromFile(const std::string& filePath) { return true; }

	protected:
		ResourceType m_ResourceType = ResourceType::Unknown;
		LoadState m_LoadState = LoadState::Idle;

	private:
		std::string m_ResourceName;
		std::string m_ResourceDirectory;
		std::string m_ResourceFilePathNative;
		std::string m_ResourceFilePathForeign;
	};
}