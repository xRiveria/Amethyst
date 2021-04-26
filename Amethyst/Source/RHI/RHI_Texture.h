#pragma once
#include <string>
#include <GL/glew.h>

/*
	Replace with Virtuals.	
*/

namespace Amethyst
{
	class RHI_Texture
	{
	public:
		RHI_Texture(const std::string& filePath, GLenum textureTarget, GLenum textureInternalFormat, bool sRGB = false);

		//Retrieves
		unsigned int RetrieveTextureID() const { return m_TextureID; }
		void BindTexture(int textureUnit = 0);
		void UnbindTexture();

		unsigned int RetrieveTextureWidth() const { return m_TextureWidth; }
		unsigned int RetrieveTextureHeight() const { return m_TextureHeight; }

	private:
		void GenerateTexture2D(void* textureData);

	private:
		unsigned int m_TextureWidth = 0;
		unsigned int m_TextureHeight = 0;

		GLenum m_TextureTarget = GL_TEXTURE_2D;
		GLenum m_TextureInternalFormat = GL_RGBA;
		GLenum m_TextureFormat = GL_RGBA;
		GLenum m_TextureDataType = GL_UNSIGNED_BYTE;

		GLenum m_TextureMinificationFilter = GL_LINEAR;
		GLenum m_TextureMagnificationFilter = GL_NEAREST;

		GLenum m_TextureWrapS = GL_REPEAT;
		GLenum m_TextureWrapT = GL_REPEAT;
		GLenum m_TextureWrapR = GL_REPEAT;

		unsigned int m_TextureID = 0;
		std::string m_FilePath;
	};
}