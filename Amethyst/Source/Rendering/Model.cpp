#include "Amethyst.h"
#include "Model.h"
#include "Mesh.h"
#include "Renderer.h"
#include "../Runtime/ECS/Entity.h"
#include "../Runtime/ECS/Components/Transform.h"
#include "../Runtime/ECS/Components/Renderable.h"
#include "../Resource/ResourceCache.h"
#include "../RHI/RHI_IndexBuffer.h"
#include "../RHI/RHI_VertexBuffer.h"
#include "../RHI/RHI_Texture2D.h"
#include "../RHI/RHI_Vertex.h"

namespace Amethyst
{
	Model::Model(Context* engineContext) : IResource(engineContext, ResourceType::Model)
	{
		m_ResourceCache = m_Context->RetrieveSubsystem<ResourceCache>();
		m_RHI_Device = m_Context->RetrieveSubsystem<Renderer>()->RetrieveRHIDevice();
		m_Mesh = std::make_unique<Mesh>(); ///
	}

	Model::~Model()
	{
		Clear();
	}

	void Model::Clear()
	{
		m_RootEntity.reset();
		m_VertexBuffer.reset();
		m_IndexBuffer.reset();
		m_Mesh->Clear();
		m_AABB.Undefine();
		m_NormalizedScale = 1.0f;
		m_IsAnimated = false;
	}

	void Model::AppendGeometry(const std::vector<uint32_t>& indices, const std::vector<RHI_Vertex_PositionTextureNormalTangent>& vertices, uint32_t* indexOffset /*= nullptr*/, uint32_t* vertexOffset /*= nullptr*/) const
	{
		if (indices.empty() || vertices.empty())
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return;
		}

		// Append indices and vertex to the main mesh.
		m_Mesh->IndicesAppend(indices, indexOffset);
		m_Mesh->VerticesAppend(vertices, vertexOffset);
	}

	void Model::RetrieveGeometry(uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset, uint32_t vertexCount, std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PositionTextureNormalTangent>* vertices)
	{
		m_Mesh->RetrieveGeometry(indexOffset, indexCount, vertexOffset, vertexCount, indices, vertices);
	}

	void Model::UpdateGeometry()
	{
		if (m_Mesh->IndicesRetrieveCount() == 0 || m_Mesh->VerticesRetrieveCount() == 0)
		{
			AMETHYST_ERROR_INVALID_PARAMETER();
			return;
		}

		GeometryCreateBuffers();
		m_NormalizedScale = GeometryComputeNormalizedScale();
		m_AABB = Math::BoundingBox(m_Mesh->VerticesRetrieve().data(), static_cast<uint32_t>(m_Mesh->VerticesRetrieve().size()));
	}

	bool Model::GeometryCreateBuffers()
	{
		bool success = true;

		// Retrieve Geometry
		std::vector<uint32_t>& indices = m_Mesh->IndicesRetrieve();
		std::vector<RHI_Vertex_PositionTextureNormalTangent>& vertices = m_Mesh->VerticesRetrieve();

		if (!indices.empty())
		{
			m_IndexBuffer = std::make_shared<RHI_IndexBuffer>(m_RHI_Device);
			if (!m_IndexBuffer->Create(indices))
			{
				AMETHYST_ERROR("Failed to create Index Buffer for \"%s\".", RetrieveResourceName().c_str());
				success = false;
			}
		}
		else
		{
			AMETHYST_ERROR("Failed to create Index Buffer for \"%s\". Provided indices are empty.", RetrieveResourceName().c_str());
			success = false;
		}

		if (!vertices.empty())
		{
			m_VertexBuffer = std::make_shared<RHI_VertexBuffer>(m_RHI_Device);
			if (!m_VertexBuffer->Create(vertices))
			{
				AMETHYST_ERROR("Failed to create Vertex Buffer for \"%s\".", RetrieveResourceName().c_str());
				success = false;
			}
		}
		else
		{
			AMETHYST_ERROR("Failed to create Vertex Buffer for \"%s\". Provided vertices are empty.", RetrieveResourceName().c_str());
			success = false;
		}

		return success;
	}

	float Model::GeometryComputeNormalizedScale() const ///
	{
		// Compute Scale Offset
		const float scaleOffset = m_AABB.RetrieveExtents().Length();

		// Return normalized scale.
		return 1.0f / scaleOffset;
	}
}