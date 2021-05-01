#include "Vector3.h"
#include "Vector4.h"
#include <string>

namespace Amethyst::Math
{
	const Vector3 Vector3::Zero(0.0f, 0.0f, 0.0f);
	const Vector3 Vector3::One(1.0f, 1.0f, 1.0f);
	const Vector3 Vector3::Left(-1.0f, 0.0f, 0.0f);
	const Vector3 Vector3::Right(1.0f, 0.0f, 0.0f);
	const Vector3 Vector3::Up(0.0f, 1.0f, 0.0f);
	const Vector3 Vector3::Down(0.0f, -1.0f, 0.0f);
	const Vector3 Vector3::Forward(0.0f, 0.0f, 1.0f);
	const Vector3 Vector3::Backward(0.0f, 0.0f, -1.0f);
	const Vector3 Vector3::Infinity(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
	const Vector3 Vector3::InfinityNegative(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

	Vector3::Vector3(const Vector4& vector) //Truncates the last Vector4 value.
	{
		this->m_X = vector.m_X;
		this->m_Y = vector.m_Y;
		this->m_Z = vector.m_Z;
	}

	std::string Vector3::ToString() const
	{
		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "X: %f, Y: %f, Z: %f", m_X, m_Y, m_Z);
		return std::string(buffer);
	}
}