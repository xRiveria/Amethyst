#pragma once
#include "GL/glew.h"
#include <string>

namespace Amethyst
{
	//Declarations
	struct RHI_Context;
	class RHI_Device;
	class RHI_Semaphore;
	class RHI_Fence;
	class RHI_Pipeline;
	class RHI_DescriptorSetLayout;

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