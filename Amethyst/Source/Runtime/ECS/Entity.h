#pragma once
#include <vector>
#include "../../Core/AmethystObject.h"
#include "Components/IComponent.h"

namespace Amethyst
{
	class Entity : public AmethystObject, public std::enable_shared_from_this<Entity> 
	{
	public:
		Entity(uint32_t transformID = 0);
		~Entity();

		void Clone();
		void Start();
		void Stop();
		void OnUpdate(float deltaTime);
		void Serialize(); ///
		void Deserialize(); ///

		//Properties
		const std::string& RetrieveName() const { return m_Name; }
		void SetName(const std::string& entityName) { m_Name = entityName; }

		bool IsActive() const { return m_IsEntityActive; }
		void SetActive(const bool& activeState) { m_IsEntityActive = activeState; }

		bool IsVisibleInHierarchy() const { return m_HierarchyVisibility; }
		void SetHierarchyVisibility(const bool& hierarchyVisibility) { m_HierarchyVisibility = hierarchyVisibility; }

		//Adds a component of type T.
		template <typename T>
		T* AddComponent(uint32_t componentID = 0)
		{
			const ComponentType type = IComponent::TypeToEnum<T>();

			//Return component in case it already exists while ignoring Script components as they can exist multiple times.
			if (HasComponent(type) && type != ComponentType::Script)
			{
				return GetComponent<T>();
			}

			//Create a new component.
			std::shared_ptr<T> component = std::make_shared<T>(this, componentID);

			//Save the new component.
			m_Components.emplace_back(std::static_pointer_cast<IComponent>(component));
			m_ComponentMask |= RetrieveComponentMask(type); //Turns on the bit in our mask.

			//Caching?

			//Initialize Component
			component->SetType(type);
			component->OnInitialization();

			//Make the scene resolve.

			return component.get(); //Returns the pointer to our newly added component.
		}

		IComponent* AddComponent(ComponentType type, uint32_t componentID = 0);

		//Return a component of type T if it exists.
		template<typename T>
		T* GetComponent()
		{

		}

		//Checks if a component exists.
		constexpr bool HasComponent(const ComponentType type) { return m_ComponentMask & RetrieveComponentMask(type); }

	private:
		constexpr uint32_t RetrieveComponentMask(ComponentType type) { return static_cast<uint32_t>(1) << static_cast<uint32_t>(type); } ///

	private:
		std::string m_Name = "Entity";
		bool m_IsEntityActive = true;
		bool m_HierarchyVisibility = true;

		//Components
		std::vector<std::shared_ptr<IComponent>> m_Components;
		uint32_t m_ComponentMask = 0;
	};
}