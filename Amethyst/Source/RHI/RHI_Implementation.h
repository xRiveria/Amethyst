#pragma once
#define API_GRAPHICS_VULKAN
#define DEBUG

//Definition - OpenGL
#if defined(API_GRAPHICS_OPENGL)


#endif

//Definiion - DX3D12
#if defined(API_GRAPHICS_D3D12)


#endif



//Definition - Vulkan

#if defined(API_GRAPHICS_VULKAN)
#define VK_USE_PLATFORM_WIN32_KHR
#include <Vulkan/include/vulkan/vulkan.h>

static const VkPolygonMode VulkanPolygonMode[] =
{
	VK_POLYGON_MODE_FILL,
	VK_POLYGON_MODE_LINE,
	VK_POLYGON_MODE_MAX_ENUM
};

static const VkCullModeFlags VulkanCullMode[] =
{
	VK_CULL_MODE_NONE,
	VK_CULL_MODE_FRONT_BIT,
	VK_CULL_MODE_BACK_BIT,
	VK_CULL_MODE_FLAG_BITS_MAX_ENUM
};

static const VkPrimitiveTopology VulkanPrimitiveTopology[] =
{
	VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,
	VK_PRIMITIVE_TOPOLOGY_LINE_LIST
};

static const VkFormat VulkanFormat[] =
{
	//R
	VK_FORMAT_R8_UNORM,
	VK_FORMAT_R16_UINT,
	VK_FORMAT_R16_SFLOAT,
	VK_FORMAT_R32_UINT,
	VK_FORMAT_R32_SFLOAT,
	//RG
	VK_FORMAT_R8G8_UNORM,
	VK_FORMAT_R16G16_SFLOAT,
	VK_FORMAT_R32G32_SFLOAT,
	//RGB
	VK_FORMAT_B10G11R11_UFLOAT_PACK32,
	VK_FORMAT_R32G32B32_SFLOAT,
	//RGBA
	VK_FORMAT_R8G8B8A8_UNORM,
	VK_FORMAT_A2R10G10B10_UNORM_PACK32,
	VK_FORMAT_R16G16B16A16_SNORM,
	VK_FORMAT_R16G16B16A16_SFLOAT,
	VK_FORMAT_R32G32B32A32_SFLOAT,
	//Depth
	VK_FORMAT_D32_SFLOAT,
	VK_FORMAT_D32_SFLOAT_S8_UINT,

	VK_FORMAT_MAX_ENUM
};

static const VkSamplerAddressMode VulkanSamplerAddressMode[] =
{
	VK_SAMPLER_ADDRESS_MODE_REPEAT,
	VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT,
	VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
	VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_BORDER,
	VK_SAMPLER_ADDRESS_MODE_MIRROR_CLAMP_TO_EDGE
};

static const VkCompareOp VulkanCompareOperator[] =
{
	VK_COMPARE_OP_NEVER,
	VK_COMPARE_OP_LESS,
	VK_COMPARE_OP_EQUAL,
	VK_COMPARE_OP_LESS_OR_EQUAL,
	VK_COMPARE_OP_GREATER,
	VK_COMPARE_OP_NOT_EQUAL,
	VK_COMPARE_OP_GREATER_OR_EQUAL,
	VK_COMPARE_OP_ALWAYS
};

static const VkStencilOp VulkanStencilOperation[] =
{
	VK_STENCIL_OP_KEEP,
	VK_STENCIL_OP_ZERO,
	VK_STENCIL_OP_REPLACE,
	VK_STENCIL_OP_INCREMENT_AND_CLAMP,
	VK_STENCIL_OP_DECREMENT_AND_CLAMP,
	VK_STENCIL_OP_INVERT,
	VK_STENCIL_OP_INCREMENT_AND_WRAP,
	VK_STENCIL_OP_DECREMENT_AND_WRAP
};

static const VkBlendFactor VulkanBlendFactor[] =
{
	VK_BLEND_FACTOR_ZERO,
	VK_BLEND_FACTOR_ONE,
	VK_BLEND_FACTOR_SRC_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_SRC_COLOR,
	VK_BLEND_FACTOR_SRC_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	VK_BLEND_FACTOR_DST_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_DST_ALPHA,
	VK_BLEND_FACTOR_DST_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_DST_COLOR,
	VK_BLEND_FACTOR_SRC_ALPHA_SATURATE,
	VK_BLEND_FACTOR_CONSTANT_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_CONSTANT_COLOR,
	VK_BLEND_FACTOR_SRC1_COLOR,
	VK_BLEND_FACTOR_ONE_MINUS_SRC1_COLOR,
	VK_BLEND_FACTOR_SRC1_ALPHA,
	VK_BLEND_FACTOR_ONE_MINUS_SRC1_ALPHA
};

static const VkBlendOp VulkanBlendOperation[] =
{
	VK_BLEND_OP_ADD,
	VK_BLEND_OP_SUBTRACT,
	VK_BLEND_OP_REVERSE_SUBTRACT,
	VK_BLEND_OP_MIN,
	VK_BLEND_OP_MAX
};

static const VkFilter VulkanFilter[] =
{
	VK_FILTER_NEAREST,
	VK_FILTER_LINEAR
};

static const VkSamplerMipmapMode VulkanMipmapMode[] =
{
	VK_SAMPLER_MIPMAP_MODE_NEAREST,
	VK_SAMPLER_MIPMAP_MODE_LINEAR
};

static const VkImageLayout VulkanImageLayout[] =
{
	VK_IMAGE_LAYOUT_UNDEFINED,
	VK_IMAGE_LAYOUT_GENERAL,
	VK_IMAGE_LAYOUT_PREINITIALIZED,
	VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
	VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL,
	VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
	VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
	VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
	VK_IMAGE_LAYOUT_PRESENT_SRC_KHR
};

