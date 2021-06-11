#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_Sampler.h"
#include "../RHI_Device.h"
#include "../../Rendering/Renderer.h"

namespace Amethyst
{	
	/* Sampler
	
		It is possible for shaders to read texels directly from images, but that is not very common when they are used as textures. Textures are usually accessed 
		through samplers, which will apply filtering and transformations to compute the final color that is retrieved.
	*/

	void RHI_Sampler::CreateResource()
	{
		VkSamplerCreateInfo samplerInfo = {};
		samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
		samplerInfo.magFilter = VulkanFilter[m_MagnificationFilter]; 
		samplerInfo.minFilter = VulkanFilter[m_MinificationFilter];
		samplerInfo.mipmapMode = VulkanMipmapMode[m_FilterMipmap];
		samplerInfo.addressModeU = VulkanSamplerAddressMode[m_SamplerAddressMode]; 
		samplerInfo.addressModeV = VulkanSamplerAddressMode[m_SamplerAddressMode];
		samplerInfo.addressModeW = VulkanSamplerAddressMode[m_SamplerAddressMode];
		samplerInfo.anisotropyEnable = m_IsAnisotropyEnabled;
		samplerInfo.maxAnisotropy = m_RHI_Device->RetrieveContextEngine()->RetrieveSubsystem<Renderer>()->RetrieveRendererOptionValue<float>(RendererOptionValue::Anisotropy);
		samplerInfo.compareEnable = m_IsComparisonEnabled ? VK_TRUE : VK_FALSE;
		samplerInfo.compareOp = VulkanCompareOperator[m_ComparisonFunction];
		samplerInfo.borderColor = VK_BORDER_COLOR_FLOAT_OPAQUE_WHITE;
		samplerInfo.mipLodBias = 0.0f; // Bias to be added to the mipmap LOD calculation and bias provided by image sampling functions in SPIR-V.
		samplerInfo.minLod = 0.0f; // Clamps the minimum of the computed LOD value.
		samplerInfo.maxLod = FLT_MAX; // Clamps the maximum of the computed LOD value. To avoid clamping the maximum value, set maxLod to the constant VK_LOD_CLAMP_NONE.

		if (vkCreateSampler(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, &samplerInfo, nullptr, reinterpret_cast<VkSampler*>(&m_Resource)) != VK_SUCCESS)
		{
			AMETHYST_ERROR("Failed to create sampler.");
		}
	}

	RHI_Sampler::~RHI_Sampler()
	{
		// Wait in case its still in use by the GPU.
		m_RHI_Device->Queue_WaitAll();

		vkDestroySampler(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, reinterpret_cast<VkSampler>(m_Resource), nullptr);
	}
}