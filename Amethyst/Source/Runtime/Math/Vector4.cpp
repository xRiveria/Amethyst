#include "Amethyst.h"
#include "Vector4.h"
#include "Vector3.h"

namespace Amethyst::Math
{
	const Vector4 Vector4::One(1.0f, 1.0f, 1.0f, 1.0f);
	const Vector4 Vector4::Zero(0.0f, 0.0f, 0.0f, 0.0f);
	const Vector4 Vector4::Infinity(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
	const Vector4 Vector4::InfinityNegative(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());

	Vector4::Vector4(const Vector3& value, float w)
	{
		this->m_X = value.m_X;
		this->m_Y = value.m_Y;
		this->m_Z = value.m_Z;
		this->m_W = w;
	}

	Vector4::Vector4(const Vector3& value)
	{
		this->m_X = value.m_X;
		this->m_Y = value.m_Y;
		this->m_Z = value.m_Z;
		this->m_W = 0.0f;
	}

	std::string Vector4::ToString() const
	{
		char buffer[256];
		sprintf_s(buffer, sizeof(buffer), "X: %f, Y: %f, Z: %f, W: %f", m_X, m_Y, m_Z, m_W);

		return std::string(buffer);
	}
};