#include "Amethyst.h"
#include "RHI_DescriptorSetLayout.h"
#include "RHI_ConstantBuffer.h"
#include "RHI_Sampler.h"
#include "RHI_Texture.h"
#include "RHI_DescriptorSetLayoutCache.h"
#include "RHI_DescriptorSet.h"

namespace Amethyst
{
	//A descriptor set layout is used to describe the content of a list of descriptor sets. You will also need one layout binding for each descriptor set, which you use to describe each descriptor set.
	RHI_DescriptorSetLayout::RHI_DescriptorSetLayout(const RHI_Device* rhi_Device, const std::vector<RHI_Descriptor>& descriptors, const std::string& name)
	{
		m_RHI_Device = rhi_Device;
		m_Descriptors = descriptors;
		m_Name = name;
		CreateResource(m_Descriptors);
		m_DynamicOffsets.fill(g_RHI_DynamicOffsetEmpty);

		for (const RHI_Descriptor& descriptor : m_Descriptors)
		{
			Utility::HashCombine(m_Hash, descriptor.ComputeHash(false));
		}
	}

	bool RHI_DescriptorSetLayout::SetConstantBuffer(const uint32_t slot, RHI_ConstantBuffer* constantBuffer)
	{
		for (RHI_Descriptor& descriptor : m_Descriptors)
		{
			if ((descriptor.m_DescriptorType == RHI_Descriptor_Type::ConstantBuffer) && descriptor.m_Slot == slot + g_RHI_ShaderShiftBuffer)
			{
				// Determine if the descriptor set needs to be binded.
				m_NeedsToBind = descriptor.m_Resource != constantBuffer->RetrieveResource() ? true : m_NeedsToBind; // Affects vkUpdateDescriptorSets.
				m_NeedsToBind = descriptor.m_Offset != constantBuffer->RetrieveOffset() ? true : m_NeedsToBind;	// Affects vkUpdateDescriptorSets
				m_NeedsToBind = descriptor.m_Range != constantBuffer->RetrieveStride() ? true : m_NeedsToBind;	// Affects vkUpdateDescriptorSets

				// Keep track of the dynamic offsets.
				if (constantBuffer->IsDynamic())
				{
					const uint32_t dynamicOffset = constantBuffer->RetrieveOffsetDynamic();

					if (m_DynamicOffsets[slot] != dynamicOffset)
					{
						m_DynamicOffsets[slot] = dynamicOffset;
						m_NeedsToBind = true; // Affects vkCmdBindDescriptorSets.
					}
				}

				//Update
				descriptor.m_Resource = constantBuffer->RetrieveResource();
				descriptor.m_Offset = constantBuffer->RetrieveOffset();
				descriptor.m_Range = constantBuffer->RetrieveStride();

				return true;
			}
		}

		return false;
	}

	void RHI_DescriptorSetLayout::SetSampler(const uint32_t slot, RHI_Sampler* sampler)
	{
		for (RHI_Descriptor& descriptor : m_Descriptors)
		{
			if (descriptor.m_DescriptorType == RHI_Descriptor_Type::Sampler && descriptor.m_Slot == slot + g_RHI_ShaderShiftSampler) // If we found our descriptor...
			{
				// Determine if the descriptor set needs to be binded.
				m_NeedsToBind = descriptor.m_Resource != sampler->RetrieveResource() ? true : m_NeedsToBind; // Affects vkUpdateDescriptorSets.

				// Update
				descriptor.m_Resource = sampler->RetrieveResource();

				break;
			}
		}
	}

	void RHI_DescriptorSetLayout::SetTexture(const uint32_t slot, RHI_Texture* texture, const bool isStorage)
	{
		if (!texture->IsSampled())
		{
			AMETHYST_ERROR("Texture can't be used for sampling.");
			return;
		}

		if (texture->RetrieveLayout() == RHI_Image_Layout::Undefined || texture->RetrieveLayout() == RHI_Image_Layout::Preinitialized)
		{
			AMETHYST_ERROR("Texture has an invalid layout.");
			return;
		}

		for (RHI_Descriptor& descriptor : m_Descriptors) 
		{
			const uint32_t slotMatch = slot + (isStorage ? g_RHI_ShaderShiftStorageTexture : g_RHI_ShaderShiftTexture);

			if (descriptor.m_DescriptorType == RHI_Descriptor_Type::Texture && descriptor.m_Slot == slotMatch) // If we found our descriptor...
			{
				// Determine if the descriptor set needs to be binded.
				m_NeedsToBind = descriptor.m_Resource != texture->RetrieveResourceView() ? true : m_NeedsToBind; // Affects vkUpdateDescriptorSets.

				// Update
				descriptor.m_Resource = texture->RetrieveResourceView();
				descriptor.m_Layout = texture->RetrieveLayout();

				break;
			}
		}
	}

	bool RHI_DescriptorSetLayout::RetrieveDescriptorSet(RHI_DescriptorSetLayoutCache* descriptorSetLayoutCache, RHI_DescriptorSet*& descriptorSet)
	{
		// Integrate resource into the hash.
		uint32_t hash = m_Hash;
		for (const RHI_Descriptor& descriptor : m_Descriptors)
		{
			Utility::HashCombine(hash, descriptor.m_Resource);
		}

		// If we don't have a descriptor set to match that state, create one.
		const auto it = m_DescriptorSets.find(hash);
		if (it == m_DescriptorSets.end())
		{
			// Only allocate if the descriptor set cache has enough capacity.
			if (descriptorSetLayoutCache->HasEnoughCapacity())
			{
				// Create descriptor set.
				m_DescriptorSets[hash] = RHI_DescriptorSet(m_RHI_Device, descriptorSetLayoutCache, m_Descriptors);

				// Reroutes the descriptor set pointer to the new one in our data.
				descriptorSet = &m_DescriptorSets[hash];
			}
			else
			{
				return false;
			}
		}
		else // Retrieve the existing one.
		{
			if (m_NeedsToBind)
			{
				descriptorSet = &it->second;
				m_NeedsToBind = false;
			}
		}

		return true;
	}

	const std::array<uint32_t, g_RHI_MaxConstantBufferCount> RHI_DescriptorSetLayout::RetrieveDynamicOffsets() const // Retrieves all dynamic offsets.
	{
		// vkCmdBindDescriptorSets expects an array without empty values.

		std::array<uint32_t, Amethyst::g_RHI_MaxConstantBufferCount> dynamicOffsets;
		dynamicOffsets.fill(0);

		uint32_t j = 0;
		for (uint32_t i = 0; i < g_RHI_MaxConstantBufferCount; i++)
		{
			if (m_DynamicOffsets[i] != g_RHI_DynamicOffsetEmpty)
			{
				dynamicOffsets[j++] = m_DynamicOffsets[i];
			}
		}

		return dynamicOffsets;
	}

	uint32_t RHI_DescriptorSetLayout::RetrieveDynamicOffsetCount() const
	{
		uint32_t dynamicOffsetCount = 0;

		for (uint32_t i = 0; i < g_RHI_MaxConstantBufferCount; i++)
		{
			if (m_DynamicOffsets[i] != g_RHI_DynamicOffsetEmpty) // Add to use count if it isn't empty.
			{
				dynamicOffsetCount++;
			}
		}

		return dynamicOffsetCount;
	}
}