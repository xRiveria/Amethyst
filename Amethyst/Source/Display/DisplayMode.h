#pragma once
#include <memory>

namespace Amethyst
{
	struct DisplayMode
	{
		DisplayMode() = default;
		DisplayMode(const uint32_t width, const uint32_t height, const uint32_t numerator, const uint32_t denominator)
		{
			this->m_Width = width;
			this->m_Height = height;
			this->m_Numerator = numerator;
			this->m_Denominator = denominator;

			// Note that the formula to calculate monitor hertz/refresh rate here would be Numerator/Denominator (Direct3D).
			this->m_Hertz = static_cast<double>(numerator) / static_cast<double>(denominator);
		}

		bool operator ==(const DisplayMode& otherDisplayMode) const
		{
			return m_Width == otherDisplayMode.m_Width && m_Height == otherDisplayMode.m_Height && m_Hertz == otherDisplayMode.m_Hertz;
		}

		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_Numerator = 0;
		uint32_t m_Denominator = 0;
		double   m_Hertz = 0; // Refresh rate.
	};
}