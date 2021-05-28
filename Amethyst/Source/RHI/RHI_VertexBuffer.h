#pragma once
#include <vector>
#include "RHI_Device.h"
#include "../Core/AmethystObject.h"

namespace Amethyst
{
	class RHI_VertexBuffer : public AmethystObject
	{
	public:
		RHI_VertexBuffer(const std::shared_ptr<RHI_Device>& rhi_Device, const uint32_t stride = 0)
		{
			m_RHI_Device = rhi_Device;
			m_Stride = stride;
		}

		~RHI_VertexBuffer()
		{
			_Destroy();
		}

		template<typename T>
		bool Create(const std::vector<T>& vertices)
		{
			m_Stride = static_cast<uint32_t>(sizeof(T));
			m_VertexCount = static_cast<uint32_t>(vertices.size());
			m_Size_GPU = static_cast<uint64_t>(m_Stride * m_VertexCount);

			return _Create(static_cast<const void*>(vertices.data()));
		}

		template<typename T>
		bool Create(const T* vertices, const uint32_t vertexCount)
		{
			m_Stride = static_cast<uint32_t>(sizeof(T));
			m_VertexCount = vertexCount;
			m_Size_GPU = static_cast<uint64_t>(m_Stride * m_VertexCount);

			return _Create(static_cast<const void*>(vertices));
		}

		template<typename T>
		bool CreateDynamic(const uint64_t vertexCount)
		{
			m_Stride = static_cast<uint32_t>(sizeof(T));
			m_VertexCount = vertexCount;
			m_Size_GPU = static_cast<uint64_t>(m_Stride * m_VertexCount);

			return _Create(nullptr);
		}

		void* Map();
		bool Unmap();

		void* RetrieveResource() const { return m_Buffer; }
		uint32_t RetrieveStride() const { return m_Stride; }
		uint32_t RetrieveVertexCount() const { return m_VertexCount; }

	private:
		bool _Create(const void* vertices);
		void _Destroy();

	private:
		bool m_PersistentMapping = true; //Only affects Vulkan.
		void* m_Mapped = nullptr;
		uint32_t m_Stride = 0;
		uint32_t m_VertexCount = 0;

		//API
		std::shared_ptr<RHI_Device> m_RHI_Device;
		void* m_Buffer = nullptr; // Data buffer.
		void* m_Allocation = nullptr; // Memory allocation.
		bool m_IsMappable = true;
	};
}
