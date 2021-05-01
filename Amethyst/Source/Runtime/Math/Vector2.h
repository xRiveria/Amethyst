#pragma once
#include <memory>
#include "MathUtilities.h"

namespace Amethyst::Math
{
	using namespace Amethyst::Math;

	class Vector2
	{
	public:
		Vector2()
		{
			this->m_X = 0;
			this->m_Y = 0;
		}

		Vector2(const Vector2& vector)
		{
			this->m_X = vector.m_X;
			this->m_Y = vector.m_Y;
		}

		Vector2(float x, float y)
		{
			this->m_X = x;
			this->m_Y = y;
		}

		Vector2(int x, int y)
		{
			this->m_X = static_cast<float>(x);
			this->m_Y = static_cast<float>(y);
		}

		Vector2(uint32_t x, uint32_t y)
		{
			this->m_X = static_cast<float>(x);
			this->m_Y = static_cast<float>(y);
		}

		Vector2(float value)
		{
			this->m_X = value;
			this->m_Y = value;
		}

		/*
			Kindly note the explicit difference between Vector2 + Vector2 (doesn't actually increment the prior Vector2's original value) and Vector2 += Vector2 (increments).
		*/

		//Addition.
		Vector2 operator+(const Vector2& vector) const
		{
			return Vector2(
				this->m_X + vector.m_X,
				this->m_Y + vector.m_Y
			);
		}

		void operator+=(const Vector2& vector)
		{
			this->m_X += vector.m_X;
			this->m_Y += vector.m_Y;
		}

		//Multiplication
		Vector2 operator*(const Vector2& vector) const
		{
			return Vector2(this->m_X * vector.m_X, this->m_Y * vector.m_Y);
		}

		void operator*=(const Vector2& vector)
		{
			this->m_X *= vector.m_X;
			this->m_Y *= vector.m_Y;
		}

		Vector2 operator*(const float value) const
		{
			return Vector2(
				this->m_X * value,
				this->m_Y * value
			);
		}

		void operator*=(const float value)
		{
			this->m_X *= value;
			this->m_Y *= value;
		}

		//Substraction
		Vector2 operator-(const Vector2& vector) const
		{
			return Vector2(
				this->m_X - vector.m_X,
				this->m_Y - vector.m_Y
			);
		}

		void operator-=(const Vector2& vector)
		{
			this->m_X -= vector.m_X;
			this->m_Y -= vector.m_Y;
		}

		Vector2 operator-(const float value) const
		{
			return Vector2(
				this->m_X - value,
				this->m_Y - value
			);
		}

		void operator-=(const float value)
		{
			this->m_X -= value;
			this->m_Y -= value;
		}

		//Division
		Vector2 operator/(const Vector2& vector2) const
		{
			return Vector2(
				this->m_X / vector2.m_X,
				this->m_Y / vector2.m_Y
			);
		}

		void operator/=(const Vector2& vector2)
		{
			this->m_X /= vector2.m_X;
			this->m_Y /= vector2.m_Y;
		}

		Vector2 operator/(const float value) const
		{
			return Vector2(
				this->m_X / value,
				this->m_Y / value
			);
		}

		void operator/=(const float value)
		{
			this->m_X /= value;
			this->m_Y /= value;
		}

		//Returns the length or magnitude.
		[[nodiscard]] float Length() const { return Utilities::SquareRoot(m_X * m_X + m_Y * m_Y); } //Same as Magnitude().
		[[nodiscard]] float Magnitude() const { return Utilities::SquareRoot(m_X * m_X + m_Y * m_Y); } //Same as Length().

		//Returns the squared length or magnitude.
		[[nodiscard]] float LengthSquared() const { return m_X * m_X + m_Y * m_Y; } //Same as MagnitudeSquared().
		[[nodiscard]] float MagnitudeSquared() const { return m_X * m_X + m_Y * m_Y; } //Same as LengthSquared().

		//Returns the distance between the two vectors.
		static inline float Distance(const Vector2& vectorA, const Vector2& vectorB) { return (vectorB - vectorA).Length(); }
		
		//Returns the squared distance between the two vectors.
		static inline float DistanceSquared(const Vector2& vectorA, const Vector2& vectorB) { return (vectorB - vectorA).LengthSquared(); }

		bool operator==(const Vector2& vector) const
		{
			return m_X == vector.m_X && m_Y == vector.m_Y;
		}

		bool operator!=(const Vector2& vector) const
		{
			return m_X != vector.m_X || m_Y != vector.m_Y;
		}

		[[nodiscard]] const float* Data() const { return &m_Y; }
		std::string ToString() const;

	public:
		float m_X;
		float m_Y;

		static const Vector2 Zero;
		static const Vector2 One;
	};
}
