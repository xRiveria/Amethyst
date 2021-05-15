#include "RHI_PipelineState.h"

namespace Amethyst
{
	//These can mostly only be filled once the other classes have been created. We will leave it as they are for now.

	RHI_PipelineState::RHI_PipelineState()
	{
		m_FrameBuffers.fill(nullptr);
		m_ClearValuesColor.fill(g_RHI_ColorLoadValue);
	}

	RHI_PipelineState::~RHI_PipelineState()
	{
		DestroyFramebuffer();
	}

	bool RHI_PipelineState::IsPipelineStateValid()
	{
		return false;
	}

	uint32_t RHI_PipelineState::RetrieveRenderTargetWidth() const
	{
		return uint32_t();
	}

	uint32_t RHI_PipelineState::RetrieveRenderTargetHeight() const
	{
		return uint32_t();
	}

	uint32_t RHI_PipelineState::ComputeHash()
	{
		return uint32_t();
	}

	void RHI_PipelineState::ResetClearValues()
	{
	}
}