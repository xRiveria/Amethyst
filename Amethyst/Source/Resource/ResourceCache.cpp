#include "Amethyst.h"
#include "ResourceCache.h"

namespace Amethyst
{
	ResourceCache::ResourceCache(Context* engineContext) : ISubsystem(engineContext)
	{
		/* Quick Note on \\ for Paths
		
			\\ is how we express the \ character in C or C++. If you specify \ alone, that means the compiler expects some special code. For example, 
			\t is tab, \r is carriage return, \n is line feed and \\ is \.

			We will stick to using \ for path seperators instead of the backward slash /. While both works on windows, \ can prevent special handling of backslash characters.	
		*/
		const std::string dataDirectory = "Resources\\";

		// Add engine standard resource directories.
		AddResourceDirectory(ResourceDirectory::Fonts, dataDirectory + "Fonts");
		AddResourceDirectory(ResourceDirectory::Icons, dataDirectory + "Icons");
		AddResourceDirectory(ResourceDirectory::Textures, dataDirectory + "Textures");
		AddResourceDirectory(ResourceDirectory::Shaders, dataDirectory + "Shaders");
		AddResourceDirectory(ResourceDirectory::ShaderCompiler, dataDirectory + "ShaderCompiler");
		AddResourceDirectory(ResourceDirectory::Cubemaps, dataDirectory + "Cubemaps");
		/// More.

		// Create project directory.
		SetProjectDirectory("Project/");

		// Subscribe to events.
		/// SUBSCRIBE_TO_EVENT(EventType::WorldSave, EVENT_HANDLER(SaveResourcesToFiles));
		/// SUBSCRIBE_TO_EVENT(EventType::WorldLoad, EVENT_HANDLER(LoadResourcesFromFiles));
	}

	ResourceCache::~ResourceCache()
	{
		// Unsubscribe from events.
		/// UNSUBSCRIBE_FROM_EVENT(EventType::WorldSave, EVENT_HANDLER(SaveResourcesToFiles));
		/// UNSUBSCRIBE_FROM_EVENT(EventType::WorldLoad, EVENT_HANDLER(LoadResourcesFromFiles));
	}

	bool ResourceCache::OnInitialize()
	{
		/// Create our ImageImporter, ModelImporter and FontImporter instances.
		return true;
	}

	std::shared_ptr<IResource>& ResourceCache::RetrieveResourceByName(const std::string& resourceName, ResourceType resourceType)
	{
		for (std::shared_ptr<IResource>& resource : m_Resources)
		{
			if (resourceName == resource->RetrieveResourceName())
			{
				return resource;
			}
		}

		static std::shared_ptr<IResource> emptyResource;
		return emptyResource;
	}

	std::vector<std::shared_ptr<IResource>> ResourceCache::RetrieveResourcesByType(ResourceType type)
	{
		std::vector<std::shared_ptr<IResource>> resources;

		for (std::shared_ptr<IResource>& resource : m_Resources)
		{
			if (resource->RetrieveResourceType() == type || type == ResourceType::Unknown)
			{
				resources.emplace_back(resource);
			}
		}

		return resources;
	}

	bool ResourceCache::IsCached(const std::string& resourceName, ResourceType resourceType)
	{
		if (resourceName.empty())
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return false;
		}

		for (std::shared_ptr<IResource>& resource : m_Resources)
		{
			if (resourceName == resource->RetrieveResourceName())
			{
				return true;
			}
		}

		return false;
	}

	uint64_t ResourceCache::RetrieveMemoryUsageCPU(ResourceType type)
	{
		uint64_t totalSize = 0;

		for (std::shared_ptr<IResource>& resource : m_Resources)
		{
			if (resource->RetrieveResourceType() == type || type == ResourceType::Unknown) // If its unknown, it would mean the resource is farse.
			{
				if (AmethystObject* object = dynamic_cast<AmethystObject*>(resource.get()))
				{
					totalSize += object->RetrieveCPUSize();
				}
			}
		}

		return totalSize;
	}

	uint64_t ResourceCache::RetrieveMemoryUsageGPU(ResourceType type)
	{
		uint64_t totalSize = 0;

		for (std::shared_ptr<IResource>& resource : m_Resources)
		{
			if (resource->RetrieveResourceType() == type || type == ResourceType::Unknown)
			{
				if (AmethystObject* object = dynamic_cast<AmethystObject*>(resource.get()))
				{
					totalSize += object->RetrieveGPUSize();
				}
			}
		}

		return totalSize;
	}

	void ResourceCache::AddResourceDirectory(ResourceDirectory resourceType, const std::string& directory)
	{
		m_ResourceDirectories[resourceType] = directory;
	}

	std::string ResourceCache::RetrieveResourceDirectory(ResourceDirectory resourceType)
	{
		for (std::pair<ResourceDirectory, std::string> directory : m_ResourceDirectories)
		{
			if (directory.first == resourceType)
			{
				return directory.second;
			}
		}

		return "";
	}

	void ResourceCache::SetProjectDirectory(const std::string& projectDirectory)
	{
		if (!FileSystem::Exists(projectDirectory))
		{
			FileSystem::CreateDirectory_(projectDirectory);
		}

		m_ProjectDirectory = projectDirectory;
	}

	std::string ResourceCache::RetrieveProjectDirectoryAbsolute() const
	{
		return FileSystem::RetrieveWorkingDirectory() + "/" + m_ProjectDirectory;
	}

	uint32_t ResourceCache::RetrieveResourceCount(ResourceType type)
	{
		return static_cast<uint32_t>(RetrieveResourcesByType(type).size());
	}

	void ResourceCache::Reset()
	{
		uint32_t resourceCount = static_cast<uint32_t>(m_Resources.size());

		m_Resources.clear();

		AMETHYST_INFO("%d resources have been cleared.", resourceCount);
	}
}