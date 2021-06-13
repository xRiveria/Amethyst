 #pragma once
#include <string>
#include "MathUtilities.h"

namespace Amethyst::Math
{
	class Vector4;
	using namespace Amethyst::Math;

	class Vector3
	{
	public:
		//Constructor
		Vector3()
		{
			this->m_X = 0;
			this->m_Y = 0;
			this->m_Z = 0;
		}

		//Copy Constructor
		Vector3(const Vector3& vector)
		{
			this->m_X = vector.m_X;
			this->m_Y = vector.m_Y;
			this->m_Z = vector.m_Z;
		}

		//We can have one for Vector4, stripping the last value.
		Vector3(const Vector4& vector);

		//Construct from coordinates.
		Vector3(float x, float y, float z)
		{
			this->m_X = x;
			this->m_Y = y;
			this->m_Z = z;
		}

		//Construct from an array.
		Vector3(float positions[3])
		{
			this->m_X = positions[0];
			this->m_Y = positions[1];
			this->m_Z = positions[2];
		}

		//Construct from a single value.
		Vector3(float value)
		{
			this->m_X = value;
			this->m_Y = value;
			this->m_Z = value;
		}

		//Normalize our Vector.
		void Normalize()
		{
			const float lengthSquared = LengthSquared();

			if (!Utilities::Equals(lengthSquared, 1.0f) && lengthSquared > 0.0f) //We will proceed with normalization if our length isn't already 1 and only if our length isn't 0.
			{
				const float lengthInverted = 1.0f / Utilities::SquareRoot(lengthSquared);
				
				m_X *= lengthInverted;
				m_Y *= lengthInverted;
				m_Z *= lengthInverted;
			}
		}

		//Return a Normalized Vector.
		[[nodiscard]] Vector3 Normalized() const
		{
			const float lengthSquared = LengthSquared();
			
			if (!Utilities::Equals(lengthSquared, 1.0f) && lengthSquared > 0.0f) //We will proceed with normalization if our length isn't already 1 and only if our length isn't 0.
			{
				const float lengthInverted = 1.0f / Utilities::SquareRoot(lengthSquared);
				return (*this) * lengthInverted;
			}
			else
			{
				return *this;
			}
		}

		//Returns a Normalized Vector. 
		static inline Vector3 Normalize(const Vector3& vector) { return vector.Normalized(); }

		//Returns the Dot product. Alternatively -> ||vectorA|| * ||vectorB|| * cos(theta in between). Measures how much two vectors point in the same direction.
		static inline float Dot(const Vector3& vectorA, const Vector3& vectorB) { return (vectorA.m_X * vectorB.m_X + vectorA.m_Y * vectorB.m_Y + vectorA.m_Z * vectorB.m_Z); }

		//Returns the dot product.
		[[nodiscard]] float Dot(const Vector3& vector) const { return m_X * vector.m_X + m_Y * vector.m_Y + m_Z * vector.m_Z; }

		//Returns the Cross Product. Remember that cross products only work in 3D. Produces a 3rd vector that is perpendicular to our vectors.
		static inline Vector3 Cross(const Vector3& vectorA, const Vector3& vectorB)
		{
			return Vector3(
				vectorA.m_Y * vectorB.m_Z - vectorB.m_Y * vectorA.m_Z,
				-(vectorA.m_Z * vectorB.m_X - vectorB.m_Z * vectorA.m_X),
				vectorA.m_X * vectorB.m_Y - vectorB.m_X * vectorA.m_Y
			);
		}

		//Returns the Cross Product.
		[[nodiscard]] Vector3 Cross(const Vector3& vector) const { return Cross(*this, vector); }

		//Returns the length/magnitude.
		[[nodiscard]] float Length() const { return Utilities::SquareRoot(m_X * m_X + m_Y * m_Y + m_Z * m_Z); } //Same as Magnitude.
		[[nodiscard]] float Magnitude() const { return Utilities::SquareRoot(m_X * m_X + m_Y * m_Y + m_Z * m_Z); } //Same as Length.

		//Returns the squared length/magnitude.
		[[nodiscard]] float LengthSquared() const { return m_X * m_X + m_Y * m_Y + m_Z * m_Z; } //Same as Magnitude.
		[[nodiscard]] float MagnitudeSquared() const { return m_X * m_X + m_Y * m_Y + m_Z * m_Z; } //Same as Length.

		//Returns a copy of our vector with its magnitude clamped to maxLength.
		inline void ClampMagnitude(float maxLength)
		{
			const float magnitudeSquared = MagnitudeSquared();

			if (magnitudeSquared > maxLength * maxLength) //If our magnitude squared is more than the max we are clamping to squared...
			{
				const float magnitude = Utilities::SquareRoot(magnitudeSquared);

				//These intermediate variables force the intermediate result to be of float precision. Without this, the intermediate result can be of higher precision, 
				//which changes behavior.
				const float normalizedX = m_X / magnitude; //Retrieve normalized value.
				const float normalizedY = m_Y / magnitude;
				const float normalizedZ = m_Z / magnitude;

				m_X = normalizedX * maxLength; //Multiply the value we are clamping to with our normalized vector.
				m_Y = normalizedY * maxLength;
				m_Z = normalizedZ * maxLength;
			}
		}

