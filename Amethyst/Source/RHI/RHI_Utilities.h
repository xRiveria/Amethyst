#pragma once
#include "GL/glew.h"
#include <string>
#include "../Runtime/Math/Vector4.h"

namespace Amethyst
{
	//Declarations
	struct RHI_Context;
	class RHI_DepthStencilState;
	class RHI_BlendState;
	class RHI_RasterizerState;
	class RHI_Device;
	class RHI_Viewport;
	class RHI_Semaphore;
	class RHI_Fence;
	class RHI_Pipeline;
	class RHI_SwapChain;
	class RHI_DescriptorSetLayout;
	class RHI_ConstantBuffer;
	class RHI_Sampler;
	class RHI_Shader;
	class RHI_InputLayout;
	class RHI_PipelineState;
	class RHI_VertexBuffer;
	class RHI_IndexBuffer;
	class RHI_PipelineCache;

	class RHI_DescriptorSetLayoutCache;
	class RHI_CommandList;
	class RHI_Texture;
	class RHI_DescriptorSet;

	//Note that the "host" is the processor in which your applciation is running. It is possible to ask Vulkan to give you a pointer to memory allocated from host-accessible regions. This is known as mapping memory.
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

	enum RHI_Blend //Think (Result = Source * Source Function +(RHI_Blend_Operation) Destination * Destination Function)
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

	enum RHI_Comparison_Function
	{
		RHI_Comparison_Never,					//Never pass the comparison.
		RHI_Comparison_Less,					//If the source data is lesser than the destination data, the comparison passes.
		RHI_Comparison_Equal,					//If the source data is equal to the destination data, the comparison passes.
		RHI_Comparison_LessEqual,				//If the source data is less than or equal to the destination data, the comparison passes.
		RHI_Comparison_Greater,					//If the source data is greater than the destination data, the comparison  oasses,
		RHI_Comparison_NotEqual,				//If the source data is not equal to the destination data, the comparison passes.
		RHI_Comparison_GreaterEqual,			//If the source data is greater than or equal to the destination data, the comparison passes.
		RHI_Comparison_Always					//Always pass the comparison.
	};

	enum RHI_Stencil_Operation
	{
		RHI_Stencil_Keep,						//Does not update the stencil buffer data.
		RHI_Stencil_Zero,						//Sets the stencil buffer data to 0.
		RHI_Stencil_Replace,					//Replaces the stencil buffer data with the new values.
		RHI_Stencil_IncrementSaturation,		//Increments the stencil buffer entry, clamping it to the maximum value.
		RHI_Stencil_DecrementSaturation,		//Decrements the stencil buffer entry, clamping it to 0.
		RHI_Stencil_Invert,						//Inverts the bits in the stencil buffer data.
		RHI_Stencil_Increment,					//Increments the stencil buffer data, wrapping to 0 if the new value exceeds the maximum value.
		RHI_Stencil_Decrement					//Decrements the stencil buffer data, wrapping to the maximum value if the new value is less than 0.
	};

	enum RHI_Cull_Mode
	{
		RHI_Cull_None,
		RHI_Cull_Front,
		RHI_Cull_Back,
		RHI_Cull_Undefined
	};

	enum RHI_Fill_Mode
	{
		RHI_Fill_Solid,
		RHI_Fill_Wireframe,
		RHI_Fill_Undefined
	};

	enum class RHI_Semaphore_State
	{
		Idle,
		Submitted,
		Signaled
	};

	enum RHI_Format : uint32_t	//Gets serialized so its better to be explicit.
	{
		//R
		RHI_Format_R8_Unorm,
		RHI_Format_R16_Uint,
		RHI_Format_R16_Float,
		RHI_Format_R32_Uint,
		RHI_Format_R32_Float,
		//RG
		RHI_Format_R8G8_Unorm,
		RHI_Format_R16G16_Float,
		RHI_Format_R32G32_Float,
		//RGB
		RHI_Format_R11G11B10_Float,
		RHI_Format_R32G32B32_Float,
		//RGBA
		RHI_Format_R8G8B8A8_Unorm,
		RHI_Format_R10G10B10A2_Unorm,
		RHI_Format_R16G16B16A16_Snorm,
		RHI_Format_R16G16B16A16_Float,
		RHI_Format_R32G32B32A32_Float,
		//Depth
		RHI_Format_D32_Float,
		RHI_Format_D32_Float_S8X24_Uint,

