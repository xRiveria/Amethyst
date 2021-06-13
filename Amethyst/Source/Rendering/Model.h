#pragma once
#include <memory>
#include <vector>
#include "../RHI/RHI_Utilities.h"
#include "../Resource/IResource.h"
#include "../Runtime/Math/BoundingBox.h"

namespace Amethyst
{
	class ResourceCache;
	class Entity;
	class Mesh;

	namespace Math
	{
		class BoundingBox;
	}

	class Model : public IResource, public std::enable_shared_from_this<Model>
	{
	public:
		Model(Context* engineContext);
		~Model();

		void Clear();

		// === IResource ===
		/// LoadFromFile
		/// SaveToFile
		// ======

		// Geometry
		void AppendGeometry(const std::vector<uint32_t>& indices, 
						    const std::vector<RHI_Vertex_PositionTextureNormalTangent>& vertices, 
							uint32_t* indexOffset = nullptr, 
							uint32_t* vertexOffset = nullptr) const;

		void RetrieveGeometry(uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset, uint32_t vertexCount, std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PositionTextureNormalTangent>* vertices);
		void UpdateGeometry();
		const Math::BoundingBox& RetrieveAABB() const { return m_AABB; }
		const std::shared_ptr<Mesh>& RetrieveMesh() const { return m_Mesh; }

		// Add Resources to the Model
		void SetRootEntity(const std::shared_ptr<Entity>& entity) { m_RootEntity = entity; }
		/// Add Material
		/// Add Texture

		// Misc
		bool IsAnimated() const { return m_IsAnimated; }
		void SetAnimated(const bool isAnimated) { m_IsAnimated = isAnimated; }
		const RHI_IndexBuffer* RetrieveIndexBuffer() const { return m_IndexBuffer.get(); }
		const RHI_VertexBuffer* RetrieveVertexBuffer() const { return m_VertexBuffer.get(); }
		std::shared_ptr<Model> RetrieveSharedPointer() { return shared_from_this(); }

	private:
		// Geometry
		bool GeometryCreateBuffers();
		float GeometryComputeNormalizedScale() const;

	private:
		std::weak_ptr<Entity> m_RootEntity;
		std::shared_ptr<RHI_VertexBuffer> m_VertexBuffer;
		std::shared_ptr<RHI_IndexBuffer> m_IndexBuffer;
		std::shared_ptr<Mesh> m_Mesh;

		Math::BoundingBox m_AABB;
		float m_NormalizedScale = 1.0f;
		bool m_IsAnimated = false;

		// Dependencies
		ResourceCache* m_ResourceCache;
		std::shared_ptr<RHI_Device> m_RHI_Device;
	};
}