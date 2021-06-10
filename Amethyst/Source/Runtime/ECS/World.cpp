#include "Amethyst.h"
#include "World.h"
#include "Entity.h"
#include "../Source/Core/Context.h"
#include "../../Rendering/Renderer.h"
#include "../../Resource/ResourceCache.h"
#include "Components/Transform.h"

namespace Amethyst
{
	World::World(Context* engineContext) : ISubsystem(engineContext)
	{
		// Subscribe world to events.
		SUBSCRIBE_TO_EVENT(EventType::WorldResolve, [this](Variant) { m_ResolveWorld = true; });
	}

	World::~World()
	{
		m_Input = nullptr;
	}

	bool World::InitializeSubsystem()
	{
		m_Input = m_EngineContext->RetrieveSubsystem<Input>();
		/// Retrieve Profiler from Engine Context.

		//Create our default entities.
		CreateCamera();
		/// CreateEnvironment();
		/// CreateDirectionalLight();

		return true;
	}

	void World::OnUpdate(float deltaTime)
	{
		//If something is being loaded, don't tick as entities are probably being added at the moment.
		if (IsWorldLoading())
		{
			return;
		}

		// Tick Entities
		{
			// Detect mode toggling between editor and play modes.
			const bool isPlayModeActivated = m_EngineContext->m_Engine->EngineMode_IsToggled(EngineMode::Engine_Game) && m_WasInEditorMode;
			const bool isPlayModeStopped = !m_EngineContext->m_Engine->EngineMode_IsToggled(EngineMode::Engine_Game) && !m_WasInEditorMode;
			m_WasInEditorMode = !m_EngineContext->m_Engine->EngineMode_IsToggled(EngineMode::Engine_Game); // We're in editor mode if our engine isn't set to play.

			// Initialize each of our entity's components.
			if (isPlayModeActivated)
			{
				for (std::shared_ptr<Entity>& entity : m_Entities)
				{
					entity->Start();
				}
			}

			// Likewise, we stop them.
			if (isPlayModeStopped)
			{
				for (std::shared_ptr<Entity>& entity : m_Entities)
				{
					entity->Stop();
				}
			}

			// Tick our Entities.
			for (std::shared_ptr<Entity>& entity : m_Entities)
			{
				entity->OnUpdate(deltaTime);
			}
		}

		// Resolve World
		if (m_ResolveWorld) // If our world should be resolved...
		{
			// Update "dirty" entities.
			{
				//Make a copy so we can still iterate while removing entities.
				std::vector<std::shared_ptr<Entity>> entitiesCopied = m_Entities;

				for (std::shared_ptr<Entity>& entity : entitiesCopied)
				{
					if (entity->IsPendingDestruction())
					{
						_EntityRemove(entity);
					}
				}
			}

			//Notify Renderer
			FIRE_EVENT_DATA(EventType::WorldResolved, m_Entities);
			m_ResolveWorld = false;
		}
	}

	void World::CreateNewWorld()
	{
		ClearWorld();
	}

	void World::ClearWorld()
	{
		// Notify any systems that entities are about to be cleared.
		FIRE_EVENT(EventType::WorldClear);
		m_EngineContext->RetrieveSubsystem<Renderer>()->ClearEntities();
		m_EngineContext->RetrieveSubsystem<ResourceCache>()->ClearAllResources();

		//Clear the entities.
		m_Entities.clear();

		m_ResolveWorld = true;
	}

	bool World::IsWorldLoading()
	{
		ProgressTracker& progressReport = ProgressTracker::RetrieveInstance();

		const bool isLoadingModel = progressReport.RetrieveLoadStatus(ProgressType::ModelImporter);
		const bool isLoadingScene = progressReport.RetrieveLoadStatus(ProgressType::World);

		return isLoadingModel || isLoadingScene;
	}

	std::shared_ptr<Entity> World::EntityCreate(bool isActive /*= true*/)
	{
		std::shared_ptr<Entity> entity = m_Entities.emplace_back(std::make_shared<Entity>());
		entity->SetActive(isActive);

		return entity;
	}

	std::vector<std::shared_ptr<Entity>> World::RetrieveEntityRoots() ///
	{
		std::vector<std::shared_ptr<Entity>> rootEntities;

		for (const std::shared_ptr<Entity>& entity : rootEntities)
		{
			/*
			if (entity->RetrieveTransform()->IsRootEntity())
			{
				rootEntities.emplace_back(entity);
			}
			*/
		}

		return rootEntities;
	} 

	const std::shared_ptr<Entity>& World::RetrieveEntityByName(const std::string& entityName)
	{
		for (const std::shared_ptr<Entity>& entity : m_Entities)
		{
			if (entity->RetrieveName() == entityName)
			{
				return entity;
			}
		}

		//Can't find any Entity...
		static std::shared_ptr<Entity> emptyEntity;
		return emptyEntity;
	}

	const std::shared_ptr<Entity>& World::RetrieveEntityByID(uint32_t entityID)
	{
		for (const std::shared_ptr<Entity>& entity : m_Entities)
		{
			if (entity->RetrieveObjectID() == entityID)
			{
				return entity;
			}
		}

		//Can't find any Entity... Note that in the simplest case, all static variables from the same translation unit are seen by the linker as a single blob of data.
		static std::shared_ptr<Entity> emptyEntity;
		return emptyEntity;
	}

	bool World::EntityExists(const std::shared_ptr<Entity>& entity)
	{
		if (!entity)
		{
			return false;
		}

		return RetrieveEntityByID(entity->RetrieveObjectID()) != nullptr;
	}

	void World::EntityRemove(const std::shared_ptr<Entity>& entity)
	{
		if (!entity)
		{
			return;
		}

		// Mark for destruction but don't delete now as the Renderer might still be using it.
		entity->MarkForDestruction();

		m_ResolveWorld = true;
	}

	//Removes an entity and all of its children.
	void World::_EntityRemove(const std::shared_ptr<Entity>& entity)
	{
		// Remove any descendants!

		// Keep a reference to its parent (in case it has one).
		
		// Remove this entity.
		for (auto it = m_Entities.begin(); it < m_Entities.end();)
		{
			const std::shared_ptr<Entity> temporary = *it;
			if (temporary->RetrieveObjectID() == entity->RetrieveObjectID())
			{
				it = m_Entities.erase(it);
				break;
			}
			++it;
		}

		//If there was a parent, update it.
	}

	std::shared_ptr<Entity> World::CreateCamera()
	{
		return std::shared_ptr<Entity>();
	}
}