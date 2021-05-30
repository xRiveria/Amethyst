#pragma once
#include <vector>
#include "../Core/AmethystObject.h"
#include "RHI_Device.h"

namespace Amethyst
{
	class RHI_IndexBuffer : public AmethystObject
	{
	public:
		RHI_IndexBuffer(const std::shared_ptr<RHI_Device>& rhi_Device)
		{
			m_RHI_Device = rhi_Device;
		}

		~RHI_IndexBuffer()
		{
			_Destroy();
		}

		template<typename T>
		bool Create(const std::vector<T>& indices)
		{
			m_Stride = sizeof(T);
			m_IndexCount = static_cast<uint32_t>(indices.size());
			m_Size_GPU = static_cast<uint64_t>(m_Stride * m_IndexCount);

			return _Create(static_cast<const void*>(indices.data()));
		}

		template<typename T>
		bool Create(const T* indices, const uint32_t indexCount)
		{
			m_Stride = sizeof(T);
			m_IndexCount = indexCount;
			m_Size_GPU = static_cast<uint64_t>(m_Stride * m_IndexCount);

			return _Create(static_cast<const void*>(indices));
		}

		template<typename T>
		bool CreateDynamic(const uint32_t indexCount)
		{
			m_Stride = sizeof(T);
			m_IndexCount = indexCount;
			m_Size_GPU = static_cast<uint64_t>(m_Stride * m_IndexCount);

			return _Create(nullptr);
		}

		void* Map();
		bool Unmap();

		void* RetrieveResource() const { return m_Buffer; }
		uint32_t RetrieveIndexCount() const { return m_IndexCount; }
		bool Is16Bit() const { return sizeof(uint16_t) == m_Stride; }
		bool Is32Bit() const { return sizeof(uint32_t) == m_Stride; }


	protected:
		bool _Create(const void* indices);
		void _Destroy();

	protected:
		bool m_PersistentMapping = true; //Only affects Vulkan.
		uint32_t m_Stride = 0;
		uint32_t m_IndexCount = 0;

		// Memory
		void* m_MappedMemory = nullptr; // Pointer to our data on the GPU if mapped.
		void* m_Buffer = nullptr; // Data buffer.
		void* m_BufferAllocation = nullptr; // Buffer memory allocation via VMA.
		bool m_IsMappable = true;

		//API
		std::shared_ptr<RHI_Device> m_RHI_Device;
	};
}