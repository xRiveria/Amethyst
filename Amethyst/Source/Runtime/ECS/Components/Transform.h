#pragma once
#include "IComponent.h"
#include <vector>
#include <glm/glm.hpp>

namespace Amethyst
{
	class Transform : public IComponent
	{
	public:
		Transform(Entity* entity, uint32_t componentId = 0);
		~Transform() = default;

		void OnInitialization() override;
		void Serialize() override; ///
		void Deserialize() override; ///

		//Transform Component
		void UpdateTransform();

		//Position

		//Rotation

		//Scale

		//Translation/Rotation

		//Directions

		//Hierarchy
	};
}