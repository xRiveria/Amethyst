#include "Amethyst.h"
#include "Vector2.h"

namespace Amethyst::Math
{
	const Vector2 Vector2::Zero(0.0f, 0.0f);
	const Vector2 Vector2::One(1.0f, 1.0f);

	std::string Vector2::ToString() const
	{
		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "X: %f, Y: %f", m_X, m_Y);
		return std::string(buffer);
	}
}