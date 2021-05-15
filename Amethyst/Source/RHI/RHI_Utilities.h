#pragma once
#include "GL/glew.h"
#include <string>
#include "../Runtime/Math/Vector4.h"

namespace Amethyst
{
	//Declarations
	struct RHI_Context;
	class RHI_Device;
	class RHI_Semaphore;
	class RHI_Fence;
	class RHI_Pipeline;
	class RHI_DescriptorSetLayout;
	class RHI_CommandList;
	class RHI_Shader;
	class RHI_Texture;
	class RHI_RasterizerState;
	class RHI_BlendState;
	class RHI_DepthStencilState;
	class RHI_SwapChain;
	class RHI_Viewport;

	enum RHI_PhysicalDevice_Type
	{
		RHI_PhysicalDevice_Unknown,			//Unknown
		RHI_PhysicalDevice_Integrated,		//The device is typically one embedded in or tightly coupled with the host. It does not have its own RAM and share the system RAM with the CPU.
		RHI_PhysicalDevice_Discrete,		//The device is typically a seperate processor connected to the host via an interlink. The GPU is seperated from the CPU with its own VRAM.
		RHI_PhysicalDevice_Virtual,			//The device is typically a virtual node in a virtualization environment - a virtual instance.
		RHI_PhysicalDevice_CPU				//The device is typically running on the same processors as the host. The CPU and GPU are essentially a common entity.
	};

	enum RHI_API_Type
	{
		RHI_API_D3D12,
		RHI_API_Vulkan,
		RHI_API_OpenGL
	};

	enum RHI_Queue_Type
	{
		RHI_Queue_Graphics,
		RHI_Queue_Transfer,
		RHI_Queue_Compute,
		RHI_Queue_Undefined
	};

	enum RHI_Image_Layout
	{
		Undefined,
		General,
		Preinitialized,
		Color_Attachment_Optimal,
		Depth_Stencil_Attachment_Optimal,
		Depth_Stencil_Read_Only_Optimal,
		Shader_Read_Only_Optimal,
		Transfer_Destination_Optimal,
		Present_Source
	};

	enum RHI_PrimitiveTopology_Mode
	{
		RHI_PrimitiveTopology_TriangleList,
		RHI_PrimitiveTopology_LineList,
		RHI_PrimitiveTopology_Unknown
	};

	enum RHI_Blend //Think (Result = Source * Source Function + Destination * Destination Function)
	{
		RHI_Blend_Zero,							//Equals to (0, 0, 0, 0). No pre-blend operations.
		RHI_Blend_One,							//Equals to (1, 1, 1, 1). No pre-blend operations.

		RHI_Blend_Source_Color,					//Equals to the source's (pixel shader) color data. No pre-blend operations.
		RHI_Blend_Inverse_Source_Color,			//Equals to (1-R, 1-G, 1-B, 1-A) of the source's (pixel shader) color data.

		RHI_Blend_Source_Alpha,					//Equals to the source's (pixel shader) alpha data. No pre-blend operations.
		RHI_Blend_Inverse_Source_Alpha,			//Equals to (1-A, 1-A, 1-A, 1-A) of the source's (pixel shader) alpha data.

		RHI_Blend_Destination_Color,			//Equals to the destination's (render target) color data. No pre-blend operations.
		RHI_Blend_Inverse_Destination_Color,	//Equals to (1-R, 1-G, 1-B, 1-A) of the destination's (render target) color data.

		RHI_Blend_Destination_Alpha,			//Equals to the destination's (render target) alpha data. No pre-blend operations.
		RHI_Blend_Inverse_Destination_Alpha,	//Equals to (1-A, 1-A, 1-A, 1-A) of the destination's (render target) alpha data.

		RHI_Blend_Source_Alpha_Saturate,		//The blend factor is (f, f, f, 1); where f = std::min(As, 1 - Ad). The pre-blend operation clamps the data to 1 or less.

		RHI_Blend_Blend_Factor,					//Equals to the blend factor set with ID3D11DeviceContext::OMSetBlendState. No pre-blend operations.
		RHI_Blend_Inverse_Blend_Factor,			//Equals to the blend factor set with ID3D11DeviceContext::OMSetBlendState. The pre-blend operation inverts the blend factor, generating 1-blendFactor.

		RHI_Blend_Source1_Color,				//Equals to the second source's color vector (used in dual source blending modes).
		RHI_Blend_Inverse_Source1_Color,		//Equals to the inverse of the second source's color vector (used in dual source blending modes).

		RHI_Blend_Source1_Alpha,				//Equals to the source's (pixel shader) alpha data (used in dual source blending modes).
		RHI_Blend_Inverse_Source1_Alpha			//Equals to (1-A, 1-A, 1-A, 1-A) of the second source's (render target) alpha data (used in dual source blending modes).
	};

	enum RHI_Blend_Operation
	{
		RHI_Blend_Operation_Add,				//Adds both source 1 and source 2.
		RHI_Blend_Operation_Subtract,			//Subtracts source 1 from source 2.
		RHI_Blend_Operation_Reverse_Subtract,	//Subtracts source 2 from source 1.
		RHI_Blend_Operation_Min,				//Find the minimum of source 1 and source 2.
		RHI_Blend_Operation_Max					//Finds the maximum of source 1 and source 2.
	};

	//Limits
	static const Math::Vector4 g_RHI_ColorLoadValue = Math::Vector4(std::numeric_limits<float>::infinity(), 0.0f, 0.0f, 0.0f);
	static const float g_RHI_DepthLoadValue = std::numeric_limits<float>::infinity();
	static const uint32_t g_RHI_StencilLoadValue = (std::numeric_limits<uint32_t>::max)() - 1;

	static const uint8_t g_RHI_MaxConstantBufferCount = 8;
	static const uint8_t g_RHI_MaxRenderTargetCount = 8;

	//Temporary
	inline std::string RetrieveRHIVendor()
	{
		return std::string(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
	}

	inline std::string RetrieveRHIRenderer()
	{
		return std::string(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
	}

	inline std::string RetrieveRHIVersion()
	{
		return std::string(reinterpret_cast<const char*>(glGetString(GL_VERSION)));
	}
}