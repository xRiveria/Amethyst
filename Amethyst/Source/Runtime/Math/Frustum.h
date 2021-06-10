#pragma once
#include "Plane.h"
#include "Matrix.h"
#include "Vector3.h"

namespace Amethyst::Math
{
	class Frustum
	{
	public:
		Frustum() = default;
		Frustum(const Matrix& viewMatrix, const Matrix& projectionMatrix, float screenDepth);
		~Frustum() = default;

		bool IsVisible(const Vector3& center, const Vector3& extent, bool ignoreNearPlane = false) const;

	private:
		Intersection CheckCube(const Vector3& center, const Vector3& extent) const;
		/// Check Sphere.

	private:
		Plane m_Planes[6];
	};
}