#include "Amethyst.h"
#include "World.h"
#include "Entity.h"
#include "../Source/Core/Context.h"

namespace Amethyst
{
	World::World(Context* context) : ISubsystem(context)
	{

	}

	World::~World()
	{

	}

	bool World::InitializeSubsystem()
	{
		//Create our default entities.
		CreateCamera();
		CreateEnvironment();
		CreateDirectionalLight();

		return true;
	}

	void World::OnUpdate(float deltaTime)
	{
		//If something is being loaded, don't update as entites are probably being added at the moment.
		if (IsWorldLoading())
		{
			return;
		}

		//Tick Entities

		const bool hasGameStarted = true; 
		const bool hasGameStopped = !hasGameStarted;
		//m_InEditorMode

		//Start
		if (hasGameStarted)
		{
			for (std::shared_ptr<Entity>& entity : m_Entities)
			{
				entity->Start();
			}
		}

		//Stop
		if (hasGameStopped)
		{
			for (std::shared_ptr<Entity>& entity : m_Entities)
			{
				entity->Stop();
			}
		}

		//Tick
		for (std::shared_ptr<Entity>& entity : m_Entities)
		{
			entity->OnUpdate(deltaTime);
		}

		//Update dirty entities.
		if (m_ResolveEntities)
		{
			//Make a copy so we can still iterate while removing entities.
			std::vector<std::shared_ptr<Entity>> entitiesCopied = m_Entities;

			for (std::shared_ptr<Entity>& entity : entitiesCopied)
			{
				if (entity->IsPendingDestruction())
				{
					_RemoveEntity(entity);
				}
			}

			//Notify Renderer
			m_ResolveEntities = false;
		}
	}

	void World::CreateNewWorld()
	{
		ClearWorld();
	}

	bool World::IsWorldLoading()
	{
		return false;
	}

	std::shared_ptr<Entity> World::CreateEntity(bool isActive)
	{
		std::shared_ptr<Entity> entity = m_Entities.emplace_back(std::make_shared<Entity>());
		entity->SetActive(isActive);

		return entity;
	}

	bool World::EntityExists(const std::shared_ptr<Entity>& entity)
	{
		if (!entity)
		{
			return false;
		}

		return RetrieveEntityByID(entity->RetrieveObjectID()) != nullptr;
	}

	void World::RemoveEntity(const std::shared_ptr<Entity>& entity)
	{
		if (!entity)
		{
			return;
		}

		//Mark for destruction but don't delete now as the Renderer might still be using it.
		entity->MarkForDestruction();
		m_ResolveEntities = true;
	}

	std::vector<std::shared_ptr<Entity>> World::RetrieveEntityRoots()
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

		//Can't find any Entity...
		static std::shared_ptr<Entity> emptyEntity;
		return emptyEntity;
	}

	void World::ClearWorld()
	{
		//Notify any systems that entities are about to be cleared.
		//Fire Event

		//Clear the entities.
		m_Entities.clear();
		m_ResolveEntities = true;
	}

	//Removes an enttiy and all of its children.
	void World::_RemoveEntity(const std::shared_ptr<Entity>& entity)
	{
		//Remove any descendants!

		//Keep a reference to its parent (in case it has one).
		
		//Remove this entity.
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

	std::shared_ptr<Entity> World::CreateEnvironment()
	{
		return std::shared_ptr<Entity>();
	}

	std::shared_ptr<Entity> World::CreateCamera()
	{
		return std::shared_ptr<Entity>();
	}

	std::shared_ptr<Entity> World::CreateDirectionalLight()
	{
		return std::shared_ptr<Entity>();
	}
}