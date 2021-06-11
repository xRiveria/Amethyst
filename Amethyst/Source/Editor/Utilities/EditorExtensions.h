#pragma once
#include <string>
#include <variant>
#include "IconLibrary.h"
#include "Source/Input/Input.h"
#include "Source/Runtime/ECS/World.h"
#include "Source/Rendering/Renderer.h"
#include "Source/Resource/ResourceCache.h"
#include "Source/Editor/ImGui/Source/imgui.h"
#include "Source/Editor/ImGui/Source/imgui_internal.h"
#include <variant>
#include <iostream>

class EditorHelper
{
public:
	static EditorHelper& RetrieveEditorHelperInstance()
	{
		static EditorHelper m_EditorHelperInstance;
		return m_EditorHelperInstance;
	}

	void Initialize(Amethyst::Context* engineContext)
	{
		g_EngineContext		= engineContext;
		g_ResourceCache		= engineContext->RetrieveSubsystem<Amethyst::ResourceCache>();
		g_WorldSystem		= engineContext->RetrieveSubsystem<Amethyst::World>();
		g_ThreadingSystem   = engineContext->RetrieveSubsystem<Amethyst::Threading>();
		g_Renderer			= engineContext->RetrieveSubsystem<Amethyst::Renderer>();
		g_InputSystem		= engineContext->RetrieveSubsystem<Amethyst::Input>();
	}

	void LoadModel(const std::string& filePath) const // Load a model.
	{

	}

	void LoadWorld(const std::string& filePath) const // Load a scene.
	{

	}

	void SaveWorld(const std::string& filePath) const // Save a scene.
	{

	}

	void PickEntity() // Literally pick an entity in the scene.
	{

	}

	void SetSelectedEntity(const std::shared_ptr<Amethyst::Entity>& entity) // Set selected entity.
	{

	}

public:
	Amethyst::Context* g_EngineContext = nullptr;
	Amethyst::ResourceCache* g_ResourceCache = nullptr;
	Amethyst::Threading* g_ThreadingSystem = nullptr;
	Amethyst::World* g_WorldSystem = nullptr;
	Amethyst::Renderer* g_Renderer = nullptr;
	Amethyst::Input* g_InputSystem = nullptr;

	std::weak_ptr<Amethyst::Entity> g_SelectedEntity;
	std::function<void()> g_OnEntitySelected = nullptr;
};

namespace ImGuiExtensions
{
	static const ImVec4 g_DefaultTint = { 255, 255, 255, 255 };

	inline void Image(const IconType iconType, const float iconSize)
	{
		ImGui::Image(
			static_cast<ImTextureID>(IconLibrary::RetrieveIconLibraryInstance().RetrieveTextureByType(iconType)),
			ImVec2(iconSize, iconSize),
			ImVec2(0, 0),
			ImVec2(1, 1),
			g_DefaultTint,
			ImColor(0, 0, 0, 0) //Border
		);
	}

	inline void Image(const IconType iconType, const float iconSizeX, const float iconSizeY)
	{
		ImGui::Image(
			static_cast<ImTextureID>(IconLibrary::RetrieveIconLibraryInstance().RetrieveTextureByType(iconType)),
			ImVec2(iconSizeX, iconSizeY),
			ImVec2(0, 0),
			ImVec2(1, 1),
			g_DefaultTint,
			ImColor(0, 0, 0, 0) //Border
		);
	}

	inline void Image(Amethyst::RHI_Texture* texture, const float itemSize)
	{
		ImGui::Image(
			static_cast<ImTextureID>(texture),
			ImVec2(itemSize, itemSize),
			ImVec2(0, 0),
			ImVec2(1, 1),
			g_DefaultTint,
			ImColor(0, 0, 0, 0) //Border
		);
	}

	inline bool ImageButton(const IconType iconType, const float iconSize)
	{
		return ImGui::ImageButton(
			static_cast<ImTextureID>(IconLibrary::RetrieveIconLibraryInstance().RetrieveTextureByType(iconType)),
			ImVec2(iconSize, iconSize),
			ImVec2(0, 0),
			ImVec2(1, 1),
			-1,
			ImColor(0, 0, 0, 0),
			g_DefaultTint
		);
	}

	//Drag & Drop
	enum DragPayloadType
	{
		DragPayload_Unknown,
		DragPayload_Texture,
		DragPayload_Entity,
		DragPayload_Model,
		DragPayload_Audio,
		DragPayload_Script,
		DragPayload_Material
	};

	struct DragDropPayload
	{
		typedef std::variant<const char*, unsigned int> DataVariant; //Holds either a const char* or unsigned integer identifier.

		DragDropPayload(const DragPayloadType payloadType = DragPayload_Unknown, const DataVariant data = nullptr)
		{
			this->m_PayloadType = payloadType;
			this->m_Data = data;
		}

		DragPayloadType m_PayloadType;
		DataVariant m_Data;
	};

	inline void CreateDragPayload(const DragDropPayload& payload)
	{
		ImGui::SetDragDropPayload(reinterpret_cast<const char*>(&payload.m_PayloadType), reinterpret_cast<const void*>(&payload), sizeof(payload), ImGuiCond_Once);
	}

	inline DragDropPayload* ReceiveDragPayload(DragPayloadType payloadType)
	{
		if (ImGui::BeginDragDropTarget()) // Remember that BeginDragDropTarget() uses the last item in the window.
		{
			// ImGui::AcceptDragDropPayload will reject the data and return null if the given type does not match the type of the dragged payload.
			if (const auto payload = ImGui::AcceptDragDropPayload(reinterpret_cast<const char*>(&payloadType))) // If matches, accept the data that comes in and stores it inside the payload variable.		
			{
				return static_cast<DragDropPayload*>(payload->Data); // Retrieve our payload object from the payload data currently saved in the drag operation.
			}

			ImGui::EndDragDropTarget();
		}

		return nullptr;
	}
}