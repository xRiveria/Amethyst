#pragma once
#include "MathUtilities.h"
#include "Vector3.h"

namespace Amethyst
{
	class Mesh;
	struct RHI_Vertex_PositionTextureNormalTangent;

	namespace Math
	{
		class Matrix;

		class BoundingBox
		{
		public:
			// Construct with Zero Size.
			BoundingBox();

			// Construct from minimum and maximum vectors.
			BoundingBox(const Vector3& minimum, const Vector3& maximum);

			// Construct from Vertices.
			BoundingBox(const Vector3* vertices, const uint32_t pointCount);

			// Construct from Vertices.
			BoundingBox(const RHI_Vertex_PositionTextureNormalTangent* vertices, const uint32_t vertexCount);

			~BoundingBox() = default;

			// Assign from bounding box.
			BoundingBox& operator =(const BoundingBox& otherBoundingBox) = default;

			// Returns the center.
			Vector3 RetrieveCenter() const { return (m_Maximum + m_Minimum) * 0.5f; }

			// Returns the size.
			Vector3 RetrieveSize() const { return m_Maximum - m_Minimum; }

			// Returns extents.
			Vector3 RetrieveExtents() const { return (m_Maximum - m_Minimum) * 0.5f; }

			// Test if a point is inside.
			Intersection IsInside(const Vector3& point) const;

			// Test if a bounding box is inside.
			Intersection IsInside(const BoundingBox& box) const;

			// Returns a transformed bounding box.
			BoundingBox Transform(const Matrix& transform) const;

			// Merge with another bounding box.
			void Merge(const BoundingBox& box);

			const Vector3& RetrieveMinimum() const { return m_Minimum; }
			const Vector3& RetrieveMaximum() const { return m_Maximum; }

			void Undefine()
			{
				m_Minimum = Vector3::InfinityNegative;
				m_Maximum = Vector3::Infinity;
			}
		
			bool IsDefined() const { return m_Minimum.m_X != INFINITY; }

			static const BoundingBox Zero;

		private:
			Vector3 m_Minimum;
			Vector3 m_Maximum;
		};
	}
}