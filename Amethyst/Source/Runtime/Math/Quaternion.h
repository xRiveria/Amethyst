#pragma once
#include "Vector3.h"

namespace Amethyst::Math
{
	class Quaternion
	{
	public:
		//Constructs an Identity Quaternion.
		Quaternion()
		{
			m_X = 0.0f;
			m_Y = 0.0f;
			m_Z = 0.0f;
			m_W = 0.0f;
		}
		
		//Constructs a new quarternion with the given components.
		Quaternion(float x, float y, float z, float w)
		{
			this->m_X = x;
			this->m_Y = y;
			this->m_Z = z;
			this->m_W = w;
		}

		~Quaternion() = default;



		float m_X;
		float m_Y;
		float m_Z;
		float m_W;

		static const Quaternion Identity;
	};
}