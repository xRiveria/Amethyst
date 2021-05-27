#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_DescriptorSet.h"
#include "../RHI_DescriptorSetLayout.h"
#include "../RHI_DescriptorSetLayoutCache.h"

namespace Amethyst
{
	RHI_DescriptorSetLayout::~RHI_DescriptorSetLayout()
	{
		if (m_Resource)
		{
			// Wait in case its still in use by the GPU.
			m_RHI_Device->Queue_WaitAll();

			vkDestroyDescriptorSetLayout(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkDescriptorSetLayout>(m_Resource), nullptr);

			m_Resource = nullptr;
		}
	}

	void RHI_DescriptorSetLayout::CreateResource(const std::vector<RHI_Descriptor>& descriptors)
	{
		AMETHYST_ASSERT(m_Resource == nullptr); // Ensure our resource is empty.

		// Layout Bindings
		static const uint8_t descriptorsMax = 255;
		std::array<VkDescriptorSetLayoutBinding, descriptorsMax> layoutBindings;

		for (uint32_t i = 0; i < static_cast<uint32_t>(descriptors.size()); i++)
		{
			const RHI_Descriptor& descriptor = descriptors[i];

			// Stage Flags
			VkShaderStageFlags stageFlags = 0;
			stageFlags |= (descriptor.m_Stage & RHI_Shader_Type::RHI_Shader_Vertex)  ? VK_SHADER_STAGE_VERTEX_BIT : 0;
			stageFlags |= (descriptor.m_Stage & RHI_Shader_Type::RHI_Shader_Pixel)   ? VK_SHADER_STAGE_FRAGMENT_BIT : 0;
			stageFlags |= (descriptor.m_Stage & RHI_Shader_Type::RHI_Shader_Compute) ? VK_SHADER_STAGE_COMPUTE_BIT : 0;

			layoutBindings[i].binding = descriptor.m_Slot;											// Binding number of this entry and corresponds to a resource of the same binding number in the shader stages.					
			layoutBindings[i].descriptorType = VulkanUtility::ToVulkanDescriptorType(descriptor);	// Specifies which type of resource descriptors are used in this binding.
			layoutBindings[i].descriptorCount = 1;													// The number of descriptors contained in the binding.
			layoutBindings[i].stageFlags = stageFlags;												// Specifies which pipeline shader stages can access a resource for this binding. VK_SHADER_STAGE_ALL is a shorthand specifying access at any point in time.
			layoutBindings[i].pImmutableSamplers = nullptr;											// Allows for initialization of a set of immutable samplers if the descriptor is a sampler (otherwise this is ignored). These immutable samplers are permanently bound into the set layout and must not be changed.
		}

		// Creation Info for a new Descriptor Set
		VkDescriptorSetLayoutCreateInfo createInfo = {};
		{
			createInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			createInfo.flags = 0;
			createInfo.pNext = nullptr;
			createInfo.bindingCount = static_cast<uint32_t>(descriptors.size()); // The number of elements in our descriptor set.
			createInfo.pBindings = layoutBindings.data(); // A pointer to an array of VkDescriptorSetLayoutBinding structs.
		}

		// Descriptor Set Layout Creation
		if (!VulkanUtility::Error::CheckResult(vkCreateDescriptorSetLayout(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, &createInfo, nullptr, reinterpret_cast<VkDescriptorSetLayout*>(&m_Resource))))
		{
			return;
		}

		VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkDescriptorSetLayout>(m_Resource), m_Name.c_str());
	}
}