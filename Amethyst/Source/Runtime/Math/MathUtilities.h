#pragma once
#include <cmath>
#include <limits>
#include <random>

namespace Amethyst::Math
{
	enum Intersection
	{
		Outside,
		Inside,
		Intersects
	};

	namespace Amethyst::Math::Utilities
	{
		// See: https://medium.com/however-mathematics/the-first-step-for-mathematical-thinking-epsilon-38c32b792982
		constexpr float Episilon = std::numeric_limits<float>::epsilon(); // Returns the machine epsilon that is the difference between 1.0 and the next value representable by the floating point type float.
		constexpr float Infinity = std::numeric_limits<float>::infinity();
		constexpr float PI = 3.14159265359f;
		constexpr float DegreeToRadian = PI / 180.0f;
		constexpr float RadianToDegree = 180.0f / PI;

		constexpr float DegreesToRadians(const float degrees) { return degrees * DegreeToRadian; }
		constexpr float RadiansToDegrees(const float radians) { return radians * RadianToDegree; }

		// X = Value to Clamp, A (Min) & B (Max) = Boundaries to Clamp To. 
		// If X is less than A, clamp to the minimum if A. Else, if X is greater than the maximum of B, clamp to B. Else, set to X.
		template<typename T> 
		constexpr T Clamp(T x, T a, T b) { return x < a ? a : (x > b ? b : x); }

		inline float CotangentFloat(float value) { return cosf(value) / sinf(value); } //In radians.

		//Check for equality but allow for a small error. Epsilon is the value between 1.0 and the next possible value dictacted by T.
		template<typename T>
		constexpr bool Equals(T leftHandSide, T rightHandSide, T error = std::numeric_limits<T>::epsilon()) { return leftHandSide + error >= rightHandSide && leftHandSide - error <= rightHandSide; }

		template<typename T>
		constexpr T Max(T valueA, T valueB) { return valueA > valueB ? valueA : valueB; }

		template<typename T>
		constexpr T Min(T valueA, T valueB) { return valueA < valueB ? valueA : valueB; }

		template<typename T>
		constexpr T Max3(T valueA, T valueB, T valueC) { return Max(valueA, Max(valueB, valueC)); }

		template<typename T>
		constexpr T Min3(T valueA, T valueB, T valueC) { return Min(valueA, Min(valueB, valueC)); }

		template<typename T>
		constexpr int Sign(T x) { return (static_cast<T>(0) < x) - (x < static_cast<T>(0)); }

		//Rounds a number down to the largest integral value that is not greater than X.
		template<typename T>
		constexpr T Floor(T x) { return floor(x); } 

		//Rounds a number up to the smallest integral value that is not less than X.
		template<typename T>
		constexpr T Ceiling(T x) { return ceil(x); } 

		//Returns the absolute value (distance from the origin).
		template<typename T>
		constexpr T Absolute(T value) { return value >= static_cast<T>(0) ? value : -value; } //If positive, return the original value, else, negate it into a positive number.

		template<typename T>
		constexpr T SquareRoot(T x) { return sqrt(x); }

		constexpr uint32_t NextPowerOfTwo(uint32_t x)
		{
			if (x < 2)
			{
				return 2;
			}

			if (x % 2 == 0)
			{
				x << 1;
			}

			--x;
			x |= x >> 1;
			x |= x >> 2;
			x |= x >> 4;
			x |= x >> 8;
			x |= x >> 16;

			return x++;
		}
	}
}