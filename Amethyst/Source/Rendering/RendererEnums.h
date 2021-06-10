#pragma once

namespace Amethyst
{
	//Shader Resource View Bindings
	enum class RendererBindingsSRV
	{
		//Material
		Material_Albedo = 0,
		Material_Roughness = 1,
		Material_Metallic = 2,
		Material_Normal = 3,
		Material_Height = 4,
		Material_Occulsion = 5,
		Material_Emission = 6,
		Material_Mask = 7,

		//G-Buffer
		GBuffer_Albedo = 8,
		GBuffer_Normal = 9,
		GBuffer_Material = 10,
		GBuffer_Velocity = 11,
		GBuffer_Depth = 12,

		//Lighting
		Light_Diffuse = 13,
		Light_Diffuse_Transparent = 14,
		Light_Specular = 15,
		Light_Specular_Transparent = 16,
		Light_Volumetric = 17,

		//Light Depth/Color Maps
		Light_Directional_Depth = 18,
		Light_Directional_Color = 19,
		Light_Point_Depth = 20,
		Light_Point_Color = 21,
		Light_Spot_Depth = 22,
		Light_Spot_Color = 23,

		//Noise
		Noise_Normal = 24,
		Noise_Blue = 25,

		//Misc
		LUTIBL = 26,
		Environment = 27,
		SSAO = 28,
		SSR = 29,
		Frame = 30,
		Tex = 31,
		Tex2 = 32,
		Font_Atlas = 33,
		SSGI = 34
	};

	//Unordered Access Views Bindings
	enum class RendererBindingsUAV
	{
		R = 0,
		RG = 1,
		RGB = 2,
		RGBA = 3,
		RGB2 = 4,
		RGB3 = 5,
		Array_RGBA = 6
	};

	enum class RendererOptionValue
	{
		Anisotropy,
		ShadowResolution,
		Tonemapping,
		Gamma,
		BloomIntensity,
		SharpenStrength,
		Fog,
		TAA_AllowUpSampling
	};

	// Render Targets
	enum class RendererRenderTarget : uint8_t
	{
		Undefined,
		GBuffer_Albedo,
		GBuffer_Normal,
		GBuffer_Material,
		GBuffer_Velocity,
		GBuffer_Depth,
		BRDF_Specular_Lut,
		Light_Diffuse,
		Light_Diffuse_Transparent,
		Light_Specular,
		Light_Specular_Transparent,
		Frame_PostProcess
	};

	// Renderer/Graphics Options
	enum RendererOption : uint64_t
	{
		RenderDebug_AABB				= 1 << 0,
		RenderDebug_PickingRay			= 1 << 1,
		RenderDebug_Grid				= 1 << 2,
		RenderDebug_Transform			= 1 << 3,
		RenderDebug_SelectionOutline    = 1 << 4,
		RenderDebug_Lights				= 1 << 5,
		RenderDebug_PerformanceMetrics  = 1 << 6,
		RenderDebug_Physics				= 1 << 7,
		RenderDebug_Wireframe			= 1 << 8,

		Render_ReverseZ					= 1 << 23
	};

	// Renderable Object Types
	enum Renderer_Object_Type
	{
		Renderer_Object_Opaque,
		Renderer_Object_Transparent,
		Renderer_Object_Light,
		Renderer_Object_Camera
	};
}