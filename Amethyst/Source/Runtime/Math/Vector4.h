#pragma once

namespace Amethyst::Math
{
	class Vector4
	{
	public:
		Vector4()
		{
			this->m_X = 0;
			this->m_Y = 0;
			this->m_Z = 0;
			this->m_W = 0;
		}

		Vector4(float x, float y, float z, float w)
		{
			this->m_X = x;
			this->m_Y = y;
			this->m_Z = z;
			this->m_W = w;
		}

		Vector4(float value)
		{
			this->m_X = value;
			this->m_Y = value;
			this->m_Z = value;
			this->m_W = value;
		}

		float m_X;
		float m_Y;
		float m_Z;
		float m_W;
	};
}