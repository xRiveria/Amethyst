#include "Amethyst.h"
#include "IComponent.h"
#include "../Entity.h"
#include "Transform.h"

namespace Amethyst
{
	IComponent::IComponent(Entity* entity, uint32_t componentID)
	{
		m_Entity = entity;
		m_Enabled = true; //Is the component enabled?
	}

	std::string IComponent::RetrieveEntityName() const
	{
		if (!m_Entity)
		{
			return "";
		}

		return m_Entity->RetrieveName();
	}

	template<typename T>
	inline constexpr ComponentType IComponent::TypeToEnum()
	{
		return ComponentType::Unknown; //Unknown here, but set accordingly in derived classes.
	}

	template<typename T>
	inline constexpr void ValidateComponentType() 
	{ 
		static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent.");
	}

	//Explicit Template Instantiation
	#define REGISTER_COMPONENT(T, EnumT) template<> ComponentType IComponent::TypeToEnum<T>() { ValidateComponentType<T>(); return EnumT; }

	//To add a new component to the engine, simply add it here.
}