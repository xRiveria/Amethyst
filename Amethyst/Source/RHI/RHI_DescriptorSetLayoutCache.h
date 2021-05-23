#pragma once
#include "../Core/AmethystObject.h"
#include "RHI_Descriptor.h"
#include "RHI_PipelineState.h"
#include <atomic>
#include <unordered_map>

namespace Amethyst
{
	class RHI_DescriptorSetLayoutCache : public AmethystObject
	{
	public:
		RHI_DescriptorSetLayoutCache(const RHI_Device* rhi_Device);
		~RHI_DescriptorSetLayoutCache();

		void SetPipelineState(RHI_PipelineState& pipelineState);
		void Reset(uint32_t descriptorSetCapacity = 0);

		//Descriptor resource updating.
		bool SetConstantBuffer(const uint32_t slot, RHI_ConstantBuffer* constantBuffer);
		void SetSampler(const uint32_t slot, RHI_Sampler* sampler);
		void SetTexture(const uint32_t slot, RHI_Texture* texture, const bool storage);

		RHI_DescriptorSetLayout* RetrieveCurrentDescriptorSetLayout() const {  }
		bool RetrieveDescriptorSet(RHI_DescriptorSet*& descriptorSet);
		void* RetrieveDescriptorPool() const { return m_DescriptorPool; }

		//Capacity
		bool HasEnoughCapacity() const { return m_DescriptorSetCapacity > RetrieveDescriptorSetCount(); }
		void GrowIfNeeded();

	private:
		uint32_t RetrieveDescriptorSetCount() const;
		void SetDescriptorSetCapacity(uint32_t descriptorSetCapacity);
		bool CreateDescriptorPool(uint32_t descriptorSetCapacity);
		void RetrieveDescriptors(RHI_PipelineState& pipelineState, std::vector<RHI_Descriptor>& descriptors);

	private:
		/*	
			A descriptor is a special opaque variable that shaders use to access buffer and image resources in an indirect fashion. It can be thought of a pointer to a resource.
			- Refer to: https://vulkan.lunarg.com/doc/view/1.2.170.0/linux/tutorial/html/08-init_pipeline_layout.html
			A descriptor set is called a "set" because it can refer to an array of homegenous resources that can be described with the same layout binding.
			
			One possible way to use multiple descriptors is to construct a descriptor set with two descriptors, with each descriptor pointing to a different texture.
			Both textures are therefore avaliable during a draw. A command in a command buffer could then select the texture to use by specifying the index of the desired 
			texture. 

			A descriptor set is "described" using a descriptor set layout - used to describe the content of a list of descriptor sets. 
		*/

		//Descriptor Set Layouts
		std::unordered_map<std::size_t, std::shared_ptr<RHI_DescriptorSetLayout>> m_DescriptorSetLayouts;
		RHI_DescriptorSetLayout* m_DescriptorSetLayoutCurrent = nullptr;
		std::vector<RHI_Descriptor> m_Descriptors;
		
		//Descriptor Pool
		void* m_DescriptorPool = nullptr;
		uint32_t m_DescriptorSetCapacity = 16;

		//Misc
		std::atomic<bool> m_AreDescriptorSetLayoutsBeingCleared = false;
		const RHI_Device* m_RHI_Device;
	};
}