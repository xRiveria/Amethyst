#pragma once
#include "IComponent.h"
#include <vector>
#include "../Runtime/Math/BoundingBox.h"
#include "../Runtime/Math/Matrix.h"

namespace Amethyst
{
	class Model;
	class Mesh;
	/// class Material;

	namespace Math
	{
		class Vector3;
	}

	enum GeometryType
	{
		Geometry_Custom,
		Geometry_Default_Cube,
		Geometry_Default_Quad,
		///Geometry_Default_Sphere,
		///Geometry_Default_Cylinder,
		///Geometry_Default_Cone
	};

	class Renderable : public IComponent
	{
	public:
		Renderable(Context* engineContext, Entity* entity, uint32_t componentID = 0);
		~Renderable() = default;

		// === IComponent ===
		/// Serialize
		/// Deserialize
		
		// Geometry
		void GeometrySet(
			const std::string& geometryName,
			uint32_t indexOffset,
			uint32_t indexCount,
			uint32_t vertexOffset,
			uint32_t vertexCount,
			const Math::BoundingBox& boundingBox,
			Model* model
		);

		void GeometryClear();
		void GeometrySet(GeometryType type);
		void GeometryRetrieve(std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PositionTextureNormalTangent>* vertices) const;

		uint32_t RetrieveGeometryIndexOffset() const { return m_GeometryIndexOffset; }
		uint32_t RetrieveGeometryIndexCount() const { return m_GeometryIndexCount; }
		uint32_t RetrieveGeometryVertexOffset() const { return m_GeometryVertexOffset; }
		uint32_t RetrieveGeometryVertexCount() const { return m_GeometryVertexCount; }
		GeometryType RetrieveGeometryType() const { return m_GeometryType; }
		const std::string& RetrieveGeometryName() const { return m_GeometryName; }
		Model* RetrieveGeometryModel() const { return m_Model; }
		const Math::BoundingBox& RetrieveBoundingBox() const { return m_BoundingBox; }
		const Math::BoundingBox& RetrieveAABB();

		// Material
		/// std::shared_ptr<Material> SetMaterial(const std::shared_ptr<Material>& material); // Sets a material from memory (adds it to the resource cache by default).
		
		// Loads a material and sets it.
		/// std::shared_ptr<Material> SetMaterial(const std::string& filePath);

		/// void UseDefaultMaterial();
		/// std::string RetrieveMaterialName() const;
		/// Material* RetrieveMaterial() const { return m_Material; }
		/// bool HasMaterial() const { return m_Material != nullptr; }

	private:
		std::string m_GeometryName;
		uint32_t m_GeometryIndexOffset;
		uint32_t m_GeometryIndexCount;
		uint32_t m_GeometryVertexOffset;
		uint32_t m_GeometryVertexCount;

		GeometryType m_GeometryType;
		Math::BoundingBox m_BoundingBox;
		Math::BoundingBox m_AABB;
		Math::Matrix m_LastTransform = Math::Matrix::Identity;
		
		bool m_IsShadowCasting = true;
		bool m_IsDefaultMaterial;

		Model* m_Model = nullptr;
		/// Material* m_Material = nullptr;
	};
}