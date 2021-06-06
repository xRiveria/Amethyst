#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_Device.h"
#include "../RHI_Shader.h"
#include "../RHI_InputLayout.h"
#include "spriv_cross/spirv_hlsl.hpp"
#include <atlbase.h>
#include <dxcapi.h>

namespace Amethyst
{
	RHI_Shader::~RHI_Shader()
	{
		const RHI_Context* rhi_Context = m_RHI_Device->RetrieveContextRHI();

		if (HasResource())
		{
			// Wait in case its in use by the GPU.
			m_RHI_Device->Queue_WaitAll();

			vkDestroyShaderModule(rhi_Context->m_LogicalDevice, static_cast<VkShaderModule>(m_Resource), nullptr);
			m_Resource = nullptr;
		}
	}
}