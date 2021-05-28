#include "Amethyst.h"
#include "../RHI_Implementation.h"
#include "../RHI_PipelineState.h"

namespace Amethyst
{
	/* VkAttachmentLoadOp
	
		Specifies how contents of an attachment are treated at the beginning of a subpass.

		- VK_ATTACHMENT_LOAD_OP_LOAD - Specifies that the contents of the image within the render area will be preserved. 
		- VK_ATTACHMENT_LOAD_OP_CLEAR - Specifies that the contents within the render area will be cleared to a uniform value, which is specified when a render pass instance begins.
		- VK_ATTACHMENT_LOAD_OP_DONT_CARE - Specifies that the previous contents within the area need not be preserved; the contents of the attachment will be undefined within the render area.
		
		See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkAttachmentLoadOp.html
	*/

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

	/* VkAttachmentStoreOp
	
		Specifies how contents of an attachment are treated at the end of a subpass.

		- VK_ATTACHMENT_STORE_OP_STORE - Specifies the contents generated during the render pass and within the render area are written to memory.
		- VK_ATTACHMENT_STORE_OP_DONT_CARE - Specifies the contents within the render area are not needed after rendering, and may be discarded. The contents of the attachment will be undefined inside the render area.
		- VK_ATTACHMENT_STORE_OP_NONE_QCOM - Specifies that the contents within the render area were not written during rendering, and may not be written to memory.

		See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/man/html/VkAttachmentStoreOp.html
	*/

	inline VkAttachmentStoreOp RetrieveStencilStoreOperation(const RHI_DepthStencilState* depthStencilState)
	{
		if (!depthStencilState)
		{
			return VK_ATTACHMENT_STORE_OP_DONT_CARE;
		}

		return depthStencilState->RetrieveStencilWritingEnabled() ? VK_ATTACHMENT_STORE_OP_STORE : VK_ATTACHMENT_STORE_OP_DONT_CARE;
	}

	/* Render Pass 
	
		Calling any OpenGL function of the form glDraw will cause vertices will to be rendered, kicking off the entire rendering pipeline in OpenGL. Thus, if you're drawing 
		5000 instances with a single glDrawElementsInstanced, that's still just one draw call.

		The term "render pass" is more nebulous. The most common meaning refers to multipass rendering techniques. In multipass techniqhes, you render the same object 
		or scene multiple times, with each rendering of the object doing a seperate computation that gets accumulated into the final value. Each rendering of the object 
		with a particular set of state is called a "pass" or a "render pass". 
		
		Note that a render pass is not necassarily a draw call. Objects could require multiple draw calls to render. While this is typically slower than making a single 
		draw call, it may be necessary for various reasons. Techniques like this are generally how forward rendering works with multiple lights. Shaders would typically 
		only be designed to handle a single light. Thus, to render multiple lights on an object, you render the object multiple times, changing the light characteristics 
		to the next light that affects the object. Each pass therefore represents a single light.

		Multipass rendering over *objects* is going out of style with the advent of powerful hardware and deferred rendering techniques. Though deferred rendering too make 
		multiple passes itself: its just usually over the *screen than individual objects*. 
	
		In Vulkan, we need to tell the API about the framebuffer attachments that will be used while rendering. This involves specifying how many color/depth buffers 
		there will be, how many samples to use for each of them and how their contents should be handled throughout the rendering operations. In addition, we also specify
		subpasses and dependencies. A render pass will consist of at least 1 subpass. All of this is wrapped in a render pass object.
	*/