#endif

//RHI_Context Header Dependencies
#include "RHI_Utilities.h"
#if defined (API_GRAPHICS_VULKAN)
#include <vector>
#include <unordered_map>
#endif

namespace Amethyst
{
	struct RHI_Context
	{
		#if defined (API_GRAPHICS_OPENGL)
		#endif

		#if defined (API_GRAPHICS_D3D12)
		#endif
		
		#if defined (API_GRAPHICS_VULKAN)

		RHI_API_Type m_APIType											= RHI_API_Type::RHI_API_Vulkan;
		uint32_t m_APIVersion											= 0;
		VkInstance m_VulkanInstance										= nullptr;
		VkPhysicalDevice m_PhysicalDevice								= nullptr;
		VkDevice m_LogicalDevice										= nullptr;
		VkPhysicalDeviceProperties m_PhysicalDeviceProperties			= {};
		VkPhysicalDeviceVulkan12Features m_PhysicalDeviceFeatures1_2    = { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES };
		VkPhysicalDeviceFeatures2 m_PhysicalDeviceFeatures				= { VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_FEATURES_2, &m_PhysicalDeviceFeatures1_2 };
		VkFormat m_SurfaceFormat										= VK_FORMAT_UNDEFINED;
		VkColorSpaceKHR	m_SurfaceColorSpace								= VK_COLOR_SPACE_MAX_ENUM_KHR;
		//Allocator
		//Allocator

		//Extensions

			#ifdef DEBUG
				/*
					VK_LAYER_KHRONOS_validation
					===========================
					The main, comprehensive Khronos validation layer - this layer encompasses the entire functionality of the layers listed below, and supersedes them. As 
					the other layers are deprecated, this layer should be used for all validation going forward.

					VK_EXT_debug_utils
					==================
					Creates a debug messenger which will pass along debug messages to an application supplied callback.
					Identify specific Vulkan objects using a name or tag to improve tracking.
					Identify specific sections within a VkQueue or VkCommandBuffer using labels to aid organization and offline analysis in external tools.

					Difference between Instance Extensions & Device Extensions
					==========================================================
					- Device extensions pertain to the behavior of a particular VkDevice object which was created with that extension activated. As such, that extension 
					cannot describe the behavior of stuff that happens before the device is created.

					External memory, for example, has obvious implications for the rendering system. So it is a device extension. However, particular VkPhysicalDevice 
					objects have different properties that can be queried with regards to the external memory functionality. You need to be able to query these properties 
					before you create the device, because if the device doesn't provide the properties you need, there's no point in making the device at all. Or at least, of 
					making the device with that extension active. 

					But device extensions govern the behavior of a device. If you don't have a device yet bvecause you haven't created one, because you're trying to decide
					whether to create one at all, what do you do? Well, that behavior has to be an instance extension. It extends the part of Vulkan that deals with the set up 
					for devices, not that governs the behavior of the device itself - something that is left for device extensions.
				
					In fact, you can say that instance extensions affect how you setup devices. Device extensions actually affect the rendering.
				*/
				
				std::vector<const char*> m_DeviceExtensions = { "VK_KHR_swapchain", "VK_EXT_memory_budget", "VK_EXT_depth_clip_enable", "VK_KHR_timeline_semaphore" };
				std::vector<const char*> m_ValidationLayers = { "VK_LAYER_KHRONOS_validation" };
				std::vector<VkValidationFeatureEnableEXT> m_ValidationExtensions = { VK_VALIDATION_FEATURE_ENABLE_BEST_PRACTICES_EXT, VK_VALIDATION_FEATURE_ENABLE_SYNCHRONIZATION_VALIDATION_EXT };
				std::vector<const char*> m_InstanceExtensions = { "VK_KHR_surface", "VK_KHR_win32_surface", "VK_EXT_debug_report", "VK_EXT_debug_utils" };
			#else
				std::vector<const char*> m_DeviceExtensions = { "VK_KHR_swapchain", "VK_EXT_memory_budget", "VK_EXT_depth_clip_enable", "VK_KHR_timeline_semaphore" };
				std::vector<const char*> m_ValidationLayers = {};
				std::vector<VkValidationFeatureEnableEXT> m_ValidationExtensions = {};
				std::vector<const char*> m_InstanceExtensions = { "VK_KHR_surface", "VK_KHR_win32_surface" };
			#endif

				bool InitializeAllocator();
				void DestroyAllocator();
		#endif

		//Debugging

		#ifdef DEBUG
			bool m_IsDebuggingEnabled = true;
			bool m_Markers = true;
			bool m_IsProfilingEnabled = true;
		#else	
			bool m_IsDebuggingEnabled = false;
			bool m_Markers = false;
			bool m_IsProfilingEnabled = false;
		#endif

		//Device Limits
		static inline uint32_t m_Texture2DDimensionsMax = 16384;
		static const uint8_t m_DescriptorsMax = 255;

		//Queues
		void* m_Queue_Graphics = nullptr;
		void* m_Queue_Compute  = nullptr;
		void* m_Queue_Transfer = nullptr;
		uint32_t m_Queue_GraphicsIndex = 0;
		uint32_t m_Queue_TransferIndex = 0;
		uint32_t m_Queue_ComputeIndex  = 0;
	};
}

//Helpers
#if defined (API_GRAPHICS_D3D12)

#elif defined (API_GRAPHICS_OPENGL)

#elif defined (API_GRAPHICS_VULKAN)
	#include "Vulkan/Vulkan_Utility.h"
#endif