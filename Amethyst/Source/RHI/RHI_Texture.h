#pragma once
#include <string>
#include <GL/glew.h>

#include <memory>
#include <array>
#include "RHI_Viewport.h"
#include "RHI_Utilities.h"
#include "../Resource/IResource.h"


namespace Amethyst
{
	enum RHI_Texture_Flags : uint16_t
	{
		RHI_Texture_Sampled					= 1 << 0,
		RHI_Texture_Storage					= 1 << 1,
		RHI_Texture_RenderTarget		    = 1 << 2,
		RHI_Texture_DepthStencil			= 1 << 3,
		RHI_Texture_DepthStencilReadOnly    = 1 << 4,
		RHI_Texture_Grayscale				= 1 << 5,
		RHI_Texture_Transparent				= 1 << 6,
		RHI_Texture_GenerateMipsWhenLoading = 1 << 7
	};

	//Shader resource views wrap textures in a format that the shaders can access them.
	enum RHI_Shader_View_Type : uint8_t
	{
		RHI_Shader_View_ColorDepth,			
		RHI_Shader_View_Stencil,			
		RHI_Shader_View_Unordered_Access	//Slightly more performance costly, but allows a texture to be written to at the same it is being read from.
	};

	class RHI_Texture : public IResource
	{
	public:
		RHI_Texture(Context* context);
		~RHI_Texture();

		//IResource
		bool SaveToFile(const std::string& filePath) override;  ///
		bool LoadFromFile(const std::string& filePath) override; ///

		//Properties
		uint32_t RetrieveWidth() const { return m_Width; }
		void SetWidth(const uint32_t width) { m_Width = width; }

		uint32_t RetrieveHeight() const { return m_Height; }
		void SetHeight(const uint32_t height) { m_Height = height; }

		bool RetrieveIsGrayscale() const { return m_Flags & RHI_Texture_Flags::RHI_Texture_Grayscale; }
		void SetGrayscale(const bool isGrayscale) { isGrayscale ? m_Flags |= RHI_Texture_Grayscale : m_Flags &= ~RHI_Texture_Flags::RHI_Texture_Grayscale; }

		bool RetrieveIsTransparent() const { return m_Flags & RHI_Texture_Flags::RHI_Texture_Transparent; }
		void SetTransparency(const bool isTransparent) { isTransparent ? m_Flags |= RHI_Texture_Transparent : m_Flags &= ~RHI_Texture_Flags::RHI_Texture_Transparent; }

		uint32_t RetrieveBitsPerChannel() const { return m_BitsPerChannel; }
		void SetBitsPerChannel(const uint32_t bits) { m_BitsPerChannel = bits; }
		uint32_t RetrieveBytesPerChannel() const { return m_BitsPerChannel / 8l; }
		uint32_t RetrieveBytesPerPixel() const { return (m_BitsPerChannel / 8) * m_ChannelCount; }

		uint32_t RetrieveChannelCount() const { return m_ChannelCount; }
		void SetChannelCount(const uint32_t channelCount) { m_ChannelCount = channelCount; }

		RHI_Format RetrieveFormat() const { return m_Format; }
		void SetFormat(const RHI_Format format) { m_Format = format; }

		//Data
		bool HasData() const { return !m_Data.empty(); }
		void SetData(const std::vector<std::vector<std::byte>>& data) { m_Data = data; }
		bool HasMipmaps() const { return m_MipCount > 1; }
		uint8_t RetrieveMipCount() const { return m_MipCount; }
		std::vector<std::byte>& AddMip() { return m_Data.emplace_back(std::vector<std::byte>()); }
		std::vector<std::vector<std::byte>>& RetrieveMips() { return m_Data; }
		std::vector<std::byte>& RetrieveMip(const uint8_t mipIndex);
		std::vector<std::byte> RetrieveOrLoadMip(const uint8_t mipIndex);

		//Binding Type
		bool IsSampled() const { return m_Flags & RHI_Texture_Flags::RHI_Texture_Sampled; }
		bool IsStorage() const { return m_Flags & RHI_Texture_Flags::RHI_Texture_Storage; }
		bool IsDepthStencil() const { return m_Flags & RHI_Texture_Flags::RHI_Texture_DepthStencil; }
		bool IsRenderTarget() const { return m_Flags & RHI_Texture_Flags::RHI_Texture_RenderTarget; }