		RHI_Format_Undefined
	};

	enum RHI_Present_Mode : uint32_t //VSync/V-Blank is used to swap display buffers when using double or triple buffering. We can use this to avoid on screen tearing artifacts.
	{
		RHI_Present_Immediate				 = 1 << 0,	//Doesn't wait for V-Blank to update the current image. Frames are not dropped. Tearing.
		RHI_Present_Mailbox					 = 1 << 1,	//Waits for V-Blank. Frames are dropped as existing entries in the queue are replaced as they come in. No tearing.
		RHI_Present_Fifo					 = 1 << 2,	//Waits for V-Blank, always. New frames are appended to the end of the queue, and one request is removed from the beginning of the queue and processed during or after each V-Blank. No tearing.
		RHI_Present_FifoRelaxed				 = 1 << 3,	//Waits for V-Blank, once. Frames are not dropped. Tearing.
		RHI_Present_SharedDemandRefresh		 = 1 << 4,  //The presentation engine and application have concurrent access to a single image - a shared presentable image. Updates the image on request, which may be at any point, resulting in possible tearing.
		RHI_Present_SharedDemandContinuousRefresh  = 1 << 5,

		// D3D11 only flags as match to Vulkan is possible. See: https://docs.microsoft.com/en-us/windows/win32/api/dxgi/ne-dxgi-dxgi_swap_effect
		RHI_Swap_Discard					 = 1 << 6, // Use this flag to specify the bit-block transfer (bitblt) model and to specify that DXGI discards the contents of the back buffer after you call Present1. 
		RHI_Swap_Sequential					 = 1 << 7, // Use this flag to specify the bitblt model and to specify that DXGI persist the contents of the backbuffer after you call Present. Use this option to present the contents of the swpachain in order, from the first buffer (buffer 0) to the last buffer.
		RHI_Swap_Flip_Sequential			 = 1 << 8, // Use this flag to specify the flip presentation model and to specify that DXGI persist the contents of the backbuffer after you call Present1.
		RHI_Swap_Flip_Discard				 = 1 << 9, // Use this flag to specify the flip presentation model and to specify that DXGI discard the contents of the backbuffer after you call Present1.
		RHI_SwapChain_Allow_Mode_Switch		 = 1 << 10 // Enables an application to switch modes by calling IDXGISwapChain::ResizeTarget. When switching from windowed to full-screen mode, the display mode (or monitor resolution) will be changed to match the dimensions of the application window.

	};

	enum class RHI_Descriptor_Type
	{
		Sampler,
		Texture,
		ConstantBuffer,	//Constant Buffers preserve the values of stored shader constants until it becomes necessary to change them.
		Undefined
	};

	enum RHI_Filter
	{
		RHI_Filter_Nearest,
		RHI_Filter_Linear
	};

	enum RHI_Sampler_Mipmap_Mode
	{
		RHI_Sampler_Mipmap_Nearest,
		RHI_Sampler_Mipmap_Linear
	};

	enum RHI_Sampler_Address_Mode
	{
		RHI_Sampler_Address_Wrap,			//Tiles the texture at whole number boundaries. 
		RHI_Sampler_Address_Mirror,			//Flips the UV coordinates at every integer boundary.
		RHI_Sampler_Address_Clamp,			//Texture coordinates are clamped in the range [0...1].
		RHI_Sampler_Address_Border,			//Uses a specified border color when the texture coordinates fall outside the range [0...1].
		RHI_Sampler_Address_MirrorOnce		//Takes the absolute value of the texture coordinate and clamps the value to 1.
	};

