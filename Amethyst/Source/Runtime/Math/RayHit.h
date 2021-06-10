#pragma once
#include <memory>
#include "../Core/AmethystDefinitions.h"
#include "Vector3.h"

namespace Amethyst
{
	class Entity;

	namespace Math
	{
		class RayHit
		{
		public:
			RayHit(const std::shared_ptr<Entity>& entity, const Vector3& position, float distance, bool isInside)
			{
				m_Entity = entity;
				m_Position = position;
				m_Distance = distance;
				m_IsInside = isInside;
			}

		public:
			std::shared_ptr<Entity> m_Entity;
			Vector3 m_Position;
			float m_Distance;
			bool m_IsInside;
		};
	}
}