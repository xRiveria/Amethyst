#include "Amethyst.h"
#include "Editor.h"
#include "Source/Core/Engine.h"
#include "Source/Core/Window.h"
#include "Source/Core/Context.h"
#include "Source/Event/EventSystem.h"
#include "SDL/SDL.h"

#include "Utilities/EditorExtensions.h"
#include "ImGui/Implementation/RHI_ImGui.h"
#include "ImGui/Implementation/imgui_impl_sdl.h"
#include "Utilities/IconLibrary.h"

#include "Widgets/AssetBrowser.h"
#include "Widgets/Console.h"
#include "Widgets/MenuBar.h"
#include "Widgets/ProgressDialog.h"
#include "Widgets/Viewport.h"
#include "Widgets/Hierarchy.h"
#include "Widgets/ObjectsPanel.h"
#include "Widgets/Toolbar.h"
#include "Widgets/QuickDiagnostics.h"

namespace EditorConfigurations
{
	const float g_FontSize = 17.0f;
}

namespace EditorGlobals
{
	Widget* g_Widget_MenuBar = nullptr;
	Widget* g_Widget_World = nullptr;

	Amethyst::Renderer* g_Renderer = nullptr;
	Amethyst::RHI_SwapChain* g_Swapchain = nullptr;
	std::shared_ptr<Amethyst::RHI_Device> g_RHI_Device = nullptr;
	Amethyst::Window* g_Window = nullptr;
	/// Profiler
}

Editor::Editor()
{
	// Create Engine
	m_Engine = std::make_unique<Amethyst::Engine>();

	// Acquire useful engine subsystems.
	m_EngineContext = m_Engine->RetrieveContext();

	/// Profiler
	EditorGlobals::g_Renderer = m_EngineContext->RetrieveSubsystem<Amethyst::Renderer>();
	EditorGlobals::g_Window = m_EngineContext->RetrieveSubsystem<Amethyst::Window>();
	EditorGlobals::g_RHI_Device = EditorGlobals::g_Renderer->RetrieveRHIDevice();
	EditorGlobals::g_Swapchain = EditorGlobals::g_Renderer->RetrieveSwapChain();

	// Initialize Editor/ImGui
	if (EditorGlobals::g_Renderer->IsInitialized()) // Ensure that our renderer has been initialized.
	{
		InitializeEditor();
	}
	else
	{
		AMETHYST_ERROR("Editor failed to initialize. A renderer is required but it has also failed to initialize.");
	}

	// Allow ImGui Get Events from the Engine's event processing loop.
	SUBSCRIBE_TO_EVENT(EventType::EventSDL, EVENT_HANDLER_VARIANT_STATIC(ImGuiImplementation_ProcessEvent));
}

Editor::~Editor()
{
	ImGuiImplementation_Shutdown();
}

void Editor::OnUpdate()
{
	while (!EditorGlobals::g_Window->WantsToClose()) // As long as our window remains open...
	{
		// Engine - Tick
		m_Engine->OnUpdate(); 

		if (!EditorGlobals::g_Renderer || !EditorGlobals::g_Renderer->IsInitialized()) // If our renderer does not exist or if it isn't initialized...
		{
			continue; // Continue to tick the engine, but make the Editor go into reloop.
		}

		if (EditorGlobals::g_Window->IsFullScreen())
		{
			// Pass copy to backbuffer.
			EditorGlobals::g_Renderer->Pass_CopyToBackbuffer(EditorGlobals::g_Swapchain->RetrieveCommandList());
		}
		else
		{
			// ImGui - Begin
			ImGui_ImplSDL2_NewFrame(m_EngineContext);
			ImGui::NewFrame();

			// Editor - Begin
			BeginEditorWindow(); // This contains a ImGui::Begin for our Editor and the start of a docking context. 

			// Editor - Tick
			for (std::shared_ptr<Widget>& widget : m_Widgets) // Tick each individual widget. Each widgets contains its own ImGui::Begin and ImGui::End behavior (based on visibility/constantness).
			{
				widget->OnUpdate();
			}

			// Editor - End
			if (m_EditorBegun)
			{
				ImGui::End(); // Ends our Editor window and docking context.
			}

			// ImGui - End/Render
			ImGui::Render();
			ImGui::RHI::Render(ImGui::GetDrawData());
		}

		// Present
		EditorGlobals::g_Renderer->Present();

		// ImGui - Child Windows
		if (!EditorGlobals::g_Window->IsFullScreen() && ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
		}
	}
}