	enum RHI_Shader_Type : uint8_t
	{
		RHI_Shader_Unknown = 0,
		RHI_Shader_Vertex =	 1 << 0,
		RHI_Shader_Pixel =	 1 << 1,
		RHI_Shader_Compute = 1 << 2
	};

	enum class RHI_Shader_Compilation_State
	{
		Idle,
		Compiling,
		Succeeded,
		Failed
	};

	///
	//Shader resource slot shifts (required to produce SPIRV from HLSL).
	static const uint32_t g_RHI_ShaderShiftStorageTexture = 000;
	static const uint32_t g_RHI_ShaderShiftBuffer		  = 100;
	static const uint32_t g_RHI_ShaderShiftTexture		  = 200;
	static const uint32_t g_RHI_ShaderShiftSampler		  = 300;

	//Limits
	static const Math::Vector4 g_RHI_ColorLoadValue   = Math::Vector4(std::numeric_limits<float>::infinity(), 0.0f, 0.0f, 0.0f);
	static const Math::Vector4 g_RHI_ColorIgnoreValue = Math::Vector4(-std::numeric_limits<float>::infinity(), 0.0f, 0.0f, 0.0f);

	static const float g_RHI_DepthLoadValue   = std::numeric_limits<float>::infinity();
	static const float g_RHI_DepthIgnoreValue = -std::numeric_limits<float>::infinity();

	static const uint32_t g_RHI_StencilLoadValue = (std::numeric_limits<uint32_t>::max)() - 1; ///
	static const uint32_t g_RHI_StencilIgnoreValue = (std::numeric_limits<uint32_t>::max)();

	static const uint8_t g_RHI_MaxConstantBufferCount = 8;
	static const uint8_t g_RHI_MaxRenderTargetCount = 8;
	static const uint32_t g_RHI_DynamicOffsetEmpty = (std::numeric_limits<uint32_t>::max)();

	// Descriptor Set Limits
	static const uint8_t g_RHI_Descriptor_Max_StorageTextures			= 10;
	static const uint8_t g_RHI_Descriptor_Max_Samplers					= 10;
	static const uint8_t g_RHI_Descriptor_Max_Textures					= 10;
	static const uint8_t g_RHI_Descriptor_Max_ConstantBuffers			= 10;
	static const uint8_t g_RHI_Descriptor_Max_ConstantBuffersDynamic    = 10;

	inline const char* RHI_Format_ToString(const RHI_Format format)
	{
		switch (format)
		{
			case RHI_Format_R8_Unorm:               return "RHI_Format_R8_Unorm";
			case RHI_Format_R16_Uint:               return "RHI_Format_R16_Uint";
			case RHI_Format_R16_Float:              return "RHI_Format_R16_Float";
			case RHI_Format_R32_Uint:               return "RHI_Format_R32_Uint";
			case RHI_Format_R32_Float:              return "RHI_Format_R32_Float";
			case RHI_Format_R8G8_Unorm:             return "RHI_Format_R8G8_Unorm";
			case RHI_Format_R16G16_Float:           return "RHI_Format_R16G16_Float";
			case RHI_Format_R32G32_Float:           return "RHI_Format_R32G32_Float";
			case RHI_Format_R32G32B32_Float:        return "RHI_Format_R32G32B32_Float";
			case RHI_Format_R8G8B8A8_Unorm:         return "RHI_Format_R8G8B8A8_Unorm";
			case RHI_Format_R16G16B16A16_Float:     return "RHI_Format_R16G16B16A16_Float";
			case RHI_Format_R32G32B32A32_Float:     return "RHI_Format_R32G32B32A32_Float";
			case RHI_Format_D32_Float:              return "RHI_Format_D32_Float";
			case RHI_Format_D32_Float_S8X24_Uint:   return "RHI_Format_D32_Float_S8X24_Uint";
			case RHI_Format_Undefined:              return "RHI_Format_Undefined";
		}

		return "Unknown Format";
	}

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