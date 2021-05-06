#include "Amethyst.h"
#include "RHI_Texture.h"
#include "../Vendor/stb_image/stb_image.h"

namespace Amethyst
{
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
}