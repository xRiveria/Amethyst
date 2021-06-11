#pragma once
#include "IResource.h"
#include "ISubsystem.h"
#include <thread>
#include <string>
#include <vector>
#include <unordered_map>

// The cache is the central resource repositary that holds all the engine's resources together.

namespace Amethyst
{
	/// Forward declare importers.
	/// class FontImporter;
	/// class ImageImporter;
	/// class ModelImporter;

	enum class ResourceDirectory
	{
		Cubemaps,
		Fonts,
		Icons,
		Scripts,
		ShaderCompiler,
		Shaders,
		Textures
	};

	class ResourceCache : public ISubsystem
	{
	public:
		ResourceCache(Context* engineContext);
		~ResourceCache();

		// === ISubsystem ===
		bool OnInitialize() override;
		// ======

		// Retrieve resource by name.
		std::shared_ptr<IResource>& RetrieveResourceByName(const std::string& resourceName, ResourceType resourceType);

		template<typename T>
		constexpr std::shared_ptr<T> RetrieveResourceByName(const std::string& resourceName)
		{
			return std::static_pointer_cast<T>(RetrieveResourceByName(resourceName, IResource::TypeToEnum<T>())); // Returns a new shared_ptr instance that shares ownership with the initial value.
		}

		// Retrieve resourcse by type.
		std::vector<std::shared_ptr<IResource>> RetrieveResourcesByType(ResourceType type = ResourceType::Unknown);

		// Retrieve resource by path.
		template<typename T>
		std::shared_ptr<T> RetrieveResourceByPath(const std::string& resourcePath)
		{
			for (std::shared_ptr<IResource>& resource : m_Resources)
			{
				if (resourcePath == resource->RetrieveResourceFilePathNative())
				{
					return std::static_pointer_cast<T>(resource);
				}
			}

			AMETHYST_WARNING("Resource located at: %s cannot be found.", resourcePath.c_str());
			return nullptr;
		}

		// Caches a resource, or replaces it with an existing cached resource.
		template<typename T>
		[[nodiscard]] std::shared_ptr<T> CacheResource(const std::shared_ptr<T>& resource)
		{
			// Validate resource.
			if (!resource)
			{
				return nullptr;
			}

			// Validate resource file path.
			if (!resource->HasFilePathNative() && !FileSystem::IsDirectory(resource->RetrieveResourceFilePathNative()))
			{
				AMETHYST_ERROR("A resource must have a valid file path in order to be cached.");
				return nullptr;
			}

			if (!FileSystem::IsEngineFile(resource->RetrieveResourceFilePathNative()))
			{
				AMETHYST_ERROR("A resource must have a native file format in order to be cached. Provided format was %s.", FileSystem::RetrieveExtensionFromFilePath(resource->RetrieveResourceFilePathNative().c_str()));
				return nullptr;
			}

			//Ensure that this resource is not already cached.
			if (IsCached(resource->RetrieveResourceName(), resource->RetrieveResourceType()))
			{
				return RetrieveResourceByName<T>(resource->RetrieveResourceName());
			}

			// Prevent threads from colliding in this critical section.

			std::lock_guard<std::mutex> cacheMutex(m_CacheMutex);

			// For deserialization purposes, we save it now.
			/// Cache the resource - save it to a file.

			// Cache it.
			return std::static_pointer_cast<T>(m_Resources.emplace_back(resource));
		}

		bool IsCached(const std::string& resourceName, ResourceType resourceType);

		template<typename T>
		void RemoveResource(std::shared_ptr<T>& resource)
		{
			if (!resource)
			{
				return;
			}

			if (!IsCached(resource->RetrieveResourceName(), resource->RetrieveResourceType()))
			{
				return;
			}

			// Remove if the object IDs match. std::remove_if will remove all elements that satisfy the range.
			m_Resources.erase(
				std::remove_if(
					m_Resources.begin(), m_Resources.end(),
					[](std::shared_ptr<IResource> resource) { return dynamic_cast<AmethystObject*>(resource.get())->RetrieveObjectID() == resource->RetrieveObjectID(); }),
					m_Resources.end());
		}

		// Loads a resource and adds it to the resource cache.
		template<typename T>
		std::shared_ptr<T> LoadResource(const std::string& resourcePath)
		{
			if (!FileSystem::Exists(resourcePath))
			{
				AMETHYST_ERROR("The resource at \"%s\" does not exist.", resourcePath.c_str());
				return nullptr;
			}

			// Check if the resource is already loaded.
			const std::string resourceName = FileSystem::RetrieveFileNameWithNoExtensionFromFilePath(resourcePath);
			if (IsCached(resourceName, IResource::TypeToEnum<T>()))
			{
				return RetrieveResourceByName<T>(resourceName);
			}

			//Create a new resource.
			auto resource = std::make_shared<T>(m_EngineContext);

			// Set a default file path in case its not overridden from LoadFromFile().
			resource->SetResourceFilePath(resourcePath);

			//Load
			if (!resource || !resource->LoadFromFile(resourcePath))
			{
				AMETHYST_ERROR("Failed to load Resource at \"%s\".", resourcePath.c_str());
				return nullptr;
			}

			// Return cached reference which is guarenteed to be around after a deserialization.
			return CacheResource<T>(resource);
		}

		// Misc - Memory - Could be useful for some tool to use for diagnostics.
		uint64_t RetrieveMemoryUsageCPU(ResourceType type = ResourceType::Unknown);
		uint64_t RetrieveMemoryUsageGPU(ResourceType type = ResourceType::Unknown);
		// Returns all resources of a given type.
		uint32_t RetrieveResourceCount(ResourceType type = ResourceType::Unknown);
		void Reset();

		// Resource Directories
		void AddResourceDirectory(ResourceDirectory resourceType, const std::string& directory);
		std::string RetrieveResourceDirectory(ResourceDirectory resourceType);
		std::string RetrieveResourceDirectory() const { return "Resources"; }

		// Project Directories
		void SetProjectDirectory(const std::string& projectDirectory);
		const std::string& RetrieveProjectDirectory() const { return m_ProjectDirectory; }
		std::string RetrieveProjectDirectoryAbsolute() const;

	private:
		/// Event Handlers
		/// void SaveResourcesToFiles();
		/// void LoadResourcesFromFiles();

	private:
		// Cache
		std::vector<std::shared_ptr<IResource>> m_Resources;
		std::mutex m_CacheMutex;

		// Directories
		std::string m_ProjectDirectory;
		std::unordered_map<ResourceDirectory, std::string> m_ResourceDirectories;

		/// Importers
		/// Model Importers
		/// Image Importers
		/// Font Importers
	};
}