void Editor::InitializeEditor()
{
	ImGuiImplementation_Initialize(m_EngineContext);
	ImGuiImplementation_ApplyStyling();

	// Initialization of misc custom systems.
	IconLibrary::RetrieveIconLibraryInstance().Initialize(m_EngineContext);	  // Editor Assets (Icons).
	EditorHelper::RetrieveEditorHelperInstance().Initialize(m_EngineContext); // For Model Loading, Scene Loading/Saving etc.

	// Create all ImGui widgets.
	m_Widgets.emplace_back(std::make_shared<Console>(this));
	m_Widgets.emplace_back(std::make_shared<MenuBar>(this));
	EditorGlobals::g_Widget_MenuBar = static_cast<MenuBar*>(m_Widgets.back().get());
	m_Widgets.emplace_back(std::make_shared<Viewport>(this));
	m_Widgets.emplace_back(std::make_shared<AssetBrowser>(this));
	m_Widgets.emplace_back(std::make_shared<Hierarchy>(this));
	EditorGlobals::g_Widget_World = static_cast<Hierarchy*>(m_Widgets.back().get());
	m_Widgets.emplace_back(std::make_shared<ObjectsPanel>(this));
	m_Widgets.emplace_back(std::make_shared<Toolbar>(this));
	m_Widgets.emplace_back(std::make_shared<ProgressDialog>(this));
	m_Widgets.emplace_back(std::make_shared<QuickDiagnostics>(this));
}

void Editor::BeginEditorWindow()
{
	// Set Main Editor Window Flags.
	const int windowFlags =
		ImGuiWindowFlags_MenuBar					|
		ImGuiWindowFlags_NoDocking					|
		ImGuiWindowFlags_NoTitleBar					|
		ImGuiWindowFlags_NoCollapse					|
		ImGuiWindowFlags_NoResize					|
		ImGuiWindowFlags_NoMove						|
		ImGuiWindowFlags_NoBringToFrontOnFocus	    |
		ImGuiWindowFlags_NoNavFocus;

	// Set Window Position and Size (if we wish to push things down for our toolbar).
	/// 

	// Set Window Style
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
	ImGui::SetNextWindowBgAlpha(0.0f);

	// Begin Window
	std::string windowName = "##Main_Window";
	bool isMainWindowOpen = true;
	m_EditorBegun = ImGui::Begin(windowName.c_str(), &isMainWindowOpen, windowFlags);
	ImGui::PopStyleVar(3);

	// Begin Dock Space
	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_DockingEnable && m_EditorBegun)
	{
		// Dock Space
		const ImGuiID windowID = ImGui::GetID(windowName.c_str()); // Retrieve our window.
		if (!ImGui::DockBuilderGetNode(windowID))
		{
			// Reset current docking state.
			ImGui::DockBuilderRemoveNode(windowID);
			ImGui::DockBuilderAddNode(windowID, ImGuiDockNodeFlags_None);
			ImGui::DockBuilderSetNodeSize(windowID, ImGui::GetMainViewport()->Size);

			ImGuiID dockMainID = windowID;
			ImGuiID dockRightID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Right, 0.2f, nullptr, &dockMainID);
			const ImGuiID dockRightDownID = ImGui::DockBuilderSplitNode(dockRightID, ImGuiDir_Down, 0.6f, nullptr, &dockRightID);
			ImGuiID dockDownID = ImGui::DockBuilderSplitNode(dockMainID, ImGuiDir_Down, 0.25f, nullptr, &dockMainID);
			const ImGuiID dockDownRightID = ImGui::DockBuilderSplitNode(dockDownID, ImGuiDir_Right, 0.6f, nullptr, &dockDownID);

			// Dock Windows
			ImGui::DockBuilderDockWindow("Hierarchy", dockRightID);
			ImGui::DockBuilderDockWindow("Properties", dockRightDownID);
			ImGui::DockBuilderDockWindow("Console", dockDownID);
			ImGui::DockBuilderDockWindow("Assets", dockDownRightID);
			ImGui::DockBuilderDockWindow("Viewport", dockMainID);

			ImGui::DockBuilderFinish(dockMainID);
		}

		ImGui::PushStyleVar(ImGuiStyleVar_FrameBorderSize, 0.0f);
		ImGui::DockSpace(windowID, ImVec2(0.0f, 0.0f), ImGuiDockNodeFlags_PassthruCentralNode);
		ImGui::PopStyleVar();
	}
}

