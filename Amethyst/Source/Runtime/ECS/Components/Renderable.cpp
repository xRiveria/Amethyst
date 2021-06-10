#include "Amethyst.h"
#include "Renderable.h"
#include "Transform.h"
#include "../Rendering/Model.h"
#include "../Utilities/Geometry.h"
#include "../Resource/ResourceCache.h"

namespace Amethyst
{
	inline void BuildGeometry(const GeometryType type, Renderable* renderable)
	{
		Model* model = new Model(renderable->RetrieveEngineContext());
		std::vector<RHI_Vertex_PositionTextureNormalTangent> vertices;
		std::vector<uint32_t> indices;

		const std::string projectDirectory = renderable->RetrieveEngineContext()->RetrieveSubsystem<ResourceCache>()->RetrieveProjectDirectory();

		// Construct Geometry
		if (type == GeometryType::Geometry_Default_Cube)
		{
			Utility::Geometry::CreateCube(&vertices, &indices);
			model->SetResourceFilePath(projectDirectory + "Default_Cube" + Extension_Model);
		}
		else if (type == GeometryType::Geometry_Default_Quad)
		{
			Utility::Geometry::CreateQuad(&vertices, &indices);
			model->SetResourceFilePath(projectDirectory + "Default_Quad" + Extension_Model);
		}

		if (vertices.empty() || indices.empty())
		{
			return;
		}

		model->AppendGeometry(indices, vertices, nullptr, nullptr);
		model->UpdateGeometry();

		renderable->GeometrySet(
			"Default_Geometry",
			0,
			static_cast<uint32_t>(indices.size()),
			0,
			static_cast<uint32_t>(vertices.size()),
			Math::BoundingBox(vertices.data(), static_cast<uint32_t>(vertices.size())),
			model
		);
	}

	Renderable::Renderable(Context* engineContext, Entity* entity, uint32_t componentID /*= 0*/) : IComponent(engineContext, entity, componentID)
	{
		m_GeometryType = GeometryType::Geometry_Custom;
		m_GeometryIndexOffset = 0;
		m_GeometryIndexCount = 0;
		m_GeometryVertexOffset = 0;
		m_GeometryVertexCount = 0;
		m_IsDefaultMaterial = false;
		m_IsShadowCasting = true;

		REGISTER_ATTRIBUTE_VALUE_VALUE(m_IsDefaultMaterial, bool);
		/// REGISTER_ATTRIBUTE_VALUE_VALUE(m_Material, Material*);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_IsShadowCasting, bool);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_GeometryIndexOffset, uint32_t);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_GeometryIndexCount, uint32_t);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_GeometryVertexOffset, uint32_t);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_GeometryVertexCount, uint32_t);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_GeometryName, std::string);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_Model, Model*);
		REGISTER_ATTRIBUTE_VALUE_VALUE(m_BoundingBox, Math::BoundingBox);
		REGISTER_ATTRIBUTE_GET_SET(GeometryType, GeometrySet, GeometryType);
	}

	void Renderable::GeometrySet(const std::string& geometryName, uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset, uint32_t vertexCount, const Math::BoundingBox& boundingBox, Model* model)
	{
		// Terrible way to delete previous geometry in case its the default one.
		if (m_GeometryName == "Default_Geometry")
		{
			delete m_Model;
		}

		m_GeometryName = geometryName;
		m_GeometryIndexOffset = indexOffset;
		m_GeometryIndexCount = indexCount;
		m_GeometryVertexOffset = vertexOffset;
		m_GeometryVertexCount = vertexCount;
		m_BoundingBox = boundingBox;
		m_Model = model;
	}

	void Renderable::GeometrySet(const GeometryType type)
	{
		m_GeometryType = type;

		if (type != Geometry_Custom)
		{
			BuildGeometry(type, this);
		}
	}

	void Renderable::GeometryClear()
	{
		GeometrySet("Cleared", 0, 0, 0, 0, Math::BoundingBox(), nullptr);
	}

	void Renderable::GeometryRetrieve(std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PositionTextureNormalTangent>* vertices) const
	{
		if (!m_Model)
		{
			AMETHYST_ERROR("Invalid Model.");
			return;
		}

		m_Model->RetrieveGeometry(m_GeometryIndexOffset, m_GeometryIndexCount, m_GeometryVertexOffset, m_GeometryVertexCount, indices, vertices);
	}

	const Math::BoundingBox& Renderable::RetrieveAABB()
	{
		// Updated if dirty. Our AABB will always follow the transform of the entity.
		if (m_LastTransform != RetrieveTransform()->RetrieveMatrix() || !m_AABB.IsDefined())
		{
			m_AABB = m_BoundingBox.Transform(RetrieveTransform()->RetrieveMatrix());
			m_LastTransform = RetrieveTransform()->RetrieveMatrix();
		}

		return m_AABB;
	}
}