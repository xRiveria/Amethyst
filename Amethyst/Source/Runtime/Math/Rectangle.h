#pragma once
#include <memory>
#include "../RHI/RHI_Utilities.h"
#include "Vector2.h"

namespace Amethyst
{
	class Renderer;

	namespace Math
	{
		class Rectangle
		{
		public:
			Rectangle()
			{
				m_Left = (std::numeric_limits<float>::max)();
				m_Top = (std::numeric_limits<float>::max)();
				m_Right = std::numeric_limits<float>::lowest();
				m_Bottom = std::numeric_limits<float>::lowest();
			}

			Rectangle(const float left, const float top, const float right, const float bottom)
			{
				this->m_Left = left;
				this->m_Top = top;
				this->m_Right = right;
				this->m_Bottom = bottom;
			}

			Rectangle(const Rectangle& rectangle)
			{
				m_Left = rectangle.m_Left;
				m_Top = rectangle.m_Top;
				m_Right = rectangle.m_Right;
				m_Bottom = rectangle.m_Bottom;
			}

			~Rectangle() = default;

			bool operator==(const Rectangle& otherTriangle) const
			{
				return
					m_Left == otherTriangle.m_Left &&
					m_Top == otherTriangle.m_Top &&
					m_Right == otherTriangle.m_Right &&
					m_Bottom == otherTriangle.m_Bottom;
			}

			bool operator!=(const Rectangle& otherTriangle) const
			{
				return
					m_Left != otherTriangle.m_Left ||
					m_Top != otherTriangle.m_Top ||
					m_Right != otherTriangle.m_Right ||
					m_Bottom != otherTriangle.m_Bottom;
			}

			bool IsDefined() const
			{
				return
					m_Left != 0.0f ||
					m_Top != 0.0f ||
					m_Right != 0.0f ||
					m_Bottom != 0.0f;
			}

			float Width() const { m_Right - m_Left; }
			float Height() const { m_Bottom - m_Top; }

			//Merge a point.
			void Merge(const Math::Vector2& point) ///
			{
				m_Left = Math::Utilities::Min(m_Left, point.m_X);
				m_Top = Math::Utilities::Min(m_Top, point.m_Y);
				m_Right = Math::Utilities::Max(m_Right, point.m_X);
				m_Bottom = Math::Utilities::Max(m_Bottom, point.m_Y);
			}

			bool CreateBuffers(Renderer* renderer);
			static int RetrieveIndexCount() { return 6; }
			RHI_IndexBuffer* RetrieveIndexBuffer() const { return m_IndexBuffer.get(); }
			RHI_VertexBuffer* RetrieveVertexBuffer() const { return m_VertexBuffer.get(); }

		public: //https://i.stack.imgur.com/nBazx.png
			float m_Left; //The X coordinate of the left side of the rectangle.
			float m_Top; //The Y coordinate of the top of the rectangle.
			float m_Right; //The X coordinate of the right side of the rectangle.
			float m_Bottom; //The Y coordinate of the bottom of the rectangle.

			static const Rectangle Zero;

		private:
			std::shared_ptr<RHI_VertexBuffer> m_VertexBuffer;
			std::shared_ptr<RHI_IndexBuffer> m_IndexBuffer;
		};
	}
}