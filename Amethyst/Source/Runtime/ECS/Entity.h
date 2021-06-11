#pragma once
#include <vector>
#include "../../Event/EventSystem.h"
#include "Components/IComponent.h"

namespace Amethyst
{
	class Context;
	class Transform;
	class Renderable;

	class Entity : public AmethystObject, public std::enable_shared_from_this<Entity> 
	{
	public:
		Entity(Context* engineContext, uint32_t transformID = 0);
		~Entity();

		/// void Clone();
		void Start();
		void Stop();
		void OnUpdate(float deltaTime);
		/// void Serialize();
		/// void Deserialize();

		// === Properties ===
		const std::string& RetrieveName() const { return m_Name; }
		void SetName(const std::string& entityName) { m_Name = entityName; }

		bool IsActive() const { return m_IsEntityActive; }
		void SetActive(const bool& activeState) { m_IsEntityActive = activeState; }

		bool IsVisibleInHierarchy() const { return m_HierarchyVisibility; }
		void SetHierarchyVisibility(const bool& hierarchyVisibility) { m_HierarchyVisibility = hierarchyVisibility; }

		// =======

		// Adds a component of type T.
		template <typename T>
		T* AddComponent(uint32_t componentID = 0)
		{
			const ComponentType type = IComponent::TypeToEnum<T>();

			// Return component in case it already exists while ignoring Script components as they can exist multiple times.
			if (HasComponent(type)/*&& type != ComponentType::Script*/)
			{
				return GetComponent<T>();
			}

			// Create a new component.
			std::shared_ptr<T> component = std::make_shared<T>(m_Context, this, componentID);

			// Save the new component.
			m_Components.emplace_back(std::static_pointer_cast<IComponent>(component));
			m_ComponentMask |= RetrieveComponentMask(type);

			// Caching of rendering performance critical components.
			if constexpr (std::is_same<T, Transform>::value)  { m_Transform = static_cast<Transform*>(component.get()); }
			if constexpr (std::is_same<T, Renderable>::value) { m_Renderable = static_cast<Renderable*>(component.get()); }

			// Initialize Component.
			component->SetType(type);
			component->OnInitialize();

			// Make the scene resolve.
			FIRE_EVENT(EventType::WorldResolve);

			return component.get(); // Returns the pointer to our newly added component.
		}

		IComponent* AddComponent(ComponentType type, uint32_t componentID = 0);

		//Return a component of type T if it exists.
		template<typename T>
		T* GetComponent()
		{
			const ComponentType type = IComponent::TypeToEnum<T>();

			if (!HasComponent(type))
			{
				return nullptr;
			}

			for (const std::shared_ptr<IComponent>& component : m_Components)
			{
				if (component->RetrieveType() == type)
				{
					return static_cast<T*>(component.get());
				}
			}

			return nullptr;
		}

		//Return any components of type T if they exist.
		template<typename T>
		std::vector<T*> GetComponents()
		{
			std::vector<T*> components;
			const ComponentType type = IComponent::TypeToEnum<T>();

			if (!HasComponent(type))
			{
				return components;
			}

			for (const std::shared_ptr<IComponent>& component : m_Components)
			{
				if (component->RetrieveType() != type)
				{
					continue;
				}

				components.emplace_back(static_cast<T*>(component.get()));
			}

			return components;
		}

		//Checks if a component exists.
		constexpr bool HasComponent(const ComponentType type) { return m_ComponentMask & RetrieveComponentMask(type); }

		template<typename T>
		bool HasComponent() { return HasComponent(IComponent::TypeToEnum<T>()); }

		//Removes a component if it exists.
		template<typename T>
		void RemoveComponent()
		{
			const ComponentType type = IComponent::TypeToEnum<T>();

			for (auto it = m_Components.begin; it != m_Components.end();)
			{
				std::shared_ptr<IComponent> component = *it;
				if (component->RetrieveType() == type)
				{
					component->OnRemove();
					it = m_Components.erase(it);
					m_ComponentMask &= ~RetrieveComponentMask(type);
				}
				else
				{
					++it;
				}
			}

			// Make the scene resolve.
			FIRE_EVENT(EventType::WorldResolve);
		}

		void RemoveComponentByID(uint32_t componentID);
		const std::vector<std::shared_ptr<IComponent>>& RetrieveAllComponents() const { return m_Components; }
		
		void MarkForDestruction() { m_DestructionPending = true; }
		bool IsPendingDestruction() const { return m_DestructionPending; }

		Transform* RetrieveTransform() const { return m_Transform; }
		Renderable* RetrieveRenderable() const { return m_Renderable; }
		std::shared_ptr<Entity> RetrieveSharedPointer() { return shared_from_this(); }

	private:
		constexpr uint32_t RetrieveComponentMask(ComponentType type) { return static_cast<uint32_t>(1) << static_cast<uint32_t>(type); }

	private:
		std::string m_Name = "Entity";
		bool m_IsEntityActive = true;
		bool m_HierarchyVisibility = true;
		bool m_DestructionPending = false;

		// Components
		Transform* m_Transform = nullptr;
		Renderable* m_Renderable = nullptr;

		std::vector<std::shared_ptr<IComponent>> m_Components;
		uint32_t m_ComponentMask = 0;
	};
}