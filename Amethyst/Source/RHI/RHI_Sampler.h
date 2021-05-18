#pragma once
#include <memory>
#include "../Core/AmethystObject.h"
#include "RHI_Utilities.h"

namespace Amethyst
{
	//Used as either a texture magnification or minification filter. The texel with coordinates nearest to the desired pixel value is used. Thus, we use nearest-point filtering.
	//The rasterizer uses the color from the texel of the nearest mipmap texture.
	#define SAMPLER_POINT		RHI_Filter_Nearest, RHI_Filter_Nearest, RHI_Sampler_Mipmap_Nearest
	//Used as a texture magnification or minification filter. A weighted average of a 2x2 area of texels surrounding the desired pixel is used.
	#define SAMPLER_BILINEAR	RHI_Filter_Nearest, RHI_Filter_Linear,  RHI_Sampler_Mipmap_Nearest
	//Used as a texture magnification or minification filter. The rasterizer linearly interpolates pixel color, using the texels of the two nearest mipmap textures.
	#define SAMPLER_TRILINEAR	RHI_Filter_Linear,  RHI_Filter_Linear,  RHI_Sampler_Mipmap_Linear

	class RHI_Sampler : public AmethystObject
	{
	public:
		RHI_Sampler(
			const std::shared_ptr<RHI_Device>& rhi_Device,
			const RHI_Filter filterMin							= RHI_Filter::RHI_Filter_Nearest,
			const RHI_Filter filterMag							= RHI_Filter::RHI_Filter_Nearest,
			const RHI_Sampler_Mipmap_Mode filterMipmap			= RHI_Sampler_Mipmap_Mode::RHI_Sampler_Mipmap_Nearest,
			const RHI_Sampler_Address_Mode samplerAddressMode   = RHI_Sampler_Address_Mode::RHI_Sampler_Address_Wrap,
			const RHI_Comparison_Function comparisonFunction	= RHI_Comparison_Function::RHI_Comparison_Always,
			const bool isAnisotropyEnabled						= false,
			const bool isComparisonEnabled						= false
		);

		~RHI_Sampler();

		RHI_Filter RetrieveFilterMin()							const { return m_FilterMin; }
		RHI_Filter RetrieveFilterMag()							const { return m_FilterMag; }
		RHI_Sampler_Mipmap_Mode RetrieveFilterMipmap()			const { return m_FilterMipmap; }
		RHI_Sampler_Address_Mode RetrieveAddressMode()			const { return m_SamplerAddressMode; }
		RHI_Comparison_Function RetrieveComparisonFunction()	const { return m_ComparisonFunction; }
		bool RetrieveAnistropyEnabled()							const { return m_IsAnisotropyEnabled; }
		bool RetrieveComparisonEnabled()						const { return m_ComparisonFunction; }
		void* RetrieveResource()								const { return m_Resource; }

	private:
		void CreateResource();

	private:
		RHI_Filter m_FilterMin = RHI_Filter::RHI_Filter_Nearest; //Scaled upwards.
		RHI_Filter m_FilterMag = RHI_Filter::RHI_Filter_Nearest; //Scaled downwards.
		RHI_Sampler_Mipmap_Mode m_FilterMipmap = RHI_Sampler_Mipmap_Mode::RHI_Sampler_Mipmap_Nearest;
		RHI_Sampler_Address_Mode m_SamplerAddressMode = RHI_Sampler_Address_Mode::RHI_Sampler_Address_Wrap;
		RHI_Comparison_Function m_ComparisonFunction = RHI_Comparison_Function::RHI_Comparison_Always;

		//Increases the image quality of textures on surfaces that are at oblique viewing angles with respect to the camera. Eliminates aliasing and improves on bilinear / trilinear filtering by reducing blur and preserving detail at extreme angles.
		bool m_IsAnisotropyEnabled = false; 
		bool m_IsComparisonEnabled = false;

		//API
		void* m_Resource = nullptr;

		//Dependencies
		std::shared_ptr<RHI_Device> m_RHI_Device;
	};
}