	inline bool CreateVulkanRenderPass(
		RHI_Context* rhi_Context,
		RHI_DepthStencilState* depthStencilState,
		RHI_SwapChain* renderTargetSwapchain,
		std::array<RHI_Texture*, g_RHI_MaxRenderTargetCount>& renderTargetColorTextures,
		std::array<Math::Vector4, g_RHI_MaxRenderTargetCount>& colorClearValues,
		RHI_Texture* renderTargetDepthTexture,
		float depthClearValue,
		uint32_t stencilClearValue,
		void*& renderPass
	)
	{
		// Attachments
		std::vector<VkAttachmentDescription> attachmentDescriptionInfos;
		std::vector<VkAttachmentReference> attachmentReferenceInfos;

		{
			VkAttachmentLoadOp loadOperationStencil = RetrieveStencilLoadOperation(stencilClearValue);
			VkAttachmentStoreOp storeOperationStencil = RetrieveStencilStoreOperation(depthStencilState);
			// Color
			{
				// Swapchain
				if (renderTargetSwapchain)
				{
					VkImageLayout layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

					VkAttachmentDescription attachmentDescriptionInfo = {};
					attachmentDescriptionInfo.format = rhi_Context->m_SurfaceFormat;					// Format of the image view that will be used for the attachment. For swapchains, this will be the surface format.
					attachmentDescriptionInfo.samples = VK_SAMPLE_COUNT_1_BIT;							// The amount of samples of the image. Currently set to 1 sample per pixel.
					attachmentDescriptionInfo.loadOp = RetrieveColorLoadOperation(colorClearValues[0]); // How the contents of the color and depth components of the attachment are treated at the beginning of the subpass where it is first used.
					attachmentDescriptionInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;				    // How the contents of the color and depth components of the attachment are treated at the end of the subpass where it is last used.
					attachmentDescriptionInfo.stencilLoadOp = loadOperationStencil;						// How the contents of the stencil components of the attachment are treated at the beginning of the subpass where it is first used.
					attachmentDescriptionInfo.stencilStoreOp = storeOperationStencil;					// How the contents of the stencil components of the attachment are treated at the end of the last subpass where it is used.
					attachmentDescriptionInfo.initialLayout = layout;									// Layout of the attachment image subresource when a render pass instance begins.
					attachmentDescriptionInfo.finalLayout = layout;										// Layout the attachment image subresource will be transitioned to when a render pass instance ends - after subpasses as well.

					// Description
					attachmentDescriptionInfos.push_back(attachmentDescriptionInfo);
					// Reference - Specifies which layout to transition to for a given subpass when it begins. Swapchains are color buffers, and thus we will choose the layout with the best performance: VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL.
					// Note that as each attachment will have a subpass, our attachmentReferenceInfos vector will follow the size of our attachmentDescriptionInfos vector.
					// As they are to be mapped 1:1 to our attachment descriptions, we can just use the size of our vector here as the attachment index.
					attachmentReferenceInfos.push_back({ static_cast<uint32_t>(attachmentReferenceInfos.size()), VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL });
				}
				else // Texture Render Targets
				{
					for (uint32_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
					{
						RHI_Texture* texture = renderTargetColorTextures[i];
						if (!texture)
						{
							continue;
						}

						VkImageLayout layout = VulkanImageLayout[static_cast<uint8_t>(texture->RetrieveLayout())];

						VkAttachmentDescription attachmentDescriptionInfo = {};
						attachmentDescriptionInfo.format = VulkanFormat[texture->RetrieveFormat()];
						attachmentDescriptionInfo.samples = VK_SAMPLE_COUNT_1_BIT;
						attachmentDescriptionInfo.loadOp = RetrieveColorLoadOperation(colorClearValues[i]);
						attachmentDescriptionInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
						attachmentDescriptionInfo.stencilLoadOp = loadOperationStencil;
						attachmentDescriptionInfo.stencilStoreOp = storeOperationStencil;
						attachmentDescriptionInfo.initialLayout = layout;
						attachmentDescriptionInfo.finalLayout = layout;

						// Description
						attachmentDescriptionInfos.push_back(attachmentDescriptionInfo);
						// Reference
						attachmentReferenceInfos.push_back({ static_cast<uint32_t>(attachmentReferenceInfos.size()), layout });
					}
				}
			}

			// Depth
			if (renderTargetDepthTexture)
			{
				VkImageLayout layout = VulkanImageLayout[static_cast<uint8_t>(renderTargetDepthTexture->RetrieveLayout())];

				VkAttachmentDescription attachmentDescriptionInfo = {};
				attachmentDescriptionInfo.format = VulkanFormat[renderTargetDepthTexture->RetrieveFormat()];
				attachmentDescriptionInfo.samples = VK_SAMPLE_COUNT_1_BIT;
				attachmentDescriptionInfo.loadOp = RetrieveDepthLoadOperation(depthClearValue);
				attachmentDescriptionInfo.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
				attachmentDescriptionInfo.stencilLoadOp = loadOperationStencil;
				attachmentDescriptionInfo.stencilStoreOp = storeOperationStencil;
				attachmentDescriptionInfo.initialLayout = layout;
				attachmentDescriptionInfo.finalLayout = layout;

				// Description
				attachmentDescriptionInfos.push_back(attachmentDescriptionInfo);
				// Reference
				attachmentReferenceInfos.push_back({ static_cast<uint32_t>(attachmentReferenceInfos.size()), layout });
			}
		}

		// Subpass
		VkSubpassDescription subpassDescriptionInfo = {};
		subpassDescriptionInfo.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS; // Pipeline type supported for this subpass. May be Graphics/Compute/Raytracing.
		subpassDescriptionInfo.colorAttachmentCount = static_cast<uint32_t>(renderTargetDepthTexture ? attachmentReferenceInfos.size() - 1 : attachmentDescriptionInfos.size());
		subpassDescriptionInfo.pColorAttachments = attachmentReferenceInfos.data();
		subpassDescriptionInfo.pDepthStencilAttachment = renderTargetDepthTexture ? &attachmentReferenceInfos.back() : nullptr;

		// Render Pass Creation
		VkRenderPassCreateInfo renderPassCreateInfo = {};
		renderPassCreateInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassCreateInfo.attachmentCount = static_cast<uint32_t>(attachmentDescriptionInfos.size()); // Number of attachments used by the render pass.
		renderPassCreateInfo.pAttachments = attachmentDescriptionInfos.data();
		renderPassCreateInfo.subpassCount = 1; // Number of subpasses to create.
		renderPassCreateInfo.pSubpasses = &subpassDescriptionInfo;
		renderPassCreateInfo.dependencyCount = 0; // Number of memory dependencies between pairs of subpasses.
		renderPassCreateInfo.pDependencies = nullptr;

		return VulkanUtility::Error::CheckResult(vkCreateRenderPass(rhi_Context->m_LogicalDevice, &renderPassCreateInfo, nullptr, reinterpret_cast<VkRenderPass*>(&renderPass)));
	}

	// A framebuffer is a portion of RAM containing a bitmap that drives a video display. It is a memory buffer containing data representing all the pixels in a complete video frame.
	inline bool CreateVulkanFramebuffer(RHI_Context* rhi_Context, void* renderPass, const std::vector<void*>& attachments, const uint32_t width, const uint32_t height, void*& framebuffer)
	{
		AMETHYST_ASSERT(width != 0);
		AMETHYST_ASSERT(height != 0);

		VkFramebufferCreateInfo framebufferCreateInfo = {};
		framebufferCreateInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferCreateInfo.renderPass = static_cast<VkRenderPass>(renderPass); // Defines what render passes the framebuffer will be compatible with - roughly meaning they use the same number and types of attachments. See: https://www.khronos.org/registry/vulkan/specs/1.2-extensions/html/vkspec.html#renderpass-compatibility
		framebufferCreateInfo.attachmentCount = static_cast<uint32_t>(attachments.size()); // The amount of attachments to this framebuffer.
		framebufferCreateInfo.pAttachments = reinterpret_cast<const VkImageView*>(attachments.data()); // Pointer to an array of VkImageViews, each which will be used as the corresponding attachment in a render pass instance. 
		framebufferCreateInfo.width = width;
		framebufferCreateInfo.height = height;
		framebufferCreateInfo.layers = 1; // Our swapchain textures are singular textures, so the number of layers is 1.

		return VulkanUtility::Error::CheckResult(vkCreateFramebuffer(rhi_Context->m_LogicalDevice, &framebufferCreateInfo, nullptr, reinterpret_cast<VkFramebuffer*>(&framebuffer)));
	}

	bool RHI_PipelineState::CreateFramebuffer(const RHI_Device* rhi_Device)
	{
		if (IsComputePipeline())
		{
			return true; // Compute pipelines do not require framebuffers as they are usually doing operations only.
		}

		m_RHI_Device = rhi_Device;

		const uint32_t renderTargetWidth = RetrieveRenderTargetWidth();
		const uint32_t renderTargetHeight = RetrieveRenderTargetHeight();

		// Destroy Existing Frame Resources
		DestroyFramebuffer();

		// Create a Render Pass
		if (!CreateVulkanRenderPass(m_RHI_Device->RetrieveContextRHI(), m_DepthStencilState, m_RenderTargetSwapchain, m_RenderTargetColorTextures, m_ClearValuesColor, m_RenderTargetDepthTexture, m_ClearValueDepth, m_ClearValueStencil, m_RenderPass))
		{
			return false;
		}

		// Name the Render Pass
		std::string renderPassName = m_RenderTargetSwapchain ? ("Render Pass Swapchain_" + std::to_string(m_Hash)) : ("Render Pass Texture_" + std::to_string(m_Hash));
		VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkRenderPass>(m_RenderPass), renderPassName.c_str());

		// Create Framebuffer
		if (m_RenderTargetSwapchain)
		{
			// Create one framebuffer per image.
			for (uint32_t i = 0; i < m_RenderTargetSwapchain->RetrieveBufferCount(); i++)
			{
				std::vector<void*> attachments = { m_RenderTargetSwapchain->RetrieveResourceView(i) };
				if (!CreateVulkanFramebuffer(m_RHI_Device->RetrieveContextRHI(), m_RenderPass, attachments, renderTargetWidth, renderTargetHeight, m_FrameBuffers[i]))
				{
					return false;
				}

				// Name the framebuffer.
				VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkFramebuffer>(m_FrameBuffers[i]), "FrameBuffer Swapchain");
			}

			return true;
		}
		else
		{
			std::vector<void*> attachments; // Gather attachments.

			// Color
			for (uint32_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
			{
				if (RHI_Texture* texture = m_RenderTargetColorTextures[i]) // If it exists...
				{
					attachments.emplace_back(texture->RetrieveResourceView_RenderTarget(m_RenderTargetColorTextureArrayIndex)); // We will retrieve its texture view.
				}
			}

			// Depth
			if (m_RenderTargetDepthTexture)
			{
				attachments.emplace_back(m_RenderTargetDepthTexture->RetrieveResourceView_DepthStencil(m_RenderTargetDepthStencilTextureArrayIndex));
			}

			// Create a framebuffer.
			if (!CreateVulkanFramebuffer(m_RHI_Device->RetrieveContextRHI(), m_RenderPass, attachments, renderTargetWidth, renderTargetHeight, m_FrameBuffers[0])) // Last one is [0] as if this is not a swapchain, we populate the first item in the framebuffer array.
			{
				return false;
			}

			// Name the framebuffer.
			VulkanUtility::Debug::SetVulkanObjectName(static_cast<VkFramebuffer>(m_FrameBuffers[0]), "Framebuffer Texture");

			return true;
		}

		return true;
	}

