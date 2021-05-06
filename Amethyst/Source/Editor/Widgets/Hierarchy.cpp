#include "Amethyst.h"
#include "Hierarchy.h"
#include "../../Runtime/ECS/World.h"
#include "../../Runtime//ECS/Entity.h"
#include "../Utilities/EditorExtensions.h"

namespace Amethyst
{
	namespace HierarchyInternal
	{
		static World* g_World = nullptr;
		static bool g_RenameEntityPopupActive = false;
		static ImGuiExtensions::DragDropPayload g_DragAndDropPayload;

		//Entities in relation to mouse events.
		static Entity* g_EntityCopied = nullptr;
		static Entity* g_EntityHovered = nullptr;
		static Entity* g_EntityClicked = nullptr;
	}

	Hierarchy::Hierarchy(Editor* editor) : Widget(editor)
	{
		m_WidgetName = "Hierarchy";
		m_WidgetFlags |= ImGuiWindowFlags_HorizontalScrollbar;

		//Context

		//Subscribe to entity clicked engine event.
	}

	void Hierarchy::OnVisibleTick()
	{
		//If our world is loading in entities, don't parse the hierarchy.
		if (HierarchyInternal::g_World->IsWorldLoading())
		{
			return;
		}

		ShowTree();

		//On left click, select entity but only on mouse release.
		if (ImGui::IsMouseReleased(0) && HierarchyInternal::g_EntityClicked)
		{
			//Ensure that the mouse was released while hovering on the same entity.
			if (HierarchyInternal::g_EntityHovered && HierarchyInternal::g_EntityHovered->RetrieveObjectID() == HierarchyInternal::g_EntityClicked->RetrieveObjectID())
			{
				SetSelectedHierarchyEntity(HierarchyInternal::g_EntityClicked->RetrieveSharedPointer());
			}

			HierarchyInternal::g_EntityClicked = nullptr;
		}
	}

	void Hierarchy::ShowTree()
	{
		OnTreeBegin();

		if (ImGui::TreeNodeEx("Root", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_SpanFullWidth))
		{
			ImGui::TreePop();
		}

		OnTreeEnd();
	}

	void Hierarchy::OnTreeBegin()
	{
		HierarchyInternal::g_EntityHovered = nullptr;
	}

	void Hierarchy::OnTreeEnd()
	{
		HandleKeyShortcuts();
		HandleClicking();
		Popups();
	}
	void Hierarchy::AddTreeEntity(Amethyst::Entity* entity)
	{
	}
	void Hierarchy::HandleClicking()
	{
	}
	void Hierarchy::HandleEntityDragAndDrop(Amethyst::Entity* entityPointer) const
	{
	}
	void Hierarchy::SetSelectedHierarchyEntity(const std::shared_ptr<Amethyst::Entity>& entity, bool fromEditor)
	{
	}
	void Hierarchy::Popups()
	{
	}
	void Hierarchy::PopupContextMenu() const
	{
	}
	void Hierarchy::PopupEntityRename() const
	{
	}
	void Hierarchy::HandleKeyShortcuts()
	{
	}
}