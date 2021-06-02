#include "Amethyst.h"
#include "RHI_CommandList.h"
#include "RHI_Fence.h"
#include "RHI_DescriptorSetLayoutCache.h"
#include "RHI_Device.h"

namespace Amethyst
{
	bool RHI_CommandList::Wait()
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

	bool RHI_CommandList::Flush()
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

			if (!End()) //End recording.
			{
				return false;
			}
		}

		if (m_CommandListState == RHI_CommandListState::Ended) //If ended...
		{
			if (!Submit()) //Submit.
			{
				return false;
			}
		}

		//Flush
		Wait(); 

		//If idle, restore state (if any).
		if (m_CommandListState == RHI_CommandListState::Idle)
		{
			if (wasRecording) //If we were recording a state before ending...
			{
				if (!Begin()) //Begin recording...
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

		m_Flushed = true; //Flush complete.

		return true;
	}
}