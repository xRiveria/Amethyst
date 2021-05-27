#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_BlendState.h"
#include "../RHI_Device.h"

namespace Amethyst
{
	RHI_BlendState::RHI_BlendState
	(
		const std::shared_ptr<RHI_Device>& rhi_Device,
		const bool isBlendingEnabled						/*= false*/,
		const RHI_Blend sourceBlendFactor					/*= RHI_Blend::RHI_Blend_Source_Alpha*/,
		const RHI_Blend destinationBlendFactor				/*= RHI_Blend::RHI_Blend_Inverse_Source_Alpha*/,
		const RHI_Blend_Operation blendOperation			/*= RHI_Blend_Operation::RHI_Blend_Operation_Add*/,
		const RHI_Blend sourceBlendFactorAlpha				/*= RHI_Blend::RHI_Blend_One*/,
		const RHI_Blend destinationBlendFactorAlpha			/*= RHI_Blend::RHI_Blend_One*/,
		const RHI_Blend_Operation blendOperationAlpha		/*= RHI_Blend_Operation::RHI_Blend_Operation_Add*/,
		const float blendFactor								/*= 0.0f*/									
	)
	{
		// Save Parameters
		m_IsBlendingEnabled = isBlendingEnabled;
		m_SourceBlendFunction = sourceBlendFactor;
		m_DestinationBlendFunction = destinationBlendFactor;
		m_BlendOperation = blendOperation;
		m_SourceBlendFunctionAlpha = sourceBlendFactor;
		m_DestinationBlendFunctionAlpha = destinationBlendFactorAlpha;
		m_BlendFactor = blendFactor;
	}

	RHI_BlendState::~RHI_BlendState()
	{

	}
}