#include "Amethyst.h"
#include "RHI_PipelineCache.h"
#include "RHI_Texture.h"
#include "RHI_Pipeline.h"
#include "RHI_SwapChain.h"
#include "RHI_DescriptorSetLayoutCache.h"

namespace Amethyst
{
	RHI_Pipeline* RHI_PipelineCache::RetrievePipeline(RHI_CommandList* commandList, RHI_PipelineState& pipelineState, RHI_DescriptorSetLayout* descriptorSetLayout)
	{
		//Validate it.
		if (!pipelineState.IsPipelineStateValid())
		{
			AMETHYST_ERROR("Invalid pipeline state.");
			return nullptr;
		}

		//Render target layout transitions.
		pipelineState.TransitionRenderTargetLayouts(commandList); ///

		//Compute a hash for it.
		uint32_t hash = pipelineState.ComputeHash();

		//If no pipeline exists for this state, create one.
		auto it = m_PipelineCache.find(hash);
		if (it == m_PipelineCache.end())
		{
			//Cache a new pipeline.
			it = m_PipelineCache.emplace(std::make_pair(hash, std::move(std::make_shared<RHI_Pipeline>(m_RHI_Device, pipelineState, descriptorSetLayout)))).first;
			AMETHYST_INFO("A new pipeline has been created.");
		}

		return it->second.get();
	}
}