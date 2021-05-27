#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_PipelineState.h"

namespace Amethyst
{
	inline VkAttachmentLoadOp RetrieveColorLoadOperation(const Math::Vector4& color)
	{
		if (color == g_RHI_ColorIgnoreValue)
		{
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}

		if (color == g_RHI_ColorLoadValue)
		{
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		}

		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	}

	inline VkAttachmentLoadOp RetrieveDepthLoadOperation(const float depth)
	{
		if (depth == g_RHI_DepthIgnoreValue)
		{
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}

		if (depth == g_RHI_DepthLoadValue)
		{
			return VK_ATTACHMENT_LOAD_OP_LOAD;
		}

		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	}

	inline VkAttachmentLoadOp RetrieveStencilLoadOperation(const uint32_t stencil)
	{
		if (stencil == g_RHI_StencilIgnoreValue)
		{
			return VK_ATTACHMENT_LOAD_OP_DONT_CARE;
		}

		if (stencil == g_RHI_StencilLoadValue)
		{
			return VK_ATTACHMENT_LOAD_OP_CLEAR;
		}

		return VK_ATTACHMENT_LOAD_OP_CLEAR;
	}

	inline VkAttachmentStoreOp RetrieveStencilStoreOperation(const RHI_DepthStencilState* depthStencilState)
	{
		if (!depthStencilState)
		{
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		return depthStencilState->RetrieveStencilWritingEnabled() ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}

	bool RHI_PipelineState::CreateFramebuffer(const RHI_Device* rhi_Device)
	{
		return false;
	}

	void* RHI_PipelineState::RetrieveFramebuffer() const
	{
		return nullptr;
	}
}