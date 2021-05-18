#pragma once
#include <memory>
#include <vector>
#include "RHI_Utilities.h"
#include "RHI_Vertex.h"
#include "../Core/AmethystObject.h"
#include "../Runtime/Log/Log.h"

namespace Amethyst
{
	struct VertexAttribute
	{
		VertexAttribute(const std::string& name, const uint32_t location, const uint32_t binding, const RHI_Format format, const uint32_t offset)
		{
			this->m_Name = name;
			this->m_Location = location;
			this->m_Binding = binding;
			this->m_Format = format;
			this->m_Offset = offset;
		}

		std::string m_Name;
		uint32_t m_Location;
		uint32_t m_Binding;
		RHI_Format m_Format;
		uint32_t m_Offset;
	};

	class RHI_InputLayout : public AmethystObject
	{
	public:
		RHI_InputLayout(const std::shared_ptr<RHI_Device>& rhi_Device)
		{
			m_RHI_Device = rhi_Device;
		}

		~RHI_InputLayout();

		bool Create(const RHI_Vertex_Type vertexType, void* vertexShaderBlob = nullptr)
		{
			if (vertexType == RHI_Vertex_Type::RHI_Vertex_Type_Unknown)
			{
				AMETHYST_ERROR("Unknown Vertex Type.");
				return false;
			}

			const uint32_t binding = 0;

			if (vertexType == RHI_Vertex_Type::RHI_Vertex_Type_Position)
			{
				m_VertexAttributes =
				{
					{ "POSITION", 0, binding, RHI_Format_R32G32B32_Float, offsetof(RHI_Vertex_Position, m_Position) }
				};
			}

			if (vertexType == RHI_Vertex_Type::RHI_Vertex_Type_PositionTexture)
			{
				m_VertexAttributes =
				{
					{ "POSITION", 0, binding, RHI_Format_R32G32B32_Float, offsetof(RHI_Vertex_Position, m_Position) },
					{ "TEXCOORD", 1, binding, RHI_Format_R32G32_Float,    offsetof(RHI_Vertex_PositionTexture, m_Texture) }
				};
			}

			if (vertexType == RHI_Vertex_Type::RHI_Vertex_Type_PositionColor)
			{
				m_VertexAttributes =
				{
					{ "POSITION", 0, binding, RHI_Format_R32G32B32_Float,    offsetof(RHI_Vertex_PositionColor, m_Position) },
					{ "COLOR",    1, binding, RHI_Format_R32G32B32A32_Float, offsetof(RHI_Vertex_PositionColor, m_Color) }
				};
			}

			if (vertexType == RHI_Vertex_Type::RHI_Vertex_Type_Position2DTextureColor8)
			{
				m_VertexAttributes =
				{
					{ "POSITION", 0, binding, RHI_Format_R32G32_Float, offsetof(RHI_Vertex_Position2DTextureColor8, m_Position) },
					{ "TEXCOORD", 1, binding, RHI_Format_R32G32_Float, offsetof(RHI_Vertex_Position2DTextureColor8, m_Texture) },
					{ "COLOR",    2, binding, RHI_Format_R8G8B8A8_Unorm, offsetof(RHI_Vertex_Position2DTextureColor8, m_Color) }
				};
			}

			if (vertexType == RHI_Vertex_Type::RHI_Vertex_Type_PositionTextureNormalTangent)
			{
				m_VertexAttributes =
				{
					{ "POSITION", 0, binding, RHI_Format_R32G32B32_Float, offsetof(RHI_Vertex_PositionTextureNormalTangent, m_Position) },
					{ "TEXCOORD", 1, binding, RHI_Format_R32G32_Float, offsetof(RHI_Vertex_PositionTextureNormalTangent, m_Texture) },
					{ "NORMAL",   2, binding, RHI_Format_R32G32B32_Float, offsetof(RHI_Vertex_PositionTextureNormalTangent, m_Normal) },
					{ "TANGENT",  3, binding, RHI_Format_R32G32B32_Float, offsetof(RHI_Vertex_PositionTextureNormalTangent, m_Tangent) }
				};
			}

			if (vertexShaderBlob && !m_VertexAttributes.empty())
			{
				return _CreateResource(vertexShaderBlob);
			}

			return true;
		}

	private:
		RHI_Vertex_Type m_VertexType;

		//API
		bool _CreateResource(void* vertexShaderBlob); //A shader blob is an opaque binary object. It has size and data, and could be anything.
		std::shared_ptr<RHI_Device> m_RHI_Device;
		void* m_Resource = nullptr;
		std::vector<VertexAttribute> m_VertexAttributes;
	};

}