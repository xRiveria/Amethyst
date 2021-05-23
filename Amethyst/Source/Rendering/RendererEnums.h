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
}