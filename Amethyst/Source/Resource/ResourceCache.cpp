#include "Amethyst.h"
#include "ResourceCache.h"

namespace Amethyst
{
	ResourceCache::ResourceCache(Context* context) : ISubsystem(context)
	{

	}

	ResourceCache::~ResourceCache()
	{
	}

	bool ResourceCache::InitializeSubsystem()
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

	uint32_t ResourceCache::RetrieveResourceCount(ResourceType type)
	{
		return uint32_t();
	}

	void ResourceCache::ClearAllResources()
	{
	}

	void ResourceCache::AddResourceDirectory(ResourceDirectory resourceType, const std::string& directory)
	{
	}

	std::string ResourceCache::RetrieveResourceDirectory(ResourceDirectory resourceType)
	{
		return std::string();
	}

	void ResourceCache::SetProjectDirectory(const std::string& projectDirectory)
	{
	}

	std::string ResourceCache::RetrieveProjectDirectoryAbsolute() const
	{
		return std::string();
	}

	void ResourceCache::SaveResourcesToFiles()
	{
	}

	void ResourceCache::LoadResourcesFromFiles()
	{
	}
}