		//Returns the distance between 2 vectors.
		static inline float Distance(const Vector3& vectorA, const Vector3& vectorB) { return (vectorB - vectorA).Length(); }
		//Returns the squared distance between 2 vectors.
		static inline float DistanceSquared(const Vector3& vectorA, const Vector3& vectorB) { return (vectorB - vectorA).LengthSquared(); }

		//Floor
		void Floor()
		{
			m_X = Utilities::Floor(m_X);
			m_Y = Utilities::Floor(m_Y);
			m_Z = Utilities::Floor(m_Z);
		}

		//Return the absolute vector.
		[[nodiscard]] Vector3 Abs() const { return Vector3(Utilities::Absolute(m_X), Utilities::Absolute(m_Y), Utilities::Absolute(m_Z)); }

		Vector3 operator*(const Vector3& vector) const
		{
			return Vector3(
				this->m_X * vector.m_X,
				this->m_Y * vector.m_Y,
				this->m_Z * vector.m_Z
			);
		}

		void operator*=(const Vector3& vector)
		{
			this->m_X *= vector.m_X;
			this->m_Y *= vector.m_Y;
			this->m_Z *= vector.m_Z;
		}

		Vector3 operator*(const float value) const
		{
			return Vector3(
				this->m_X * value,
				this->m_Y * value,
				this->m_Z * value
			);
		}

		void operator*=(const float value)
		{
			this->m_X *= value;
			this->m_Y *= value;
			this->m_Z *= value;
		}

		Vector3 operator+(const Vector3& vector) const { return Vector3(this->m_X + vector.m_X, this->m_Y + vector.m_Y, this->m_Z + vector.m_Z); }
		Vector3 operator+(const float value) const { return Vector3(this->m_X + value, this->m_Y + value, this->m_Z + value); }

		void operator+=(const Vector3& vector)
		{
			this->m_X += vector.m_X;
			this->m_Y += vector.m_Y;
			this->m_Z += vector.m_Z;
		}

		void operator+=(const float value)
		{
			this->m_X += value;
			this->m_Y += value;
			this->m_Z += value;
		}

		Vector3 operator-(const Vector3& vector) const { return Vector3(this->m_X - vector.m_X, this->m_Y - vector.m_Y, this->m_Z - vector.m_Z); }
		Vector3 operator-(const float value) const { return Vector3(this->m_X - value, this->m_Y - value, this->m_Z - value); }

		void operator-=(const Vector3& vector)
		{
			this->m_X -= vector.m_X;
			this->m_Y -= vector.m_Y;
			this->m_Z -= vector.m_Z;
		}

		void operator-=(const float value)
		{
			this->m_X -= value;
			this->m_Y -= value;
			this->m_Z -= value;
		}

		Vector3 operator/(const Vector3& vector) const { return Vector3(m_X / vector.m_X, m_Y / vector.m_Y, m_Z / vector.m_Z); }
		Vector3 operator/(const float value) const { return Vector3(m_X / value, m_Y / value, m_Z / value); }

		void operator/=(const Vector3& vector)
		{
			this->m_X /= vector.m_X;
			this->m_Y /= vector.m_Y;
			this->m_Z /= vector.m_Z;
		}

		void operator/=(const float value)
		{
			this->m_X /= value;
			this->m_Y /= value;
			this->m_Z /= value;
		}

		//Test for equality without using epsilon.
		bool operator==(const Vector3& vector) const
		{
			return this->m_X == vector.m_X && this->m_Y == vector.m_Y && this->m_Z == vector.m_Z;
		}

		bool operator!=(const Vector3& vector) const
		{
			return !(*this == vector);
		}

		//Return negation.
		Vector3 operator-() const { return Vector3(-m_X, -m_Y, -m_Z); }

		[[nodiscard]] std::string ToString() const;
		[[nodiscard]] const float* Data() const { return &m_X; }

	public:
		float m_X;
		float m_Y;
		float m_Z;

		static const Vector3 Zero;
		static const Vector3 Left;
		static const Vector3 Right;
		static const Vector3 Up;
		static const Vector3 Down;
		static const Vector3 Forward;
		static const Vector3 Backward;
		static const Vector3 One;
		static const Vector3 Infinity;
		static const Vector3 InfinityNegative;
	};

	// Reverse Order
	inline Vector3 operator*(float leftSideFloat, const Vector3& rightSideVector) { return rightSideVector * leftSideFloat; }
}