void Editor::ImGuiImplementation_Initialize(Amethyst::Context* context)
{
	// Version Validation
	IMGUI_CHECKVERSION();
	// Register ImGui Library.

	// Context Creation
	ImGui::CreateContext();

	// Configuration
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
	io.ConfigWindowsResizeFromEdges = true;
	io.ConfigViewportsNoTaskBarIcon = true;

	// Font 
	const std::string fontDirectory = context->RetrieveSubsystem<Amethyst::ResourceCache>()->RetrieveResourceDirectory(Amethyst::ResourceDirectory::Fonts) + "\\";
	io.Fonts->AddFontFromFileTTF((fontDirectory + "opensans/OpenSans-Bold.ttf").c_str(), EditorConfigurations::g_FontSize);

	// Initialize SDL (Windows, Input) and RHI (Rendering)
	ImGui_ImplSDL2_Init(static_cast<SDL_Window*>(EditorGlobals::g_Window->RetrieveSDLHandle())); 
	ImGui::RHI::Initialize(context);
}

void Editor::ImGuiImplementation_ProcessEvent(const Amethyst::Variant& eventVariant)
{
	SDL_Event* sdlEvent = eventVariant.RetrieveValue<SDL_Event*>();
	ImGui_ImplSDL2_ProcessEvent(sdlEvent);
}

void Editor::ImGuiImplementation_Shutdown()
{
	if (ImGui::GetCurrentContext())
	{
		ImGui::RHI::Shutdown();
		ImGui_ImplSDL2_Shutdown();
		ImGui::DestroyContext();
	}
}

void Editor::ImGuiImplementation_ApplyStyling()
{
	// Apply Dark Style
	ImGui::StyleColorsDark();
	ImVec4* colors = ImGui::GetStyle().Colors;

	// Window Background
	colors[ImGuiCol_WindowBg] = ImVec4{ 0.1f, 0.105f, 0.11f, 1.0f };

	// Headers
	colors[ImGuiCol_Header] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_HeaderHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_HeaderActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Buttons
	colors[ImGuiCol_Button] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_ButtonHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_ButtonActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Frame BG
	colors[ImGuiCol_FrameBg] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };
	colors[ImGuiCol_FrameBgHovered] = ImVec4{ 0.3f, 0.305f, 0.31f, 1.0f };
	colors[ImGuiCol_FrameBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };

	// Tabs
	colors[ImGuiCol_Tab] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabHovered] = ImVec4{ 0.38f, 0.3805f, 0.381f, 1.0f };
	colors[ImGuiCol_TabActive] = ImVec4{ 0.28f, 0.2805f, 0.281f, 1.0f };
	colors[ImGuiCol_TabUnfocused] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TabUnfocusedActive] = ImVec4{ 0.2f, 0.205f, 0.21f, 1.0f };

	// Title
	colors[ImGuiCol_TitleBg] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgActive] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
	colors[ImGuiCol_TitleBgCollapsed] = ImVec4{ 0.15f, 0.1505f, 0.151f, 1.0f };
}