#include "Amethyst.h"
#include "RHI_Sampler.h"
#include "RHI_Device.h"
#include "RHI_Implementation.h"

namespace Amethyst
{
	RHI_Sampler::RHI_Sampler(
		const std::shared_ptr<RHI_Device>& rhi_Device, 
		const RHI_Filter filterMin, 
		const RHI_Filter filterMag, 
		const RHI_Sampler_Mipmap_Mode filterMipmap, 
		const RHI_Sampler_Address_Mode samplerAddressMode, 
		const RHI_Comparison_Function comparisonFunction, 
		const bool isAnisotropyEnabled, 
		const bool isComparisonEnabled)
	{
		if (!m_RHI_Device || !m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice)
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return;
		}

		m_Resource = nullptr;
		m_RHI_Device = rhi_Device;
		m_FilterMin = filterMin;
		m_FilterMag = filterMag;
		m_FilterMipmap = filterMipmap;
		m_SamplerAddressMode = samplerAddressMode;
		m_ComparisonFunction = comparisonFunction;
		m_IsAnisotropyEnabled = isAnisotropyEnabled;
		m_IsComparisonEnabled = isComparisonEnabled;

		CreateResource();
	}
}