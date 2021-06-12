#pragma once
#include <vector>
#include "../RHI/RHI_Utilities.h"
#include "../RHI/RHI_Vertex.h"

namespace Amethyst
{
	class Mesh
	{
	public:
		Mesh() = default;
		~Mesh() {  }

		// Geometry
		void Clear();
		void RetrieveGeometry(
			uint32_t indexOffset,
			uint32_t indexCount,
			uint32_t vertexOffset,
			uint32_t vertexCount,
			std::vector<uint32_t>* indices,
			std::vector<RHI_Vertex_PositionTextureNormalTangent>* vertices
		);
		uint32_t RetrieveMemoryUsage() const;

		// Vertices
		void VertexAdd(const RHI_Vertex_PositionTextureNormalTangent& vertex);
		void VerticesAppend(const std::vector<RHI_Vertex_PositionTextureNormalTangent>& vertices, uint32_t* vertexOffset);
		void VerticesSet(const std::vector<RHI_Vertex_PositionTextureNormalTangent>& vertices) { m_Vertices = vertices; };
		std::vector<RHI_Vertex_PositionTextureNormalTangent>& VerticesRetrieve() { return m_Vertices; }
		uint32_t VerticesRetrieveCount() const;

		// Indices
		void IndexAdd(uint32_t index) { m_Indices.emplace_back(index); }
		void IndicesAppend(const std::vector<uint32_t>& indices, uint32_t* indexOffset);
		void IndicesSet(const std::vector<uint32_t>& indices) { m_Indices = indices; }
		std::vector<uint32_t>& IndicesRetrieve() { return m_Indices; }
		uint32_t IndicesRetrieveCount() const { return static_cast<uint32_t>(m_Indices.size()); }

		// Misc
		uint32_t RetrieveTriangleCount() const { return IndicesRetrieveCount() / 3; }

	private:
		std::vector<RHI_Vertex_PositionTextureNormalTangent> m_Vertices;
		std::vector<uint32_t> m_Indices;
	};
}