#pragma once
#include <memory>
#include "RHI_Utilities.h"
#include "../Core/AmethystObject.h"

namespace Amethyst
{
	class RHI_DepthStencilState : public AmethystObject
	{
	public:
		RHI_DepthStencilState() = default;
		RHI_DepthStencilState(
			const std::shared_ptr<RHI_Device>& rhi_Device,
			const bool isDepthTestingEnabled						= true,
			const bool isDepthWritingEnabled						= true,
			const RHI_Comparison_Function depthComparisonFunction	= RHI_Comparison_Function::RHI_Comparison_LessEqual,
			const bool isStencilTestingEnabled						= false,
			const bool isStencilWritingEnabled						= false,
			const RHI_Comparison_Function stencilComparisonFunction = RHI_Comparison_Function::RHI_Comparison_Always,
			const RHI_Stencil_Operation stencilFailOperation		= RHI_Stencil_Operation::RHI_Stencil_Keep,
			const RHI_Stencil_Operation stencilDepthFailOperation	= RHI_Stencil_Operation::RHI_Stencil_Keep,
			const RHI_Stencil_Operation stencilPassOperation		= RHI_Stencil_Operation::RHI_Stencil_Replace
		);

		~RHI_DepthStencilState();

		bool RetrieveDepthTestingEnabled()   const { return m_IsDepthTestingEnabled; }
		bool RetrieveDepthWritingEnabled()   const { return m_IsDepthWritingEnabled; }
		bool RetrieveStencilTestingEnabled() const { return m_IsStencilTestingEnabled; }
		bool RetrieveStencilWritingEnabled() const { return m_IsStencilWritingEnabled; }
		
		RHI_Comparison_Function RetrieveDepthComparisonFunction()	const	{ return m_DepthComparisonFunction; }
		RHI_Comparison_Function RetrieveStencilComparisonFunction() const { return m_StencilComparisonFunction; }
		RHI_Stencil_Operation RetrieveStencilFailOperation()		const { return m_StencilFailOperation; }
		RHI_Stencil_Operation RetrieveStencilDepthFailOperation()   const	{ return m_StencilDepthFailOperation; }
		RHI_Stencil_Operation RetrieveStencilPassOperation()		const { return m_StencilPassOperation; }

		uint8_t RetrieveStencilReadMask()  const { return m_StencilReadMask; }
		uint8_t RetrieveStencilWriteMask() const { return m_StencilWriteMask; }
		
		void* RetrieveResource() const { return m_Buffer; }

	private:
		bool m_IsDepthTestingEnabled							= false;
		bool m_IsDepthWritingEnabled							= false;
		RHI_Comparison_Function m_DepthComparisonFunction		= RHI_Comparison_Function::RHI_Comparison_Never;
		bool m_IsStencilTestingEnabled							= false;
		bool m_IsStencilWritingEnabled							= false;
		RHI_Comparison_Function m_StencilComparisonFunction		= RHI_Comparison_Function::RHI_Comparison_Never;
		RHI_Stencil_Operation m_StencilFailOperation		    = RHI_Stencil_Operation::RHI_Stencil_Keep; //If the operation fails, we keep the contents of the stencil buffer.
		RHI_Stencil_Operation m_StencilDepthFailOperation       = RHI_Stencil_Operation::RHI_Stencil_Keep; //If the operation fails, we keep the contents of the stencil buffer.
		RHI_Stencil_Operation m_StencilPassOperation			= RHI_Stencil_Operation::RHI_Stencil_Replace; //If the operation passes, we replace the contents of the stencil buffer.
		uint8_t m_StencilReadMask							    = 1;
		uint8_t m_StencilWriteMask							    = 1;
		
		bool m_Initialized										= false;
		void* m_Buffer											= nullptr;
	};
}