#pragma once
#include "../Widget.h"

namespace Amethyst
{
	class Viewport : public Widget
	{
		class World;

	public:
		Viewport(Editor* editor);

		void OnVisibleTick() override;

	private:
		float m_ViewportWidth = 0.0f;
		float m_ViewportHeight = 0.0f;
		Math::Vector2 m_ViewportOffset = Math::Vector2::Zero;
		float m_WindowPadding = 4.0f;

		bool m_IsResolutionDirty = true; //Signifies that our viewport needs to be updated.

		World* m_World = nullptr;
		
	};
}