#include "Amethyst.h"
#include "RHI_DescriptorSetLayoutCache.h"
#include "RHI_Shader.h"
#include "RHI_Pipeline.h"
#include "RHI_DescriptorSetLayout.h"

namespace Amethyst
{
	RHI_DescriptorSetLayoutCache::RHI_DescriptorSetLayoutCache(const RHI_Device* rhi_Device)
	{
		m_RHI_Device = rhi_Device;

		//Set the descriptor set capacity to an initial value.
		SetDescriptorSetCapacity(16);
	}

	void RHI_DescriptorSetLayoutCache::SetPipelineState(RHI_PipelineState& pipelineState)
	{
		//Retrieve pipeline descriptors.
		RetrieveDescriptors(pipelineState, m_Descriptors);

		//Compute a hash for the descriptors for comparison.
		uint32_t hash = 0;
		for (const RHI_Descriptor& descriptor : m_Descriptors)
		{
			Utility::HashCombine(hash, descriptor.ComputeHash(false));
		}

		//If there is no descriptor set layout for this particular hash, create one.
		auto it = m_DescriptorSetLayouts.find(hash);
		if (it == m_DescriptorSetLayouts.end()) //Cannot find...
		{
			//Create a name for the descriptor set layout - this is very uswful for Vulkan debugging.
			std::string name = "Compute Shader: " + (pipelineState.m_ComputeShader ? pipelineState.m_ComputeShader->RetrieveName() : "Null");
			name += "Vertex Shader: " + (pipelineState.m_VertexShader ? pipelineState.m_VertexShader->RetrieveName() : "Null");
			name += "Pixel Shader: " + (pipelineState.m_PixelShader ? pipelineState.m_PixelShader->RetrieveName() : "Null");

			//Emplace a new descriptor set layout.
			it = m_DescriptorSetLayouts.emplace(std::make_pair(hash, std::make_shared<RHI_DescriptorSetLayout>(m_RHI_Device, m_Descriptors, name.c_str()))).first;
		}

		//Retrieve the descriptor set layout we will be using.
		m_DescriptorSetLayoutCurrent = it->second.get();
		m_DescriptorSetLayoutCurrent->NeedsToBind();
	}

	bool RHI_DescriptorSetLayoutCache::SetConstantBuffer(const uint32_t slot, RHI_ConstantBuffer* constantBuffer)
	{
		AMETHYST_ASSERT(m_DescriptorSetLayoutCurrent != nullptr);
		return m_DescriptorSetLayoutCurrent->SetConstantBuffer(slot, constantBuffer);
	}

	void RHI_DescriptorSetLayoutCache::SetSampler(const uint32_t slot, RHI_Sampler* sampler)
	{
		AMETHYST_ASSERT(m_DescriptorSetLayoutCurrent != nullptr);
		m_DescriptorSetLayoutCurrent->SetSampler(slot, sampler);
	}

	void RHI_DescriptorSetLayoutCache::SetTexture(const uint32_t slot, RHI_Texture* texture, const bool storage)
	{
		AMETHYST_ASSERT(m_DescriptorSetLayoutCurrent != nullptr);
		m_DescriptorSetLayoutCurrent->SetTexture(slot, texture, storage);
	}

	bool RHI_DescriptorSetLayoutCache::RetrieveDescriptorSet(RHI_DescriptorSet*& descriptorSet)
	{
		AMETHYST_ASSERT(m_DescriptorSetLayoutCurrent != nullptr);
		return m_DescriptorSetLayoutCurrent->RetrieveDescriptorSet(this, descriptorSet);
	}

	void RHI_DescriptorSetLayoutCache::GrowIfNeeded()
	{
		//If there is room for at least one more descriptor set (hence +1), we don't need to re-allocate yet.
		const uint32_t requiredCapacity = RetrieveDescriptorSetCount() + 1;

		//If we are over budget, re-allocate the descriptor pool with double its current size.
		if (requiredCapacity > m_DescriptorSetCapacity)
		{
			SetDescriptorSetCapacity(m_DescriptorSetCapacity * 2);
		}
	}

	uint32_t RHI_DescriptorSetLayoutCache::RetrieveDescriptorSetCount() const
	{
		/*
			Instead of updating dewsceripots to not reference it, the RHI_Texture2D destructor resets the descriptor set layout cache.
			As this can happen from another thread, we thus do the wait here. Ideally, ~RHI_Texture2D() is made to work right.
		*/
		while (m_AreDescriptorSetLayoutsBeingCleared)
		{
			AMETHYST_INFO("Waiting for descriptor set layouts to be cleared...");
			std::this_thread::sleep_for(std::chrono::milliseconds(16));
		}

		uint32_t descriptorSetCount = 0;
		for (const std::pair<size_t, std::shared_ptr<RHI_DescriptorSetLayout>>& descriptorSetsLayout : m_DescriptorSetLayouts)
		{
			descriptorSetCount += descriptorSetsLayout.second->RetrieveDescriptorSetCount();
		}

		return descriptorSetCount;
	}

	void RHI_DescriptorSetLayoutCache::RetrieveDescriptors(RHI_PipelineState& pipelineState, std::vector<RHI_Descriptor>& descriptors)
	{
		if (!pipelineState.IsPipelineStateValid())
		{
			AMETHYST_ERROR("Invalid pipeline state.");
			descriptors.clear();
			return;
		}

		bool descriptorsAcquired = false;

		if (pipelineState.IsComputePipeline())
		{
			//Wait for compilation.
			pipelineState.m_ComputeShader->WaitForCompilation();

			//Retrieve computer shader descriptors.
			descriptors = pipelineState.m_ComputeShader->RetrieveDescriptors();
			descriptorsAcquired = true;
		}
		else if (pipelineState.IsGraphicsPipeline())
		{
			//Wait for compilation.
			pipelineState.m_VertexShader->WaitForCompilation();

			//Retrieve vertex shader descriptors.
			descriptors = pipelineState.m_VertexShader->RetrieveDescriptors();
			descriptorsAcquired = true;

			//If there is a pixel shader, merge it's resources into our map as well.
			if (pipelineState.m_PixelShader)
			{
				//Wait for compilation.
				pipelineState.m_PixelShader->WaitForCompilation();

				for (const RHI_Descriptor& descriptorReflected : pipelineState.m_PixelShader->RetrieveDescriptors())
				{
					//Assume that the descriptor has been created in the vertex shader and only try to update its shader stage.
					bool updatedExisting = false;
					for (RHI_Descriptor& descriptor : descriptors)
					{
						//If the descriptor is the same...
						if ((descriptor.m_DescriptorType == descriptorReflected.m_DescriptorType) && (descriptor.m_Slot == descriptorReflected.m_Slot))
						{
							descriptor.m_Stage |= descriptorReflected.m_Stage;
							updatedExisting = true;
							break;
						}
					}

					//If no updating took place, this descriptor is new, so we add it.
					if (!updatedExisting)
					{
						descriptors.emplace_back(descriptorReflected);
					}
				}
			}
		}

		//Change constant buffers to dynamic if requested.
		if (descriptorsAcquired)
		{
			for (uint32_t i = 0; i < g_RHI_MaxConstantBufferCount; i++)
			{
				for (RHI_Descriptor& descriptor : descriptors)
				{
					if (descriptor.m_DescriptorType == RHI_Descriptor_Type::ConstantBuffer)
					{
						if (descriptor.m_Slot == pipelineState.m_DynamicConstantBufferSlots[i] + g_RHI_ShaderShiftBuffer)
						{
							descriptor.m_IsDynamicConstantBuffer = true;
						}
					}
				}
			}
		}
	}


}