#include "Amethyst.h"
#include "../Rendering/Renderer.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "../RHI/RHI_IndexBuffer.h"
#include "../RHI/RHI_Vertex.h"
#include "Rectangle.h"

namespace Amethyst::Math
{
	const Rectangle Rectangle::Zero(0.0f, 0.0f, 0.0f, 0.0f);
	
	bool Rectangle::CreateBuffers(Renderer* renderer)
	{
		if (!renderer)
		{
			//AMETHYST_ERROR_INVALID_PARAMETER();
			return false;
		}

		//Compute Screen Coordinates
		const RHI_Viewport viewport = renderer->RetrieveViewport();
		const float sc_Left = -(viewport.m_Width * 0.5f) + m_Left;
		const float sc_Right = sc_Left + Width();
		const float sc_Top = (viewport.m_Height * 0.5f) - m_Top;
		const float sc_Bottom = sc_Top - Height();

		//Create Vertex Buffer
		const RHI_Vertex_PositionTexture vertices[6] =
		{
			//First Triangle
			RHI_Vertex_PositionTexture(Vector3(sc_Left, sc_Top, 0.0f), Vector2(0.0f, 0.0f)),	    //Top Left
			RHI_Vertex_PositionTexture(Vector3(sc_Right, sc_Bottom, 0.0f), Vector2(1.0f, 1.0f)),	//Bottom Right
			RHI_Vertex_PositionTexture(Vector3(sc_Left, sc_Bottom, 0.0f), Vector2(0.0f, 1.0f)),		//Bottom Left

			//Second Triangle
			RHI_Vertex_PositionTexture(Vector3(sc_Left, sc_Top, 0.0f), Vector2(0.0f, 0.0f)),		//Top Left
			RHI_Vertex_PositionTexture(Vector3(sc_Right, sc_Top, 0.0f), Vector2(1.0f, 0.0f)),		//Top Right
			RHI_Vertex_PositionTexture(Vector3(sc_Right, sc_Bottom, 0.0f), Vector2(1.0f, 1.0f))     //BottomRight
		};

		m_VertexBuffer = std::make_shared<RHI_VertexBuffer>(renderer->RetrieveRHIDevice());
		if (!m_VertexBuffer->Create(vertices, 6))
		{
			//AMETHYST_ERROR("Failed to create vertex buffer.");
			return false;
		}

		//Create Index Buffer
		const uint32_t indices[6] = { 0, 1, 2, 3, 4, 5 };
		m_IndexBuffer = std::make_shared<RHI_IndexBuffer>(renderer->RetrieveRHIDevice());
		if (!m_IndexBuffer->Create(indices, 6))
		{
			//AMETHYST_ERROR("Failed to create index buffer.");
			return false;
		}

		return true;
	}


}