		//Format Type
		bool IsDepthFormat() const { return m_Format == RHI_Format::RHI_Format_D32_Float || m_Format == RHI_Format::RHI_Format_D32_Float_S8X24_Uint; }
		bool IsStencilFormat() const { return m_Format == RHI_Format::RHI_Format_D32_Float_S8X24_Uint; }
		bool IsDepthStencilFormat() const { return IsDepthFormat && IsStencilFormat(); }
		bool IsColorFormat() const { return !IsDepthStencilFormat(); }
		
		//Layout
		void SetLayout(const RHI_Image_Layout layout, RHI_CommandList* commandList = nullptr);
		RHI_Image_Layout RetrieveLayout() const { return m_ImageLayout; }

		//Misc
		uint32_t RetrieveArraySize() const { return m_ArraySize; }
		const RHI_Viewport& RetrieveViewport() const { return m_Viewport; }
		uint16_t RetrieveFlags() const { return m_Flags; }

		//GPU Resources
		void* RetrieveResource() const { return m_Resource; }
		void SetResource(void* resource) { m_Resource = resource; }
		void* RetrieveResourceView(const uint32_t viewIndex = 0)						const { return m_ResourceView[viewIndex]; }
		void* RetrieceResourceView_UnorderedAccess()									const { return m_ResourceView_UnorderedAccess; }
		void* RetrieveResourceView_DepthStencil(const uint32_t viewIndex = 0)			const { return viewIndex < m_ResourceView_DepthStencil.size() ? m_ResourceView_DepthStencil[viewIndex] : nullptr; }
		void* RetrieveResourceView_DepthStencilReadOnly(const uint32_t viewIndex = 0)   const { return viewIndex < m_ResourceView_DepthStencilReadOnly.size() ? m_ResourceView_DepthStencilReadOnly[viewIndex] : nullptr; }
		void* RetrieveResourceView_RenderTarget(const uint32_t viewIndex = 0)			const { return viewIndex < m_ResourceView_RenderTarget.size() ? m_ResourceView_RenderTarget[viewIndex] : nullptr; }

	protected:
		bool LoadFromFile_NativeFormat(const std::string& filePath);
		bool LoadFromFile_ForeignFormat(const std::string& filePath, bool generateMipmaps);
		static uint32_t RetrieveChannelCountFromFormat(RHI_Format format);
		virtual bool CreateResourceGPU() 
		{ 
			AMETHYST_ERROR("Function not implemented by API.");
			return false;
		}

	protected:
		uint32_t m_BitsPerChannel = 8;
		uint32_t m_Width = 0;
		uint32_t m_Height = 0;
		uint32_t m_ChannelCount = 4;
		uint32_t m_ArraySize = 1;	///
		uint8_t m_MipCount = 1;
		RHI_Format m_Format = RHI_Format::RHI_Format_Undefined;
		RHI_Image_Layout m_ImageLayout = RHI_Image_Layout::Undefined;
		uint16_t m_Flags = 0;
		RHI_Viewport m_Viewport;
		std::vector<std::vector<std::byte>> m_Data; ///
		std::shared_ptr<RHI_Device> m_RHI_Device;

		//API
		void* m_ResourceView[2] = { nullptr, nullptr }; //Color/Depth, Stencil
		void* m_ResourceView_UnorderedAccess = nullptr; ///
		void* m_Resource = nullptr;
		std::array<void*, g_RHI_MaxRenderTargetCount> m_ResourceView_RenderTarget = { nullptr };
		std::array<void*, g_RHI_MaxRenderTargetCount> m_ResourceView_DepthStencil = { nullptr };
		std::array<void*, g_RHI_MaxRenderTargetCount> m_ResourceView_DepthStencilReadOnly = { nullptr };

	private:
		uint32_t RetrieveByteCount();
	};
}

#if Old 1
	class RHI_Texture
	{
	public:
		RHI_Texture(const std::string& filePath, GLenum textureTarget, GLenum textureInternalFormat, bool sRGB = false);

		//Retrieves
		unsigned int RetrieveTextureID() const { return m_TextureID; }
		void BindTexture(int textureUnit = 0);
		void UnbindTexture();
		void ResizeTexture(unsigned int textureWidth, unsigned int textureHeight);

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
#endif