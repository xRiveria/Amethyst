#pragma once
#include "../Widget.h"
#include <memory>

class Entity;

class Hierarchy : public Widget
{
public:
	Hierarchy(Editor* editor);
	void OnVisibleTick() override;

private:
	void ShowTree();
	void OnTreeBegin();
	void OnTreeEnd();
	void AddTreeEntity(Amethyst::Entity* entity);
	void HandleClicking();
	void HandleEntityDragAndDrop(Amethyst::Entity* entityPointer) const;
	void SetSelectedHierarchyEntity(const std::shared_ptr<Amethyst::Entity>& entity, bool fromEditor = true);

	//Misc (Popups)
	void Popups();
	void PopupContextMenu() const;
	void PopupEntityRename() const;
	static void HandleKeyShortcuts();
	

	//Context Menu Actions
};
