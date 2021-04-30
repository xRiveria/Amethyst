#pragma once
#include "../../Core/ISubsystem.h"
#include <vector>
#include <string>

namespace Amethyst
{
	class Entity;

	class World : public ISubsystem
	{
	public:
		World();
		~World();

		bool InitializeSubsystem() override;
		void OnUpdate(float deltaTime) override;
		
		void CreateNewWorld();
		bool IsWorldLoading();

		//Entities
		std::shared_ptr<Entity> CreateEntity(bool isActive = true);
		bool EntityExists(const std::shared_ptr<Entity>& entity);
		void RemoveEntity(const std::shared_ptr<Entity>& entity);
		std::vector<std::shared_ptr<Entity>> RetrieveEntityRoots();
		const std::shared_ptr<Entity>& RetrieveEntityByName(const std::string& entityName);
		const std::shared_ptr<Entity>& RetrieveEntityByID(uint32_t entityID);
		const std::vector<std::shared_ptr<Entity>>& RetrieveAllEntities() { return m_Entities; }

	private:
		void ClearWorld();
		void _RemoveEntity(const std::shared_ptr<Entity>& entity);

		//Common Entity Creation
		std::shared_ptr<Entity> CreateEnvironment();
		std::shared_ptr<Entity> CreateCamera();
		std::shared_ptr<Entity> CreateDirectionalLight();

	private:
		std::string m_Name;
		bool m_InEditorMode = false;
		bool m_ResolveEntities = true;

		std::vector<std::shared_ptr<Entity>> m_Entities;
	}; 
}