#pragma once
#include "../Core/AmethystObject.h"

namespace Amethyst
{
	class RHI_Viewport : public AmethystObject
	{
	public:
		RHI_Viewport(const float x = 0.0f, const float y = 0.0f, const float width = 0.0f, const float height = 0.0f, const float depthMinimum = 0.0f, const float depthMaximum = 1.0f)
		{
			this->m_X				= x;
			this->m_Y				= y;
			this->m_Width			= width;
			this->m_Height			= height;
			this->m_DepthMinimum	= depthMinimum;
			this->m_DepthMaximum	= depthMaximum;
		}

		RHI_Viewport(const RHI_Viewport& viewport)
		{
			m_X				 = viewport.m_X;
			m_Y				 = viewport.m_Y;
			m_Width			 = viewport.m_Width;
			m_Height		 = viewport.m_Height;
			m_DepthMinimum	 = viewport.m_DepthMinimum;
			m_DepthMaximum	 = viewport.m_DepthMaximum;
		}

		~RHI_Viewport() = default;

		bool operator==(const RHI_Viewport& viewport) const
		{
			return
				m_X			   == viewport.m_X				&& m_Y				== viewport.m_Y		 &&
				m_Width		   == viewport.m_Width			&& m_Height		    == viewport.m_Height &&
				m_DepthMinimum == viewport.m_DepthMinimum	&& m_DepthMaximum	== viewport.m_DepthMaximum;
		}

		bool operator!=(const RHI_Viewport& viewport) const
		{
			return !(*this == viewport);
		}

		bool IsDefined() const
		{
			return
				m_X				!= 0.0f ||
				m_Y				!= 0.0f ||
				m_Width			!= 0.0f ||
				m_Height		!= 0.0f ||
				m_DepthMinimum  != 0.0f ||
				m_DepthMaximum  != 0.0;
		}

		float RetrieveAspectRatio() const { return m_Width / m_Height; }

	public:
		float m_X				= 0.0f;
		float m_Y				= 0.0f;
		float m_Width			= 0.0f;
		float m_Height			= 0.0f;
		float m_DepthMinimum	= 0.0f;
		float m_DepthMaximum	= 0.0f;

		static const RHI_Viewport Undefined;
	};
}