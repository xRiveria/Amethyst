#include "Amethyst.h"
#include "ResourceCache.h"

namespace Amethyst
{
	ResourceCache::ResourceCache(Context* context) : ISubsystem(context)
	{
		const std::string dataDirectory = "Resources\\";

		//Add engine standard resource directories.
		AddResourceDirectory(ResourceDirectory::Fonts, dataDirectory + "Fonts");
		AddResourceDirectory(ResourceDirectory::Icons, dataDirectory + "Icons");
		//More.

		//Create project directory.
		SetProjectDirectory("Project/");

		//Subscribe to events.
	}

	ResourceCache::~ResourceCache()
	{
		//Unsubscribe from events.
	}

	bool ResourceCache::OnInitialize()
	{
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
			if (resource->RetrieveResourceType() == type || type == ResourceType::Unknown) //If its unknown, it would mean the resource is farse.
			{
				if (AmethystObject* object = dynamic_cast<AmethystObject*>(resource.get())) //If our pointer is successfully casted (not zero)...
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

	void ResourceCache::ClearAllResources()
	{
		uint32_t resourceCount = static_cast<uint32_t>(m_Resources.size());

		m_Resources.clear();

		AMETHYST_INFO("%d resources have been cleared.", resourceCount);
	}

	void ResourceCache::SaveResourcesToFiles()
	{
	}

	void ResourceCache::LoadResourcesFromFiles()
	{

	}
}