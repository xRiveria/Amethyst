#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_DepthStencilState.h"
#include "../RHI_Device.h"

namespace Amethyst
{
	RHI_DepthStencilState::RHI_DepthStencilState(
		const std::shared_ptr<RHI_Device>& rhi_Device,
		const bool isDepthTestingEnabled						/*= true*/,
		const bool isDepthWritingEnabled						/*= true*/,
		const RHI_Comparison_Function depthComparisonFunction	/*= RHI_Comparison_Function::RHI_Comparison_LessEqual*/,
		const bool isStencilTestingEnabled						/*= false*/,
		const bool isStencilWritingEnabled						/*= false*/,
		const RHI_Comparison_Function stencilComparisonFunction /*= RHI_Comparison_Function::RHI_Comparison_Always*/,
		const RHI_Stencil_Operation stencilFailOperation		/*= RHI_Stencil_Operation::RHI_Stencil_Keep*/,
		const RHI_Stencil_Operation stencilDepthFailOperation   /*= RHI_Stencil_Operation::RHI_Stencil_Keep*/,
		const RHI_Stencil_Operation stencilPassOperation        /*= RHI_Stencil_Operation::RHI_Stencil_Replace*/
	)
	{
		// Save Properties
		m_IsDepthTestingEnabled = isDepthTestingEnabled;
		m_IsDepthWritingEnabled = isDepthWritingEnabled;
		m_DepthComparisonFunction = depthComparisonFunction;
		m_IsStencilTestingEnabled = isStencilTestingEnabled;
		m_IsStencilWritingEnabled = isStencilWritingEnabled;
		m_StencilComparisonFunction = stencilComparisonFunction;
		m_StencilFailOperation = stencilFailOperation;
		m_StencilDepthFailOperation = stencilDepthFailOperation;
		m_StencilPassOperation = stencilPassOperation;
	}

	RHI_DepthStencilState::~RHI_DepthStencilState() = default;
}