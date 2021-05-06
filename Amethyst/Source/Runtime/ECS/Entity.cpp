#include "Amethyst.h"
#include "Entity.h"
#include "Components/Transform.h"

namespace Amethyst
{
	Entity::Entity(uint32_t transformID)
	{
		m_Name = "Entity";
		m_IsEntityActive = true;
		m_HierarchyVisibility = true;

		//We can add our transform component here as all components will have a Transform component.
		//AddComponent<Transform>(transformID);
	}

	Entity::~Entity()
	{
		m_IsEntityActive = false;
		m_HierarchyVisibility = false;
		//m_Transform = nullptr;
		m_Name.clear();
		m_ComponentMask = 0;

		for (auto it = m_Components.begin(); it != m_Components.end();)
		{
			(*it)->OnRemove();
			(*it).reset();
			it = m_Components.erase(it);
		}

		m_Components.clear();
	}

	void Entity::Clone()
	{
		//Complex
	}

	void Entity::Start()
	{
		//Calls OnStart() on all the entity's components.
		for (const std::shared_ptr<IComponent>& component : m_Components)
		{
			component->OnStart();
		}
	}

	void Entity::Stop()
	{
		//Calls OnStop() on all the entity's components.
		for (const std::shared_ptr<IComponent>& component : m_Components)
		{
			component->OnStop();
		}
	}

	void Entity::OnUpdate(float deltaTime)
	{
		if (!m_IsEntityActive) //If the Entity isn't active, don't tick.
		{
			return;
		}

		//Calls OnUpdate() on all the entity's components.
		for (const std::shared_ptr<IComponent>& component : m_Components)
		{
			component->OnUpdate(deltaTime);
		}
	}

	void Entity::Serialize()
	{
		//Complex
	}

	void Entity::Deserialize()
	{
		//Complex
	}

	IComponent* Entity::AddComponent(ComponentType type, uint32_t componentID)
	{
		switch (type)
		{
			//case ComponentType::Transform: return AddComponent<Transform>(componentID);
			case ComponentType::Unknown: return nullptr;

			default: return nullptr;
		}

		return nullptr;
	}

	void Entity::RemoveComponentByID(uint32_t componentID)
	{
		ComponentType componentType = ComponentType::Unknown;

		for (auto it = m_Components.begin(); it != m_Components.end();)
		{
			std::shared_ptr<IComponent> component = *it;
			if (componentID == component->RetrieveObjectID())
			{
				componentType = component->RetrieveType();
				component->OnRemove();
				it = m_Components.erase(it);
				break;
			}
			else
			{
				++it;
			}
		}

		//The script component can have multiple instances, so we will only remove its flag if there are no more components of that type left.
		bool othersOfSameTypeExist = false;
		for (auto it = m_Components.begin(); it != m_Components.end(); ++it)
		{
			othersOfSameTypeExist = ((*it)->RetrieveType() == componentType) ? true : othersOfSameTypeExist;
		}

		if (!othersOfSameTypeExist)
		{
			m_ComponentMask &= ~RetrieveComponentMask(componentType);
		}

		//Resolve the scene.
	}
}