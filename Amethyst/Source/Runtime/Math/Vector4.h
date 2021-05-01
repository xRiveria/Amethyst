#pragma once
#include <string>
#include "MathUtilities.h"

namespace Amethyst::Math
{
	using namespace Amethyst::Math;

	class Vector3;
	class Matrix;

	class Vector4
	{
	public:
		Vector4()
		{
			this->m_X = 0;
			this->m_Y = 0;
			this->m_Z = 0;
			this->m_W = 0;
		}

		Vector4(float x, float y, float z, float w)
		{
			this->m_X = x;
			this->m_Y = y;
			this->m_Z = z;
			this->m_W = w;
		}

		Vector4(float value)
		{
			this->m_X = value;
			this->m_Y = value;
			this->m_Z = value;
			this->m_W = value;
		}

		Vector4(const Vector3& value, float w);
		Vector4(const Vector3& value);

		~Vector4() = default;

		bool operator ==(const Vector4& vector) const
		{
			return this->m_X == vector.m_X && this->m_Y == vector.m_Y && this->m_Z == vector.m_Z && this->m_W == vector.m_W;
		}

		bool operator !=(const Vector4& vector) const
		{
			return !(*this == vector);
		}

		Vector4 operator*(const float value) const
		{
			return Vector4(
				m_X * value,
				m_Y * value,
				m_Z * value,
				m_W * value
			);
		}

		void operator*=(const float value)
		{
			m_X *= value;
			m_Y *= value;
			m_Z *= value;
			m_W *= value;
		}

		Vector4 operator/(const float value) const
		{
			return Vector4(m_X / value, m_Y / value, m_Z / value, m_W / value);
		}

		//Returns the length or magnitude.
		[[nodiscard]] float Length() const { return Utilities::SquareRoot(m_X * m_X + m_Y * m_Y + m_Z * m_Z + m_W * m_W); } //Same as Magnitude.
		[[nodiscard]] float Magnitude() const { return Utilities::SquareRoot(m_X * m_X + m_Y * m_Y + m_Z * m_Z + m_W * m_W); } //Same as Length.

		//Returns the length or magnitude squared.
		[[nodiscard]] float LengthSquared() const { return m_X * m_X + m_Y * m_Y + m_Z * m_Z + m_W * m_W; } //Same as Magnitude.
		[[nodiscard]] float MagnitudeSquared() const { return m_X * m_X + m_Y * m_Y + m_Z * m_Z + m_W * m_W; } //Same as Length.

		//Normalize
		void Normalize()
		{
			const float lengthSquared = LengthSquared();

			if (!Utilities::Equals(lengthSquared, 1.0f) && lengthSquared > 0.0f) //Proceed with normalization if our vector isn't already 1.0 and is more than 0.0.
			{
				const float lengthInverted = 1.0f / Utilities::SquareRoot(lengthSquared);

				this->m_X *= lengthInverted;
				this->m_Y *= lengthInverted;
				this->m_Z *= lengthInverted;
				this->m_W *= lengthInverted;
			}
		}

		//Return normalized vector.
		[[nodiscard]] Vector4 Normalized() const
		{
			const float lengthSquared = LengthSquared();
			
			if (!Utilities::Equals(lengthSquared, 1.0f) && lengthSquared > 0.0f)
			{
				const float lengthInverted = 1.0f / Utilities::SquareRoot(lengthSquared);
				return (*this) * lengthInverted;
			}
			else
			{
				return *this;
			}
		}

		std::string ToString() const;
		const float* Data() const { return &m_X; }

	public:
		float m_X;
		float m_Y;
		float m_Z;
		float m_W;

		static const Vector4 One;
		static const Vector4 Zero;
		static const Vector4 Infinity;
		static const Vector4 InfinityNegative;
	};
}