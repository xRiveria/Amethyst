#pragma once
#include <memory>
#include <functional>
#include <any>
#include <vector>
#include "../../../Core/AmethystObject.h"
#include "Transform.h"

namespace Amethyst
{
	class Entity;

	enum class ComponentType : uint32_t
	{
		Camera,
		Collider,
		Light,
		Renderable,
		Script,
		Transform,
		Unknown
	};

	// Attributes/Properties of our component.
	struct Attribute 
	{
		std::function<std::any()> Getter; // std::any describes a type-safe container for single values of any type.
		std::function<void(std::any)> Setter;
	};

	class IComponent : public AmethystObject, public std::enable_shared_from_this<IComponent>
	{
	public:
		IComponent(Context* engineContext, Entity* entity, uint32_t componentID = 0, Transform* transform = nullptr);
		virtual ~IComponent() = default;

		// Runs when the component is added to an Entity.
		virtual void OnInitialize() {}

		// Runs every time the simulation starts.
		virtual void OnStart() {}

		// Runs every time the simulation stops.
		virtual void OnStop() {}

		// Runs every time the component is removed.
		virtual void OnRemove() {}

		// Runs every frame.
		virtual void OnUpdate(float deltaTime) {}

		// Runs when the entity is being saved.
		/// virtual void Serialize() {} 

		// Runs when the entity is being loaded.
		/// virtual void Deserialize() {} 

		// Type of our component.
		template<typename T>
		static constexpr ComponentType TypeToEnum();

		// Properties
		Context* RetrieveEngineContext() const { return m_Context; }
		Transform* RetrieveTransform() const { return m_Transform; }
		ComponentType RetrieveType() const { return m_Type; }
		void SetType(ComponentType type) { m_Type = type; }

		// Derive Pointer
		template<typename T>
		std::shared_ptr<T> RetrieveSharedPointer() { return std::dynamic_pointer_cast<T>(shared_from_this()); }

		// Attributes
		const std::vector<Attribute>& RetrieveAttributes() const { m_Attributes; }
		void SetAttribute(const std::vector<Attribute>& attributes)
		{ 
			for (uint32_t i = 0; i < static_cast<uint32_t>(m_Attributes.size()); i++)
			{
				m_Attributes[i].Setter(attributes[i].Getter());
			}
		}

		// Entity
		Entity* RetrieveEntity() const { return m_Entity; }
		std::string RetrieveEntityName() const;

	protected:
		#define REGISTER_ATTRIBUTE_GET_SET(getter, setter, type) RegisterAttribute(     \
        [this]() { return getter(); },                           \
        [this](const std::any& valueIn) { setter(std::any_cast<type>(valueIn)); });     \
		
		#define REGISTER_ATTRIBUTE_VALUE_SET(value, setter, type) RegisterAttribute(	\
		[this]() { return value; },														\
		[this](const std::any& valueIn) { setter(std::any_cast<type>(valueIn)); });		\
		
		#define REGISTER_ATTRIBUTE_VALUE_VALUE(value, type) RegisterAttribute(			\
		[this]() { return value; },														\
		[this](const std::any& valueIn) { value = std::any_cast<type>(valueIn); });		\

		// Registers an Attribute.
		void RegisterAttribute(std::function<std::any()>&& getter, std::function<void(std::any)>&& setter)
		{
			Attribute attribute;
			attribute.Getter = std::move(getter);
			attribute.Setter = std::move(setter);
			m_Attributes.emplace_back(attribute);
		}

	protected:
		// The type of the component.
		ComponentType m_Type = ComponentType::Unknown;

		// The state of the component.
		bool m_Enabled = false;

		// The owner of the component.
		Entity* m_Entity = nullptr;

		// The transform of the component (always exists).
		Transform* m_Transform = nullptr;

	private:
		// The attributes of the component.
		std::vector<Attribute> m_Attributes;
	};
}