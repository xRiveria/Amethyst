#include "Editor.h"
#include "Source/Core/Engine.h"
#include "Source/Core/Window.h"
#include "Source/Core/Context.h"
#include "Source/Event/EventSystem.h"
#include "SDL/SDL.h"
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
	SUBSCRIBE_TO_EVENT(Amethyst::EventType::EventSDL, EVENT_HANDLER_VARIANT_STATIC(ImGuiImplementation_ProcessEvent));
}

Editor::~Editor()
{
	ImGuiImplementation_Shutdown();
}

void Editor::InitializeEditor()
{
	ImGuiImplementation_Initialize(m_EngineContext);
	ImGuiImplementation_ApplyStyling();

	// Initialization of misc custom systems.
	IconLibrary::RetrieveIconLibraryInstance().InitializeIconLibrary(m_EngineContext);
	EditorHelper::RetrieveEditorHelperInstance().InitializeEditorHelper(m_EngineContext); ///

	// Create all ImGui widgets.
	m_Widgets.emplace_back(std::make_shared<Console>(this));
	m_Widgets.emplace_back(std::make_shared<MenuBar>(this));
	EditorGlobals::g_Widget_MenuBar = static_cast<MenuBar*>(m_Widgets.back().get());
	m_Widgets.emplace_back(std::make_shared<AssetBrowser>(this));
}

void Editor::BeginEditorWindow()
{

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

	// Font - Create Font Library?
	io.Fonts->AddFontFromFileTTF("Resources/Fonts/opensans/OpenSans-Bold.ttf", EditorConfigurations::g_FontSize);

	// Initialize SDL (Windows, Input) and RHI (Rendering)
	ImGui_ImplSDL2_Init(static_cast<SDL_Window*>(EditorGlobals::g_Window->RetrieveSDLHandle())); ///
	ImGui::RHI::Initialize(context); ///
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


	/*
	void Editor::InitializeEditor(std::pair<GLFWwindow*, WindowProperties> windowContext)
	{
		m_WindowContext = windowContext;

		//Setup ImGui Context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		(void)io;

		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard; //Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable; //Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable; //Enable Multi-Viewport / Platform Windows
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad; //Enable Gamepad Controls
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportNoTaskbarIcons;
		//io.ConfigFlags |= ImGuiConfigFlags_ViewportsNoMerge;
		io.Fonts->AddFontFromFileTTF("Resources/Fonts/opensans/OpenSans-Bold.ttf", 17.0f);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("Resources/Fonts/opensans/OpenSans-Regular.ttf", 17.0f);

		//Setup Styling
		ImGui::StyleColorsDark();

		//When viewports are enabled, we tweak WindowRounding/WindowBg so Window platforms can look identifical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		SetupEditorStyling();
		io.FontDefault = io.Fonts->Fonts.back();

		//Setup Platform/Renderer Bindings
		ImGui_ImplGlfw_InitForOpenGL(m_WindowContext.first, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void Editor::BeginEditorRenderLoop()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void Editor::RenderDockingContext()
	{
		static bool dockSpaceOpen = true;
		static bool opt_fullscreen = true;
		static bool opt_padding = false;
		static ImGuiDockNodeFlags dockspace_flags = ImGuiDockNodeFlags_None;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(viewport->WorkSize);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}
		else
		{
			dockspace_flags &= ~ImGuiDockNodeFlags_PassthruCentralNode;
		}

		// When using ImGuiDockNodeFlags_PassthruCentralNode, DockSpace() will render our background
		// and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (dockspace_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		// Important: note that we proceed even if Begin() returns false (aka window is collapsed).
		// This is because we want to keep our DockSpace() active. If a DockSpace() is inactive,
		// all active windows docked into it will lose their parent and become undocked.
		// We cannot preserve the docking relationship between an active window and an inactive docking, otherwise
		// any change of dockspace/settings would lead to windows being stuck in limbo and never being visible.
		if (!opt_padding)
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace Demo", &dockSpaceOpen, window_flags);
		if (!opt_padding)
			ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// DockSpace
		ImGuiIO& io = ImGui::GetIO();
		ImGuiStyle& style = ImGui::GetStyle();

		float minimumWindowsize = style.WindowMinSize.x;
		style.WindowMinSize.x = 250.0f;
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockSpace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), dockspace_flags);
		}
		style.WindowMinSize.x = minimumWindowsize;
	}

	void Editor::EndEditorRenderLoop()
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(m_WindowContext.second.m_WindowWidth, m_WindowContext.second.m_WindowHeight); //For default viewport.

		//Rendering
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backupCurrentContext = m_WindowContext.first;
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(m_WindowContext.first);
		}
	}

	void Editor::SetupEditorStyling()
	{
		auto& colors = ImGui::GetStyle().Colors;
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
	*/