	/* Swapchain
	
		Remember that Vulkan does not have the concept of a default framebuffer, hence it requires an infrastructure that will own the buffers we will render to before 
		we visualize them on the screen. This is known as a swapchain. Essentially, the swapchain is a queue of images that are waiting to be presented to the screen. Our 
		application will acquire such an image to draw to it, and then return it to the queue. Hence, the concept of a front buffer (for display) and 1 or more backbuffers (render targets) for the swapchain.
	*/

	void* RHI_PipelineState::RetrieveFramebuffer() const
	{
		// If this is a swapchain, return the appropriate buffer.
		if (m_RenderTargetSwapchain)
		{
			if (m_RenderTargetSwapchain->RetrieveImageIndex() >= g_RHI_MaxRenderTargetCount)
			{
				AMETHYST_ERROR("Invalid swapchain image index: %d.", m_RenderTargetSwapchain->RetrieveImageIndex());
				return nullptr;
			}

			return m_FrameBuffers[m_RenderTargetSwapchain->RetrieveImageIndex()];
		}

		// If this is a render texture, we simply return the first framebuffer.
		return m_FrameBuffers[0];
	}

	void RHI_PipelineState::DestroyFramebuffer()
	{
		if (!m_RHI_Device)
		{
			return;
		}

		// Wait in case the buffer is still in use by the graphics queue.
		m_RHI_Device->Queue_Wait(RHI_Queue_Type::RHI_Queue_Graphics);

		for (uint32_t i = 0; i < g_RHI_MaxRenderTargetCount; i++)
		{
			if (void* framebuffer = m_FrameBuffers[i])
			{
				vkDestroyFramebuffer(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkFramebuffer>(framebuffer), nullptr);
			}
		}

		m_FrameBuffers.fill(nullptr);

		// Destroy Render Pass
		vkDestroyRenderPass(m_RHI_Device->RetrieveContextRHI()->m_LogicalDevice, static_cast<VkRenderPass>(m_RenderPass), nullptr);
		m_RenderPass = nullptr;
	}
}