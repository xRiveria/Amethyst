#pragma once
#include <utility>
#include "Source/Core/Variant.h"
#include "../RHI/RHI_Utilities.h"
#include "../Core/Engine.h"
#include "Widget.h"

// Forward Declarations
namespace Amethyst
{
	class Window;
	class Engine;
	class Context;
	class Renderer;
}

class Editor
{
public:
	Editor();
	~Editor();

	void OnUpdate();

	template<typename T>
	T* RetrieveWidget()
	{
		for (const Widget& widget : m_Widgets)
		{
			if (T* widget_t = dynamic_cast<T*>(widget.get()))
			{
				return widget_t;
			}
		}

		return nullptr;
	}

	Amethyst::Context* RetrieveEngineContext() { return m_EngineContext; }

private:
	void InitializeEditor();
	void BeginEditorWindow();

	static void ImGuiImplementation_Initialize(Amethyst::Context* context);
	static void ImGuiImplementation_Shutdown();
	static void ImGuiImplementation_ApplyStyling();
	static void ImGuiImplementation_ProcessEvent(const Amethyst::Variant& eventVariant);

private:
	// Engine Contexts
	std::unique_ptr<Amethyst::Engine> m_Engine;
	Amethyst::Context* m_EngineContext = nullptr; // Consists of subsystems.

	// Editor Contexts
	bool m_EditorBegun = false;
	std::vector<std::shared_ptr<Widget>> m_Widgets;
};
