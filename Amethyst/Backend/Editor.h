#pragma once
#include "WindowManager.h"
#include <utility>

namespace Amethyst
{
	class Editor
	{
	public:
		Editor() {}
		~Editor();

		void InitializeEditor(std::pair<GLFWwindow*, WindowProperties> windowContext);
		void BeginEditorRenderLoop();
		void RenderDockingContext(); //Create all your user menus below after calling this function.
		void EndEditorRenderLoop();

	private:
		void SetupEditorStyling();

	private:
		std::pair<GLFWwindow*, WindowProperties> m_WindowContext;
	};
}