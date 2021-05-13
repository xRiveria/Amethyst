#include "Amethyst.h"
#include "Renderer.h"
#include "../Runtime/ECS/Entity.h"
#include "../RHI/RHI_Device.h"

namespace Amethyst
{
	Renderer::Renderer(Context* context) : ISubsystem(context)
	{

	}

	Renderer::~Renderer()
	{
	}

	bool Renderer::InitializeSubsystem()
	{
		//Retrieve required systems.

		//Resolution, viewport andd swapchain default to whatever the window size is.

		//Set resolution.

		//Set viewport.

		//Create Device.
		m_RHI_Device = std::make_shared<RHI_Device>(m_Context);
		if (!m_RHI_Device->IsInitialized())
		{
			AMETHYST_INFO("Failed to create RHI Device.");
			return false;
		}

		//Create Pipeline Cache
		//Create Descriptor Set Layout Cache.

		//Create Swapchain


		return false;
	}

	void Renderer::OnUpdate(float deltaTime)
	{
	}
}