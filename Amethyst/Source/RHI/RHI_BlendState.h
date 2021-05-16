#pragma once
#include "RHI_Utilities.h"
#include "../Core/AmethystObject.h"

namespace Amethyst
{
	class RHI_BlendState : public AmethystObject
	{
	public:
		RHI_BlendState() = default;
		RHI_BlendState(
			const std::shared_ptr<RHI_Device>& rhi_Device,
			const bool isBlendingEnabled					= false,
			const RHI_Blend sourceBlendFactor				= RHI_Blend::RHI_Blend_Source_Alpha,
			const RHI_Blend destinationBlendFactor		    = RHI_Blend::RHI_Blend_Inverse_Source_Alpha,
			const RHI_Blend_Operation blendOperation		= RHI_Blend_Operation::RHI_Blend_Operation_Add,
			const RHI_Blend sourceBlendFactorAlpha			= RHI_Blend::RHI_Blend_One,
			const RHI_Blend destinationBlendFactorAlpha		= RHI_Blend::RHI_Blend_One,
			const RHI_Blend_Operation blendOperationAlpha	= RHI_Blend_Operation::RHI_Blend_Operation_Add,
			const float blendFactor							= 0.0f
		);
		~RHI_BlendState();

		bool RetrieveBlendingEnabled() const { return m_IsBlendingEnabled; }

		RHI_Blend RetrieveSourceBlendFunction()				 const { return m_SourceBlendFunction; }
		RHI_Blend RetrieveDestinationBlendFunction()		 const { return m_DestinationBlendFunction; }
		RHI_Blend_Operation RetrieveBlendOperation()		 const { return m_BlendOperation; }

		RHI_Blend RetrieveSourceBlendFunctionAlpha()	 	 const { return m_SourceBlendFunctionAlpha; }
		RHI_Blend RetrieveDestinationBlendFunctionAlpha()	 const { return m_DestinationBlendFunctionAlpha; }
		RHI_Blend_Operation RetrieveBlendOperationAlpha()	 const { return m_BlendOperationAlpha; }

		void* RetrieveResource()							 const { return m_Resource; }

		void SetBlendFactor(const float blendFactor)		 { m_BlendFactor = blendFactor; }
		float RetrieveBlendFactor()							 const { return m_BlendFactor; }

		bool operator==(const RHI_BlendState& blendState) const
		{
			return
				m_IsBlendingEnabled				 == blendState.RetrieveBlendingEnabled() &&
				m_SourceBlendFunction			 == blendState.RetrieveSourceBlendFunction() &&
				m_DestinationBlendFunction		 == blendState.RetrieveDestinationBlendFunction() &&
				m_BlendOperation				 == blendState.RetrieveBlendOperation() &&
				m_SourceBlendFunctionAlpha		 == blendState.RetrieveSourceBlendFunctionAlpha() &&
				m_DestinationBlendFunctionAlpha	 == blendState.RetrieveDestinationBlendFunctionAlpha() &&
				m_BlendOperationAlpha			 == blendState.RetrieveBlendOperationAlpha();
		}

	private:
		bool m_IsBlendingEnabled						= false;
		RHI_Blend m_SourceBlendFunction					= RHI_Blend::RHI_Blend_Source_Alpha;
		RHI_Blend m_DestinationBlendFunction			= RHI_Blend::RHI_Blend_Inverse_Source_Alpha;
		RHI_Blend_Operation m_BlendOperation			= RHI_Blend_Operation::RHI_Blend_Operation_Add;
		RHI_Blend m_SourceBlendFunctionAlpha			= RHI_Blend::RHI_Blend_One;
		RHI_Blend m_DestinationBlendFunctionAlpha	   	= RHI_Blend::RHI_Blend_One;
		RHI_Blend_Operation m_BlendOperationAlpha		= RHI_Blend_Operation::RHI_Blend_Operation_Add;
		float m_BlendFactor								= 1.0f;

		void* m_Resource = nullptr; //The resource this blend state is attached to.
		bool m_Initialized = false;
	};
}