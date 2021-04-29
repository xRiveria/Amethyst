#pragma once
#include <cmath>
#include <limits>
#include <random>

namespace Amethyst::Math
{
	namespace Amethyst::Math::Utilities
	{
		//Check for equality but allow for a small error. Epsilon is the value between 1.0 and the next possible value dictacted by T.
		template<typename T>
		constexpr bool Equals(T leftHandSide, T rightHandSide, T error = std::numeric_limits<T>::epsilon()) { return leftHandSide + error >= rightHandSide && leftHandSide - error <= rightHandSide; }

		template<typename T>
		constexpr int Sign(T x) { return (static_cast<T>(0) < x) - (x < static_cast<T>(0)); }

		template<typename T>
		constexpr T SquareRoot(T x) { return sqrt(x); }
	}
}