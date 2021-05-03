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
		float m_Width = 0.0f;
		float m_Height = 0.0f;
		float m_WindowPadding = 4.0f;
		bool m_IsResolutionDirty = true;

		World* m_World = nullptr;
	};
}