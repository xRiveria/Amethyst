#include "Amethyst.h"
#include "IComponent.h"
#include "Transform.h"
#include "Renderable.h"
#include "Camera.h"
#include "../Entity.h"

namespace Amethyst
{
	IComponent::IComponent(Context* engineContext, Entity* entity, uint32_t componentID /*= 0*/, Transform* transform /*= nullptr*/)
	{
		m_Context = engineContext;
		m_Entity = entity;
		m_Transform = transform ? transform : entity->RetrieveTransform();
		m_Enabled = true; // Is the component enabled?
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
		return ComponentType::Unknown; // Unknown here, but set accordingly in derived classes.
	}

	template<typename T>
	inline constexpr void ValidateComponentType() 
	{ 
		static_assert(std::is_base_of<IComponent, T>::value, "Provided type does not implement IComponent.");
	}

	// Explicit template instantiation.
	#define REGISTER_COMPONENT(T, enumT) template<> ComponentType IComponent::TypeToEnum<T>() { ValidateComponentType<T>(); return enumT; }

	// To add a new component to the engine, simply register it here;
	REGISTER_COMPONENT(Camera, ComponentType::Camera)
	REGISTER_COMPONENT(Renderable, ComponentType::Renderable)
	REGISTER_COMPONENT(Transform, ComponentType::Transform)
}