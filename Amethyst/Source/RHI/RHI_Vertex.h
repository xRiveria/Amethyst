#pragma once
#include "../Runtime/Math/Vector2.h"
#include "../Runtime/Math/Vector3.h"
#include "../Runtime/Math/Vector4.h"

namespace Amethyst
{
	struct RHI_Vertex_Undefined{};

	struct RHI_Vertex_Position
	{
		RHI_Vertex_Position(const Math::Vector3& position)
		{
			this->m_Position[0] = position.m_X;
			this->m_Position[1] = position.m_Y;
			this->m_Position[2] = position.m_Z;
		}

		float m_Position[3] = { 0 };
	};

	struct RHI_Vertex_PositionColor
	{
		RHI_Vertex_PositionColor() = default;

		RHI_Vertex_PositionColor(const Math::Vector3& position, const Math::Vector4& color)
		{
			this->m_Position[0] = position.m_X;
			this->m_Position[1] = position.m_Y;
			this->m_Position[2] = position.m_Z;

			this->m_Color[0] = color.m_X;
			this->m_Color[1] = color.m_Y;
			this->m_Color[2] = color.m_Z;
			this->m_Color[3] = color.m_W;
		}

		float m_Position[3] = { 0 };
		float m_Color[4] = { 0 };
	};

	struct RHI_Vertex_PositionTexture
	{
		RHI_Vertex_PositionTexture(const float positionX, const float positionY, const float positionZ, const float textureX, const float textureY)
		{
			m_Position[0] = positionX;
			m_Position[1] = positionY;
			m_Position[2] = positionZ;

			m_Texture[0] = textureX;
			m_Texture[1] = textureY;
		}

		RHI_Vertex_PositionTexture(const Math::Vector3& position, const Math::Vector2& texture)
		{
			this->m_Position[0] = position.m_X;
			this->m_Position[1] = position.m_Y;
			this->m_Position[2] = position.m_Z;

			this->m_Texture[0] = texture.m_X;
			this->m_Texture[1] = texture.m_Y;
		}

		float m_Position[3] = { 0 };
		float m_Texture[2] = { 0 };
	};

	struct RHI_Vertex_Position2DTextureColor8
	{
		RHI_Vertex_Position2DTextureColor8() = default;

		float m_Position[2] = { 0 };
		float m_Texture[2] = { 0 };

		uint32_t m_Color = 0;
	};

	struct RHI_Vertex_PositionTextureNormalTangent
	{
		RHI_Vertex_PositionTextureNormalTangent() = default;
		
		RHI_Vertex_PositionTextureNormalTangent(const Math::Vector3& position, const Math::Vector2& texture,
											    const Math::Vector3& normal = Math::Vector3::Zero, const Math::Vector3& tangent = Math::Vector3::Zero)
		{
			this->m_Position[0] = position.m_X;
			this->m_Position[1] = position.m_Y;
			this->m_Position[2] = position.m_Z;

			this->m_Texture[0] = texture.m_X;
			this->m_Texture[1] = texture.m_Y;

			this->m_Normal[0] = normal.m_X;
			this->m_Normal[1] = normal.m_Y;
			this->m_Normal[2] = normal.m_Z;

			this->m_Tangent[0] = tangent.m_X;
			this->m_Tangent[1] = tangent.m_Y;
			this->m_Tangent[2] = tangent.m_Z;
		}

		float m_Position[3] = { 0 };
		float m_Texture[2] = { 0 };
		float m_Normal[3] = { 0 };
		float m_Tangent[3] = { 0 };
	};

	static_assert(std::is_trivially_copyable<RHI_Vertex_Position>::value, "RHI_Vertex_Position is not trivially copyable.");
	static_assert(std::is_trivially_copyable<RHI_Vertex_PositionTexture>::value, "RHI_Vertex_PositionTexture is not trivially copyable.");
	static_assert(std::is_trivially_copyable<RHI_Vertex_PositionColor>::value, "RHI_Vertex_PositionColor is not trivially copyable.");
	static_assert(std::is_trivially_copyable<RHI_Vertex_Position2DTextureColor8>::value, "RHI_Vertex_Position2DTextureColor8 is not trivially copyable.");
	static_assert(std::is_trivially_copyable<RHI_Vertex_PositionTextureNormalTangent>::value, "RHI_Vertex_PositionTextureNormalTangent is not trivially copyable.");

	enum RHI_Vertex_Type
	{
		RHI_Vertex_Type_Unknown,
		RHI_Vertex_Type_Position,
		RHI_Vertex_Type_PositionColor,
		RHI_Vertex_Type_PositionTexture,
		RHI_Vertex_Type_PositionTextureNormalTangent,
		RHI_Vertex_Type_Position2DTextureColor8
	};

	template<typename T>
	constexpr RHI_Vertex_Type RHI_Vertex_TypeToEnum() { return RHI_Vertex_Type::RHI_Vertex_Type_Unknown; }

	//Explicit Template Instantiation
	template<> inline RHI_Vertex_Type RHI_Vertex_TypeToEnum<RHI_Vertex_Position>() { return RHI_Vertex_Type::RHI_Vertex_Type_Position; }
	template<> inline RHI_Vertex_Type RHI_Vertex_TypeToEnum<RHI_Vertex_PositionColor>() { return RHI_Vertex_Type::RHI_Vertex_Type_PositionColor; }
	template<> inline RHI_Vertex_Type RHI_Vertex_TypeToEnum<RHI_Vertex_PositionTexture>() { return RHI_Vertex_Type::RHI_Vertex_Type_PositionTexture; }
	template<> inline RHI_Vertex_Type RHI_Vertex_TypeToEnum<RHI_Vertex_PositionTextureNormalTangent>() { return RHI_Vertex_Type::RHI_Vertex_Type_PositionTextureNormalTangent; }
	template<> inline RHI_Vertex_Type RHI_Vertex_TypeToEnum<RHI_Vertex_Position2DTextureColor8>() { return RHI_Vertex_Type::RHI_Vertex_Type_Position2DTextureColor8; }
}