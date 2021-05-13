#pragma once
#include <unordered_map>
#include <array>
#include <atomic>
#include "../Core/ISubsystem.h"

namespace Amethyst
{
	class Renderer : public ISubsystem
	{
	public:
		Renderer(Context* context);
		~Renderer();

		bool InitializeSubsystem() override;
		void OnUpdate(float deltaTime) override;

		//Primitive Rendering



	private:
		//RHI Core
		std::shared_ptr<RHI_Device> m_RHI_Device;
		//std::shared_ptr<RHI_PipelineCache> m_RHI_PipelineCache;
	};
}