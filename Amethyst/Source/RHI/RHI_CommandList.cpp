#include "Amethyst.h"
#include "RHI_CommandList.h"
#include "RHI_Fence.h"
#include "RHI_DescriptorSetLayoutCache.h"
#include "RHI_Device.h"

namespace Amethyst
{
	bool RHI_CommandList::WaitCommandBuffer()
	{
		AMETHYST_ASSERT(m_CommandListState == RHI_CommandListState::Submitted);

		if (!m_ProcessedFence->Wait()) //If our fence isn't in waiting state...
		{
			return false;
		}

		m_DescriptorSetLayoutCache->GrowIfNeeded();
		m_CommandListState = RHI_CommandListState::Idle;

		return true;
	}

	bool RHI_CommandList::FlushCommandBuffer()
	{
		if (m_CommandListState == RHI_CommandListState::Idle) //If nothing's going on, return.
		{
			return true;
		}

		//If recording, end it.
		bool wasRecording = false;
		bool hadRenderPass = false;

		if (m_CommandListState == RHI_CommandListState::Recording) //If we are recording...
		{
			wasRecording = true; //Save it for later.
			
			if (m_IsRenderPassActive) //If we have a active render pass...
			{
				hadRenderPass = true; //Save it for later.

				if (!EndRenderPass()) //End render passes.
				{
					return false;
				}
			}

			if (!EndCommandBuffer()) //End recording.
			{
				return false;
			}
		}

		if (m_CommandListState == RHI_CommandListState::Ended) //If ended...
		{
			if (!SubmitCommandBuffer()) //Submit.
			{
				return false;
			}
		}

		//Flush
		WaitCommandBuffer(); 

		//If idle, restore state (if any).
		if (m_CommandListState == RHI_CommandListState::Idle)
		{
			if (wasRecording) //If we were recording a state before ending...
			{
				if (!BeginCommandBuffer()) //Begin recording...
				{
					return false;
				}

				if (hadRenderPass) 
				{
					if (!BeginRenderPass(*m_PipelineState))
					{
						return false;
					}
				}
			}
		}

		m_IsCommandBufferFlushed = true; //Flush complete.

		return true;
	}
}