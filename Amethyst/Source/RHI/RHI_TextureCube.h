#pragma once
#include "RHI_Texture.h"

namespace Amethyst
{
	class RHI_TextureCube : public RHI_Texture
	{
	public:
		RHI_TextureCube(Context* context) : RHI_Texture(context)
		{
			m_ResourceType = ResourceType::TextureCube;
		}

		// Creates a cubemap with initial data - 6 textures containing (possibly) mip-levels.
		RHI_TextureCube(Context* context, const uint32_t width, const uint32_t height, const RHI_Format format, const std::vector<std::vector<std::vector<std::byte>>>& data) : RHI_Texture(context)
		{
			m_ResourceType = ResourceType::TextureCube;
			m_Width = width;
			m_Height = height;
			m_Viewport = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
			m_ChannelCount = RetrieveChannelCountFromFormat(format);
			m_Format = format;
			m_DataCube = data;
			m_ArraySize = 6;
			m_Flags = RHI_Texture_Flags::RHI_Texture_Sampled;
			m_MipCount = static_cast<uint32_t>(m_Data.front().size());

			RHI_TextureCube::CreateResourceGPU();
		}

		// Creates a cubemap without any initial data, to be used as a render target.
		RHI_TextureCube(Context* context, const uint32_t width, const uint32_t height, const RHI_Format format) : RHI_Texture(context)
		{
			m_ResourceType = ResourceType::TextureCube;
			m_Width = width;
			m_Height = height;
			m_Viewport = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
			m_ChannelCount = RetrieveChannelCountFromFormat(format);
			m_Format = format;
			m_ArraySize = 6;
			m_Flags = RHI_Texture_Flags::RHI_Texture_Sampled;
			m_Flags |= IsDepthFormat() ? RHI_Texture_Flags::RHI_Texture_DepthStencil : RHI_Texture_Flags::RHI_Texture_RenderTarget;
			m_MipCount = 1;

			RHI_TextureCube::CreateResourceGPU();
		}

		~RHI_TextureCube();

		//RHI_Texture
		bool CreateResourceGPU() override;

	private:
		std::vector<std::vector<std::vector<std::byte>>> m_DataCube;
	};
}