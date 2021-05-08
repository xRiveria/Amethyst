#pragma once
#include "../Backend/WindowManager.h"
#include <utility>
#include "Widget.h"

namespace Amethyst
{
	class Context;

	class Editor
	{
	public:
		Editor() {}
		~Editor();

		void InitializeEditor(std::pair<GLFWwindow*, WindowProperties> windowContext);
		void BeginEditorRenderLoop();
		void RenderDockingContext(); //Create all your user menus below after calling this function.
		void EndEditorRenderLoop();

		Context* RetrieveContext() { return m_Context; }

	private:
		void SetupEditorStyling();

	private:
		std::pair<GLFWwindow*, WindowProperties> m_WindowContext;
		std::vector<std::shared_ptr<Widget>> m_Widgets;
		Context* m_Context = nullptr;
	};
}