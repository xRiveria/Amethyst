#pragma once
#include <unordered_map>
#include <array>
#include <atomic>
#include "../Core/ISubsystem.h"
#include "../RHI/RHI_Viewport.h"

namespace Amethyst
{
	class Renderer : public ISubsystem
	{
	public:
		Renderer(Context* context);
		~Renderer();

		bool InitializeSubsystem() override;
		void OnUpdate(float deltaTime) override;

		//Viewport
		const RHI_Viewport& RetrieveViewport() const { return m_Viewport; }
		void SetViewport(float width, float height);

		//Primitive Rendering

		const std::shared_ptr<RHI_Device>& RetrieveRHIDevice() const { return m_RHI_Device; }

	private:
		//RHI Core
		std::shared_ptr<RHI_Device> m_RHI_Device;


		//Resolution & Viewport
		RHI_Viewport m_Viewport = RHI_Viewport(0, 0, 0, 0);
		 
		//std::shared_ptr<RHI_PipelineCache> m_RHI_PipelineCache;
	};
}