#pragma once
#include <memory>
#include "RHI_PipelineState.h"

namespace Amethyst
{
	class RHI_Pipeline : public AmethystObject
	{
	public:
		RHI_Pipeline() = default;
		RHI_Pipeline(const RHI_Device* rhi_Device, RHI_PipelineState& rhi_PipelineState, RHI_DescriptorSetLayout* rhi_DescriptorSetLayout);
		~RHI_Pipeline();

		void* RetrievePipeline() const { return m_Pipeline; }
		void* RetrievePipelineLayout() const { return m_PipelineLayout; }
		RHI_PipelineState* RetrievePipelineState() { return &m_PipelineState; }

	private:
		RHI_PipelineState m_PipelineState;

		//API
		void* m_Pipeline = nullptr;
		void* m_PipelineLayout = nullptr;

		//Dependencies
		const RHI_Device* m_RHI_Device;
	};
}