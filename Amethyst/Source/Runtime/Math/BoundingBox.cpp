#include "Amethyst.h"
#include "../RHI/RHI_Vertex.h"

namespace Amethyst::Math
{
	const BoundingBox BoundingBox::Zero(Vector3::Zero, Vector3::Zero);

	BoundingBox::BoundingBox()
	{
		m_Minimum = Vector3::Infinity;
		m_Maximum = Vector3::InfinityNegative;
	}

	BoundingBox::BoundingBox(const Vector3& minimum, const Vector3& maximum)
	{
		this->m_Minimum = minimum;
		this->m_Maximum = maximum;
	}

	BoundingBox::BoundingBox(const Vector3& vertices, const uint32_t pointCount)
	{

	}

	BoundingBox::BoundingBox(const RHI_Vertex_PositionTextureNormalTangent* vertices, const uint32_t vertexCount)
	{
		m_Minimum = Vector3::Infinity;
		m_Maximum = Vector3::InfinityNegative;

		for (uint32_t i = 0; i < vertexCount; i++)
		{
			m_Maximum.m_X = Utilities::Max(m_Maximum.m_X, vertices[i].m_Position[0]);
			m_Maximum.m_Y = Utilities::Max(m_Maximum.m_Y, vertices[i].m_Position[1]);
			m_Maximum.m_Z = Utilities::Max(m_Maximum.m_Z, vertices[i].m_Position[2]);


		}
	}

	Intersection BoundingBox::IsInside(const Vector3& point) const
	{
		// If our points falls outside the confines of our bounding box...

		if (point.m_X < m_Minimum.m_X || point.m_X > m_Maximum.m_X ||
			point.m_Y < m_Minimum.m_Y || point.m_Y > m_Maximum.m_Y ||
			point.m_Z < m_Minimum.m_Z || point.m_Z > m_Maximum.m_Z)
		{
			return Intersection::Outside;
		}
		else
		{
			return Intersection::Inside;
		}
	}
}