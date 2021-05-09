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

		void SetResourceFilePath(const std::string& filePath)
		{

		}

		bool HasFilePathNative() const { return !m_ResourceFilePathNative.empty(); }
		const std::string& RetrieveResourceFilePathNative() const { return m_ResourceFilePathNative; }
		const std::string& RetrieveResourceName() const { return m_ResourceName; }
		ResourceType RetrieveResourceType() const { return m_ResourceType; }

		//Type
		template<typename T>
		static constexpr ResourceType TypeToEnum();

		//IO
		virtual bool SaveToFile(const std::string& filePath) { return true; }
		virtual bool LoadFromFile(const std::string& filePath) { return true; }

	protected:
		ResourceType m_ResourceType = ResourceType::Unknown;

	private:
		std::string m_ResourceName;
		std::string m_ResourceFilePathNative;
	};
}