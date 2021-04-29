#pragma once
#include <string>

namespace Amethyst::Math
{
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

	public:
		float m_X;
		float m_Y;
		float m_Z;
	};
}