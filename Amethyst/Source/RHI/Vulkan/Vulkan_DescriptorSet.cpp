#include "Amethyst.h"
#include "../RHI_DescriptorSet.h"
#include "../RHI_Implementation.h"
#include "../RHI_DescriptorSetLayout.h"
#include "../RHI_DescriptorSetLayoutCache.h"

namespace Amethyst
{
	RHI_DescriptorSet::~RHI_DescriptorSet()
	{

	}

	bool RHI_DescriptorSet::Create()
	{
		// Validate Descriptor Set
		AMETHYST_ASSERT(m_Resource = nullptr);

		// Descriptor Set Layouts
		std::array<void*, 1> descriptorSetLayouts = { m_DescriptorSetLayoutCache->RetrieveCurrentDescriptorSetLayout()->RetrieveResource() };

		// Allocate Info
		VkDescriptorSetAllocateInfo allocateInfo = {};
		{
			allocateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocateInfo.descriptorPool = static_cast<VkDescriptorPool>(m_DescriptorSetLayoutCache->RetrieveDescriptorPool());
			allocateInfo.descriptorSetCount = 1;
			allocateInfo.pSetLayouts = reinterpret_cast<VkDescriptorSetLayout*>(descriptorSetLayouts.data());
		}

		// Allocates the descriptor set from the passed in descriptor pool.
		if (!VulkanUtility::Error::CheckResult(vkAllocateDescriptorSets(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, &allocateInfo, reinterpret_cast<VkDescriptorSet*>(&m_Resource))))
		{
			return false;
		}

		// Name
		VulkanUtility::Debug::SetVulkanObjectName(*reinterpret_cast<VkDescriptorSet*>(&m_Resource), m_Name.c_str());

		return true;
	}

	void RHI_DescriptorSet::Update(const std::vector<RHI_Descriptor>& descriptors) // Updates all our descriptors.
	{
		// Validate Descriptor Set
		AMETHYST_ASSERT(m_Resource != nullptr);

		std::array<VkDescriptorImageInfo, RHI_Context::m_DescriptorsMax> imageInfos; // Structures specifying descriptor image information.
		std::array<VkDescriptorBufferInfo, RHI_Context::m_DescriptorsMax> bufferInfos; // Structures specifying descriptor buffer information.
		std::array<VkWriteDescriptorSet, RHI_Context::m_DescriptorsMax> writeDescriptorSets; // Our list of descriptors to be updated.
		uint8_t i = 0;

		for (const RHI_Descriptor& descriptor : descriptors)
		{
			// Ignore null resources (this is legal, as a render pass can choose to not use one or more resources).
			if (!descriptor.m_Resource)
			{
				continue;
			}

			// Sampler
			if (descriptor.m_DescriptorType == RHI_Descriptor_Type::Sampler)
			{
				imageInfos[i].sampler = static_cast<VkSampler>(descriptor.m_Resource);
				imageInfos[i].imageView = nullptr;
				imageInfos[i].imageLayout = VK_IMAGE_LAYOUT_UNDEFINED;
			}
			// Sampled/Storage Texture
			else if (descriptor.m_DescriptorType == RHI_Descriptor_Type::Texture)
			{
				imageInfos[i].sampler = nullptr;
				imageInfos[i].imageView = static_cast<VkImageView>(descriptor.m_Resource);
				imageInfos[i].imageLayout = descriptor.m_Resource ? VulkanImageLayout[static_cast<uint8_t>(descriptor.m_Layout)] : VK_IMAGE_LAYOUT_UNDEFINED;
			}
			// Constant/Uniform Buffer
			else if (descriptor.m_DescriptorType == RHI_Descriptor_Type::ConstantBuffer)
			{
				bufferInfos[i].buffer = static_cast<VkBuffer>(descriptor.m_Resource);
				bufferInfos[i].offset = descriptor.m_Offset;
				bufferInfos[i].range = descriptor.m_Range;
			}

			//Write Descriptor Set
			writeDescriptorSets[i].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			writeDescriptorSets[i].pNext = nullptr;
			writeDescriptorSets[i].dstSet = static_cast<VkDescriptorSet>(m_Resource); // The destination descriptor set to update.
			writeDescriptorSets[i].dstBinding = descriptor.m_Slot; // The descriptor binding within the set.
			writeDescriptorSets[i].dstArrayElement = 0; // The starting element in the array. 
			writeDescriptorSets[i].descriptorCount = 1; // The number of descriptors to update.
			writeDescriptorSets[i].descriptorType = VulkanUtility::ToVulkanDescriptorType(descriptor); 
			writeDescriptorSets[i].pImageInfo = &imageInfos[i]; // A pointer to an array of VkDescriptorImageInfo structs or is ignored.
			writeDescriptorSets[i].pBufferInfo = &bufferInfos[i]; // A pointer to an array of VkDescriptorBufferInfo structs or is ignored.
			writeDescriptorSets[i].pTexelBufferView = nullptr; // A pointer to an array of VkBufferView handles.

			i++;
		}

		vkUpdateDescriptorSets(
			m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, // Device
			static_cast<uint32_t>(i),							 // descriptorWriteCount
			writeDescriptorSets.data(),							 // pDescriptorWrites
			0,													 // descriptorCopyCount
			nullptr												 // pDescriptorCopies
		);
	}
}