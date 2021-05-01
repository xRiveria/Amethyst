#pragma once
#include <cmath>
#include <limits>
#include <random>

namespace Amethyst::Math
{
	namespace Amethyst::Math::Utilities
	{
		//Returns the absolute value.


		//Check for equality but allow for a small error. Epsilon is the value between 1.0 and the next possible value dictacted by T.
		template<typename T>
		constexpr bool Equals(T leftHandSide, T rightHandSide, T error = std::numeric_limits<T>::epsilon()) { return leftHandSide + error >= rightHandSide && leftHandSide - error <= rightHandSide; }

		template<typename T>
		constexpr int Sign(T x) { return (static_cast<T>(0) < x) - (x < static_cast<T>(0)); }

		template<typename T>
		constexpr T Floor(T x) { return floor(x); }

		//Returns the absolute value (distance from the origin).
		template<typename T>
		constexpr T Absolute(T value) { return value >= static_cast<T>(0) ? value : -value; } //If positive, return the original value, else, negate it into a positive number.

		template<typename T>
		constexpr T SquareRoot(T x) { return sqrt(x); }
	}
}