#include "../Vendor/stb_image/stb_image.h"

#include "Amethyst.h"
#include "RHI_Texture.h"
#include "RHI_Device.h"
#include "../Rendering/Renderer.h"
#include "../Resource/ResourceCache.h"
//Image Import

namespace Amethyst
{
	RHI_Texture::RHI_Texture(Context* context) : IResource(context, ResourceType::Texture)
	{
		m_RHI_Device = context->RetrieveSubsystem<Renderer>()->RetrieveRHIDevice();
	}

	RHI_Texture::~RHI_Texture()
	{
		m_Data.clear();
		m_Data.shrink_to_fit();
	}

	bool RHI_Texture::SaveToFile(const std::string& filePath)
	{
		return false;
	}

	bool RHI_Texture::LoadFromFile(const std::string& filePath)
	{
		return false;
	}

	std::vector<std::byte>& RHI_Texture::RetrieveMip(const uint8_t mipIndex)
	{
		static std::vector<std::byte> empty;

		if (mipIndex >= m_Data.size())
		{
			AMETHYST_WARNING("Index out of range.");
			return empty;
		}

		return m_Data[mipIndex];
	}

	std::vector<std::byte> RHI_Texture::RetrieveOrLoadMip(const uint8_t mipIndex)
	{
		return std::vector<std::byte>();
	}

	bool RHI_Texture::LoadFromFile_NativeFormat(const std::string& filePath)
	{
		return false;
	}

	bool RHI_Texture::LoadFromFile_ForeignFormat(const std::string& filePath, bool generateMipmaps)
	{
		return false;
	}

	uint32_t RHI_Texture::RetrieveChannelCountFromFormat(RHI_Format format)
	{
		switch (format)
		{
			case RHI_Format_R8_Unorm:               return 1;
			case RHI_Format_R16_Uint:               return 1;
			case RHI_Format_R16_Float:              return 1;
			case RHI_Format_R32_Uint:               return 1;
			case RHI_Format_R32_Float:              return 1;
			case RHI_Format_R8G8_Unorm:             return 2;
			case RHI_Format_R16G16_Float:           return 2;
			case RHI_Format_R32G32_Float:           return 2;
			case RHI_Format_R11G11B10_Float:        return 3;
			case RHI_Format_R16G16B16A16_Snorm:     return 3;
			case RHI_Format_R32G32B32_Float:        return 3;
			case RHI_Format_R8G8B8A8_Unorm:         return 4;
			case RHI_Format_R10G10B10A2_Unorm:      return 4;
			case RHI_Format_R16G16B16A16_Float:     return 4;
			case RHI_Format_R32G32B32A32_Float:     return 4;
			case RHI_Format_D32_Float:              return 1;
			case RHI_Format_D32_Float_S8X24_Uint:   return 2;

			default:                                return 0;
		}
	}

	uint32_t RHI_Texture::RetrieveByteCount()
	{
		uint32_t byteCount = 0;

		for (std::vector<std::byte>& mip : m_Data)
		{
			byteCount += static_cast<uint32_t>(mip.size()); //For each item in the byte vector, add its elements to the total byte count.
		}

		return byteCount;
	}




#if Old 1
	RHI_Texture::RHI_Texture(const std::string& filePath, GLenum textureTarget, GLenum textureInternalFormat, bool sRGB)
	{
		m_FilePath = filePath;
		m_TextureTarget = textureTarget;
		m_TextureInternalFormat = textureInternalFormat;

		if (m_TextureInternalFormat == GL_RGB || m_TextureInternalFormat == GL_SRGB)
		{
			m_TextureInternalFormat = sRGB ? GL_SRGB : GL_RGB;
		}
		if (m_TextureInternalFormat == GL_RGBA || m_TextureInternalFormat == GL_SRGB_ALPHA)
		{
			m_TextureInternalFormat = sRGB ? GL_SRGB_ALPHA : GL_RGBA;
		}

		//Flip textures on their Y coordinates while loading.
		//stbi_set_flip_vertically_on_load(true);

		int textureWidth, textureHeight, componentCount;
		unsigned char* textureData = stbi_load(filePath.c_str(), &textureWidth, &textureHeight, &componentCount, 0);
		if (textureData)
		{
			GLenum format;
			if (componentCount == 1)
			{
				format = GL_RED;
			}
			else if (componentCount == 3)
			{
				format = GL_RGB;
			}
			else if (componentCount == 4)
			{
				format = GL_RGBA;
			}

			m_TextureFormat = format;
			m_TextureWidth = textureWidth;
			m_TextureHeight = textureHeight;
			m_TextureDataType = GL_UNSIGNED_BYTE;

			if (textureTarget == GL_TEXTURE_2D)
			{
				glGenTextures(1, &m_TextureID);

				BindTexture();

				glTexImage2D(m_TextureTarget, 0, m_TextureInternalFormat, m_TextureWidth, m_TextureHeight, 0, format, m_TextureDataType, textureData);
				glTexParameteri(m_TextureTarget, GL_TEXTURE_MIN_FILTER, m_TextureMinificationFilter);
				glTexParameteri(m_TextureTarget, GL_TEXTURE_MAG_FILTER, m_TextureMagnificationFilter);
				glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_S, m_TextureWrapS);
				glTexParameteri(m_TextureTarget, GL_TEXTURE_WRAP_T, m_TextureWrapT);

				UnbindTexture();

				AMETHYST_INFO("Loaded Texture at: %s", filePath.c_str());
			}

			stbi_image_free(textureData);
		}
		else
		{
			stbi_image_free(textureData);
			AMETHYST_WARNING("Failed to load Texture at: %s", filePath.c_str());
		}

		if (textureData == NULL)
		{
			AMETHYST_WARNING("Failed to load Texture at: %s", filePath.c_str());
		}
	}

	void RHI_Texture::BindTexture(int textureUnit)
	{
		if (textureUnit >= 0)
		{
			glActiveTexture(GL_TEXTURE0 + textureUnit);
		}
		glBindTexture(m_TextureTarget, m_TextureID);
	}

	void RHI_Texture::UnbindTexture()
	{
		glBindTexture(m_TextureTarget, 0);
	}

	void RHI_Texture::ResizeTexture(unsigned int textureWidth, unsigned int textureHeight)
	{
		BindTexture();
		glTexImage2D(GL_TEXTURE_2D, 0, m_TextureInternalFormat, textureWidth, textureHeight, 0, m_TextureFormat, m_TextureDataType, 0);
	}
#endif

}