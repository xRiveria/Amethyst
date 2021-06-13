#pragma once

namespace Amethyst::Math
{
	class Plane
	{
	public:
		Plane() = default;

		Plane(const Vector3& normal, float distanceFromOrigin);

		// Construct from a normal vector and a point on the plane.
		Plane(const Vector3& normal, const Vector3& point);

		// Construct from 3 vertices.
		Plane(const Vector3& a, const Vector3& b, const Vector3& c);

		~Plane() = default;

		void Normalize();
		static Plane Normalize(const Plane& plane);

		float Dot(const Vector3& vector) const;
		static float Dot(const Plane& plane, const Vector3& vector);

	public:
		Vector3 m_Normal = Vector3::Zero;
		float m_DistanceFromOrigin = 0.0f;
	};
}