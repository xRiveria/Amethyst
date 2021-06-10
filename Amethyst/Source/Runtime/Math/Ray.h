#pragma once
#include "Vector3.h"
#include "../Core/AmethystDefinitions.h"

namespace Amethyst
{
	class Context;

	namespace Math
	{
		class RayHit;
		class BoundingBox;
		class Plane;

		class Ray
		{
		public:
			Ray() = default;
			Ray(const Vector3& start, const Vector3& end);
			~Ray() = default;

			// Returns hit distance or infinity if there is no hit.
			float HitDistance(const BoundingBox& box) const; // Objects have a bounding box. We check if we hit this bounding box.
			float HitDistance(const Plane& plane, Vector3* intersectionPoint = nullptr) const; // Check if we hit a point on a plane.
			float HitDistance(const Vector3& vector1, const Vector3& vector2, const Vector3& vector3, Vector3* outNormal = nullptr, Vector3* outBary = nullptr) const;
			/// float HitDistance(const Sphere& sphere) const;

			const Vector3& RetrieveStart() const { return m_Start; }
			const Vector3& RetrieveEnd() const { return m_End; }
			const float& RetrieveLength() const { return m_Length; }
			const Vector3& RetrieveDirection() const { return m_Direction; }
		
		private:
			Vector3 m_Start;
			Vector3 m_End;
			Vector3 m_Direction;
			float m_Length = 0.0f;
		};
	}
}