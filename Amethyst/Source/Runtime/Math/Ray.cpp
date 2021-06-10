#include "Amethyst.h"
#include "Ray.h"

namespace Amethyst::Math
{
	Ray::Ray(const Vector3& start, const Vector3& end)
	{
		m_Start = start;
		m_End = end;
		const Vector3 startToEnd = (end - start);
		m_Length = startToEnd.Length();
		m_Direction = startToEnd.Normalized();
	}

	float Ray::HitDistance(const BoundingBox& box) const
	{
		// If undefined, no hit is registered (obviously). We return an infinite distance.
		if (!box.IsDefined())
		{
			return Utilities::Infinity;
		}

		// Check if our ray's origin is inside the box.
		if (box.IsInside(m_Start))
		{
			return 0.0f;
		}

		float distance = Utilities::Infinity;

		// Check for intersection in the X direction.
		if (m_Start.m_X < box.RetrieveMinimum().m_X && m_Direction.m_X > 0.0f)
		{
			const float x = (box.RetrieveMinimum().m_X - m_Start.m_X) / m_Direction.m_X;
			if (x < distance)
			{

			}
		}
	}
}