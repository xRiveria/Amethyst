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

	BoundingBox::BoundingBox(const Vector3* vertices, const uint32_t point_count)
	{
		m_Minimum = Vector3::Infinity;
		m_Maximum = Vector3::InfinityNegative;

		for (uint32_t i = 0; i < point_count; i++)
		{
			m_Maximum.m_X = Utilities::Max(m_Maximum.m_X, vertices[i].m_X);
			m_Maximum.m_Y = Utilities::Max(m_Maximum.m_Y, vertices[i].m_Y);
			m_Maximum.m_Z = Utilities::Max(m_Maximum.m_Z, vertices[i].m_Z);
						  		   
			m_Minimum.m_X = Utilities::Min(m_Minimum.m_X, vertices[i].m_X);
			m_Minimum.m_Y = Utilities::Min(m_Minimum.m_Y, vertices[i].m_Y);
			m_Minimum.m_Z = Utilities::Min(m_Minimum.m_Z, vertices[i].m_Z);
		}
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

			m_Minimum.m_X = Utilities::Min(m_Minimum.m_X, vertices[i].m_Position[0]);
			m_Minimum.m_Y = Utilities::Min(m_Minimum.m_Y, vertices[i].m_Position[1]);
			m_Minimum.m_Z = Utilities::Min(m_Minimum.m_Z, vertices[i].m_Position[2]);
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

	Intersection BoundingBox::IsInside(const BoundingBox& box) const
	{
		if (box.m_Maximum.m_X < m_Minimum.m_X || box.m_Minimum.m_X > m_Maximum.m_X ||
			box.m_Maximum.m_Y < m_Minimum.m_Y || box.m_Minimum.m_Y > m_Maximum.m_Y ||
			box.m_Maximum.m_Z < m_Minimum.m_Z || box.m_Minimum.m_Z > m_Maximum.m_Z)
		{
			return Intersection::Outside;
		}
		else if 
		(
			box.m_Minimum.m_X < m_Minimum.m_X || box.m_Maximum.m_X > m_Maximum.m_X ||
			box.m_Minimum.m_Y < m_Minimum.m_Y || box.m_Maximum.m_Y > m_Maximum.m_Y ||
			box.m_Minimum.m_Z < m_Minimum.m_Z || box.m_Maximum.m_Z > m_Maximum.m_Z)
		{
			return Intersection::Intersects;
		}
		else
		{
			return Intersection::Inside;
		}
	}

	BoundingBox BoundingBox::Transform(const Matrix& transform) const
	{
		const Vector3 center_new = transform * RetrieveCenter();
		const Vector3 extent_old = RetrieveExtents();
		const Vector3 extend_new = Vector3
		(
			Utilities::Absolute(transform.m00) * extent_old.m_X + Utilities::Absolute(transform.m10) * extent_old.m_Y + Utilities::Absolute(transform.m20) * extent_old.m_Z,
			Utilities::Absolute(transform.m01) * extent_old.m_X + Utilities::Absolute(transform.m11) * extent_old.m_Y + Utilities::Absolute(transform.m21) * extent_old.m_Z,
			Utilities::Absolute(transform.m02) * extent_old.m_X + Utilities::Absolute(transform.m12) * extent_old.m_Y + Utilities::Absolute(transform.m22) * extent_old.m_Z
		);

		return BoundingBox(center_new - extend_new, center_new + extend_new);
	}

	void BoundingBox::Merge(const BoundingBox& box)
	{
		m_Minimum.m_X = Utilities::Min(m_Minimum.m_X, box.m_Minimum.m_X);
		m_Minimum.m_Y = Utilities::Min(m_Minimum.m_Y, box.m_Minimum.m_Y);
		m_Minimum.m_Z = Utilities::Min(m_Minimum.m_Z, box.m_Minimum.m_Z);
		m_Maximum.m_X = Utilities::Max(m_Maximum.m_X, box.m_Maximum.m_X);
		m_Maximum.m_Y = Utilities::Max(m_Maximum.m_X, box.m_Maximum.m_X);
		m_Maximum.m_Z = Utilities::Max(m_Maximum.m_X, box.m_Maximum.m_X);
	}
}