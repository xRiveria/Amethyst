#pragma once
#include "../../Core/ISubsystem.h"
#include <vector>
#include <string>

namespace Amethyst
{
	class Context;
	class Entity;

	class World : public ISubsystem
	{
	public:
		World(Context* engineContext);
		~World();

		// === Subsystem ===
		bool InitializeSubsystem() override;
		void OnUpdate(float deltaTime) override;
		
		void CreateNewWorld();
		/// Save World To File
		/// Load World From File
		const std::string& RetrieveWorldName() { return m_WorldName; }
		void ResolveWorld() { m_ResolveWorld = true; }
		bool IsWorldLoading();

		//Entities
		std::shared_ptr<Entity> EntityCreate(bool isActive = true);
		bool EntityExists(const std::shared_ptr<Entity>& entity);
		void EntityRemove(const std::shared_ptr<Entity>& entity);

		std::vector<std::shared_ptr<Entity>> RetrieveEntityRoots();
		const std::shared_ptr<Entity>& RetrieveEntityByName(const std::string& entityName);
		const std::shared_ptr<Entity>& RetrieveEntityByID(uint32_t entityID);
		const std::vector<std::shared_ptr<Entity>>& RetrieveAllEntities() { return m_Entities; }

	private:
		void ClearWorld();
		void _EntityRemove(const std::shared_ptr<Entity>& entity);

		// Common Entity Creation
		/// std::shared_ptr<Entity> CreateEnvironment();
		std::shared_ptr<Entity> CreateCamera();
		/// std::shared_ptr<Entity> CreateDirectionalLight();

	private:
		std::string m_WorldName;
		bool m_WasInEditorMode = false;
		bool m_ResolveWorld = true;
		/// Profiler Pointer.
		Input* m_Input = nullptr;

		std::vector<std::shared_ptr<Entity>> m_Entities;
	}; 
}