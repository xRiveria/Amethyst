#pragma once
#include <memory>
#include "RHI_Utilities.h"
#include "../Core/AmethystObject.h"

namespace Amethyst
{
	//In Direct3D, a constant buffer can be used to send a bunch of data to the GPU, provided their types, strides and offsets are properly defined.
	class RHI_ConstantBuffer : public AmethystObject  //Constant buffers (HLSL) are literally uniform buffers in GLSL. 
	{
	public:
		RHI_ConstantBuffer(const std::shared_ptr<RHI_Device>& rhi_Device, const std::string& name, bool isDynamic = false);
		~RHI_ConstantBuffer() { _Destroy(); }

		template<typename T>
		bool Create(const uint32_t offsetCount = 1)
		{
			m_Stride = static_cast<uint32_t>(sizeof(T));
			m_OffsetCount = offsetCount;
			m_Size_GPU = static_cast<uint64_t>(m_Stride * m_OffsetCount);

			return _Create();
		}
		
		void* Map();
		void Unmap(const uint64_t offset = 0, const uint64_t size = 0);

		void* RetrieveResource() const { return m_Buffer; }
		uint32_t RetrieveStride() const { return m_Stride; }
		uint32_t RetrieveOffsetCount() const { return m_OffsetCount; }

		//Static Offset - The kind of offset that is used when updating the buffer.
		uint32_t RetrieveOffset() const { return m_OffsetIndex * m_Stride; }
		uint32_t RetrieveOffsetIndex() const { return m_OffsetIndex; }
		void SetOffsetIndex(const uint32_t offsetIndex) { m_OffsetIndex = offsetIndex; }

		//Dynamic Offset - The kind of offset that is used when binding descriptor sets.
		bool IsDynamic() const { return m_IsDynamic; }
		uint32_t RetrieveOffsetDynamic() const { return m_OffsetDynamicIndex * m_Stride; }
		uint32_t RetrieveOffsetIndexDynamic() const { return m_OffsetDynamicIndex; }
		void SetOffsetIndexDynamic(const uint32_t offsetIndex) { m_OffsetDynamicIndex = offsetIndex; }

	private:
		bool _Create();
		void _Destroy();

	private:
		bool m_IsDynamic = false; //Only affects Vulkan.
		bool m_PersistentMapping = true; //Only affects Vulkan. Saves 2ms of CPU time.
		void* m_Mapped = nullptr;

		uint32_t m_Stride = 0;
		uint32_t m_OffsetCount = 1;
		uint32_t m_OffsetIndex = 0;
		uint32_t m_OffsetDynamicIndex = 0;

		//API
		void* m_Buffer = nullptr;
		void* m_Allocation = nullptr;

		//Dependencies
		std::shared_ptr<RHI_Device> m_RHI_Device;
	};
}