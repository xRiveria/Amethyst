#pragma once
#include "RHI_Texture.h"
#include "RHI_Viewport.h"

namespace Amethyst
{
	class RHI_Texture2D : public RHI_Texture
	{
	public:
		//Creates a texture from data.
		RHI_Texture2D(Context* context, const uint32_t width, const uint32_t height, const RHI_Format format, const std::vector<std::vector<std::byte>>& data) : RHI_Texture(context)
		{
			m_ResourceType = ResourceType::Texture2D;
			m_Width = width;
			m_Height = height;
			m_Viewport = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
			m_ChannelCount = RetrieveChannelCountFromFormat(format);
			m_Format = format;
			m_Data = data;
			m_Flags = RHI_Texture_Flags::RHI_Texture_Sampled;
			m_MipCount = static_cast<uint32_t>(data.size());

			RHI_Texture2D::CreateResourceGPU();
		}

		//Creates a texture from data.
		RHI_Texture2D(Context* context, const uint32_t width, const uint32_t height, const RHI_Format format, const std::vector<std::byte>& data) : RHI_Texture(context)
		{
			m_Data.emplace_back(data);

			m_ResourceType = ResourceType::Texture2D;
			m_Width = width;
			m_Height = height;
			m_Viewport = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
			m_ChannelCount = RetrieveChannelCountFromFormat(format);
			m_Format = format;
			m_Flags = RHI_Texture_Flags::RHI_Texture_Sampled;
			m_MipCount = 1;

			RHI_Texture2D::CreateResourceGPU();
		}

		//Creates an empty texture (inteded for deferred loading).
		RHI_Texture2D(Context* context, const bool generateMipmaps = true) : RHI_Texture(context)
		{
			m_ResourceType = ResourceType::Texture2D;
			m_Flags = RHI_Texture_Flags::RHI_Texture_Sampled;
			m_Flags |= generateMipmaps ? RHI_Texture_Flags::RHI_Texture_GenerateMipsWhenLoading : 0;
		}

		//Creates a texture without any data (intended for usage as a render target).
		RHI_Texture2D(Context* context, const uint32_t width, const uint32_t height, const RHI_Format format, const uint32_t arraySize = 1, const uint16_t flags = 0, std::string name = "") : RHI_Texture(context)
		{
			m_Name = name;
			m_ResourceType = ResourceType::Texture2D;
			m_Width = width;
			m_Height = height;
			m_ChannelCount = RetrieveChannelCountFromFormat(format);
			m_Viewport = RHI_Viewport(0, 0, static_cast<float>(width), static_cast<float>(height));
			m_Format = format;
			m_ArraySize = arraySize;
			m_Flags = flags;
			m_Flags |= RHI_Texture_Flags::RHI_Texture_Sampled;
			m_Flags |= IsDepthFormat() ? RHI_Texture_Flags::RHI_Texture_DepthStencil : (RHI_Texture_Flags::RHI_Texture_RenderTarget | RHI_Texture_Flags::RHI_Texture_Storage); //Need to optimize this as not every RT is used in a compute shader.
			m_MipCount = 1;

			RHI_Texture2D::CreateResourceGPU();
		}

		~RHI_Texture2D();

		//RHI Texture
		bool CreateResourceGPU() override;
	};
}