#pragma once
#include <memory>
#include <vector>
#include "../RHI/RHI_Utilities.h"
#include "../Resource/IResource.h"
#include "../Runtime/Math/BoundingBox.h"

namespace Amethyst
{
	class Model : public IResource, public std::enable_shared_from_this<Model>
	{
	public:
		Model(Context* engineContext);

		// Geometry
		void AppendGeometry(const std::vector<uint32_t>& indices, 
						    const std::vector<RHI_Vertex_PositionTextureNormalTangent>& vertices, 
							uint32_t* indexOffset = nullptr, 
							uint32_t* vertexOffset = nullptr) const;

		void UpdateGeometry();
		void RetrieveGeometry(uint32_t indexOffset, uint32_t indexCount, uint32_t vertexOffset, uint32_t vertexCount, std::vector<uint32_t>* indices, std::vector<RHI_Vertex_PositionTextureNormalTangent>* vertices);
	};
}