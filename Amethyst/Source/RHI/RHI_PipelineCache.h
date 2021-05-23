#pragma once
#include <memory>
#include <unordered_map>
#include "RHI_Utilities.h"
#include "../Core/AmethystObject.h"

namespace Amethyst
{
	class RHI_PipelineCache : public AmethystObject
	{
	public:
		RHI_PipelineCache(const RHI_Device* rhi_Device) { m_RHI_Device = rhi_Device; }
		RHI_Pipeline* RetrievePipeline(RHI_CommandList* commandList, RHI_PipelineState& pipelineState, RHI_DescriptorSetLayout* descriptorSetLayout);

	private:
		//Hash of pipeline state.
		std::unordered_map<uint32_t, std::shared_ptr<RHI_Pipeline>> m_PipelineCache;

		//Dependencies
		const RHI_Device* m_RHI_Device;
	};
}