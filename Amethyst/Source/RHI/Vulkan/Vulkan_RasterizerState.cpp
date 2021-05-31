#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_RasterizerState.h"
#include "../RHI_Device.h"

namespace Amethyst
{
	RHI_RasterizerState::RHI_RasterizerState
	(
		const std::shared_ptr<RHI_Device>& rhi_Device,
		const RHI_Cull_Mode cullMode,
		const RHI_Fill_Mode fillMode,
		const bool isDepthClippingEnabled,
		const bool isScissorEnabled,
		const bool isAntialiasedLineEnabled,
		const float depthBias /*= 0.0f*/,
		const float depthBiasClamp /*= 0.0f*/,
		const float depthBiasSlopeScaled /*= 0.0f*/,
		const float lineWidth /*= 1.0f*/
	)
	{
		// Save Properties.
		m_CullMode = cullMode;
		m_FillMode = fillMode;
		m_IsDepthClippingEnabled = isDepthClippingEnabled;
		m_IsScissorEnabled = isScissorEnabled;
		m_IsAntialiasedLineEnabled = isAntialiasedLineEnabled;
		m_DepthBias = depthBias;
		m_DepthBiasClamp = depthBiasClamp;
		m_DepthBiasSlopeScaled = depthBiasSlopeScaled;
		m_LineWidth = lineWidth;
	}

	RHI_RasterizerState::~RHI_RasterizerState()
	{

	}
}