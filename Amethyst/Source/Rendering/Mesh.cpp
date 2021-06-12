#include "Amethyst.h"
#include "Mesh.h"
#include "../RHI/RHI_Vertex.h"

namespace Amethyst
{
	void Mesh::Clear()
	{
		m_Vertices.clear();
		m_Vertices.shrink_to_fit();
		m_Indices.clear();
		m_Indices.shrink_to_fit();
	}

	uint32_t Mesh::RetrieveMemoryUsage() const
	{
		uint32_t size = 0;
		size += uint32_t(m_Vertices.size() * sizeof(RHI_Vertex_PositionTextureNormalTangent));
		size += uint32_t(m_Indices.size() * sizeof(uint32_t));

		return size;
	}

	void Mesh::RetrieveGeometry(uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset, uint32_t vertexCount, std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PositionTextureNormalTangent>* vertices)
	{
		if ((indexOffset == 0 && indexCount == 0) || (vertexOffset == 0 && vertexCount == 0) || !vertices || !indices)
		{
			AMETHYST_ERROR("Mesh::RetrieveGeometry: Invalid Parameters.");
			return;
		}

		// Indices
		const auto indexFirst = m_Indices.begin() + indexOffset; ///
		const auto indexLast = m_Indices.begin() + indexOffset + indexCount;
		*indices = std::vector<uint32_t>(indexFirst, indexLast);

		// Vertices
		const auto vertexFirst = m_Vertices.begin() + vertexOffset; ///
		const auto vertexLast = m_Vertices.begin() + vertexOffset + vertexCount;
		*vertices = std::vector<RHI_Vertex_PositionTextureNormalTangent>(vertexFirst, vertexLast);
	}

	uint32_t Mesh::VerticesRetrieveCount() const
	{
		return static_cast<uint32_t>(m_Vertices.size());
	}

	void Mesh::VertexAdd(const RHI_Vertex_PositionTextureNormalTangent& vertex)
	{
		m_Vertices.emplace_back(vertex);
	}

	void Mesh::VerticesAppend(const std::vector<RHI_Vertex_PositionTextureNormalTangent>& vertices, uint32_t* vertexOffset)
	{
		if (vertexOffset)
		{
			*vertexOffset = static_cast<uint32_t>(m_Vertices.size());
		}

		m_Vertices.insert(m_Vertices.end(), vertices.begin(), vertices.end());
	}

	void Mesh::IndicesAppend(const std::vector<uint32_t>& indices, uint32_t* indexOffset)
	{
		if (indexOffset)
		{
			*indexOffset = static_cast<uint32_t>(m_Indices.size());
		}

		m_Indices.insert(m_Indices.end(), indices.begin(), indices.end()); // Insert at the end of our vector the entire length of our vertices vector.
	}
}