#pragma once
#include <memory>
#include "RHI_Utilities.h"
#include "../Core/AmethystObject.h"

namespace Amethyst
{
	class RHI_RasterizerState : public AmethystObject
	{
	public:
		RHI_RasterizerState() = default;
		RHI_RasterizerState(
			const std::shared_ptr<RHI_Device>& rhi_Device,
			const RHI_Cull_Mode cullMode,
			const RHI_Fill_Mode fillMode,
			const bool isDepthClippingEnabled,
			const bool isScissorEnabled,
			const bool isAntialiasedLineEnabled,
			const float depthBias				= 0.0f,
			const float depthBiasClamp			= 0.0f,
			const float depthBiasSlopeScaled	= 0.0f,
			const float lineWidth				= 1.0f
		);

		~RHI_RasterizerState();

		RHI_Cull_Mode RetrieveCullMode()		 const { return m_CullMode; }
		RHI_Fill_Mode RetrieveFillMode()		 const { return m_FillMode; }
		bool RetrieveDepthClippingEnabled()		 const { return m_IsDepthClippingEnabled; }
		bool RetrieveScissorEnabled()			 const { return m_IsScissorEnabled; }
		bool RetrieveAntialiasedLineEnabled()	 const { return m_IsAntialiasedLineEnabled; }
		bool IsInitialized()					 const { return m_Initialized; }
		void* RetrieveResource()				 const { return m_Buffer; }
		float RetrieveLineWidth()				 const { return m_LineWidth; }
		float RetrieveDepthBias()				 const { return m_DepthBias; }
		float RetrieveDepthBiasClamp()			 const { return m_DepthBiasClamp; }
		float RetrieveDepthBiasSlopeScaled()	 const { return m_DepthBiasSlopeScaled; }

		bool operator==(const RHI_RasterizerState& rasterizerState) const
		{
			return
				m_CullMode					== rasterizerState.RetrieveCullMode() &&
				m_FillMode					== rasterizerState.RetrieveFillMode() &&
				m_IsDepthClippingEnabled    == rasterizerState.RetrieveDepthClippingEnabled() &&
				m_IsScissorEnabled			== rasterizerState.RetrieveScissorEnabled() &&
				m_IsAntialiasedLineEnabled	== rasterizerState.RetrieveAntialiasedLineEnabled() &&
				m_DepthBias					== rasterizerState.RetrieveDepthBias() &&
				m_DepthBiasSlopeScaled		== rasterizerState.RetrieveDepthBiasSlopeScaled() &&
				m_LineWidth					== rasterizerState.RetrieveLineWidth();
		}

	private:
		//Properties
		RHI_Cull_Mode m_CullMode = RHI_Cull_Mode::RHI_Cull_Undefined;
		RHI_Fill_Mode m_FillMode = RHI_Fill_Mode::RHI_Fill_Undefined;
		bool m_IsDepthClippingEnabled = false; //Enables Z value clipping in addition to the default X and Y clips.
		bool m_IsScissorEnabled = false;
		bool m_IsAntialiasedLineEnabled = false;
		float m_DepthBias = 0.0f;
		float m_DepthBiasClamp = 0.0f;
		float m_DepthBiasSlopeScaled = 0.0f;
		float m_LineWidth = 1.0f;

		bool m_Initialized = false;
		void* m_Buffer = nullptr; //Rasterizer state view.
	};
}