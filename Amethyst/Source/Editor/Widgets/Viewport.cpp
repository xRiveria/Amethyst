#include "Amethyst.h"
#include "Viewport.h"
#include "Context.h"
#include "../Utilities/EditorExtensions.h"

Viewport::Viewport(Editor* editor) : Widget(editor)
{
	m_WidgetName = "Viewport";
	m_WidgetSize = Amethyst::Math::Vector2(450, 250);
	m_WidgetFlags |= ImGuiWindowFlags_NoScrollbar;
	m_WidgetPadding = Amethyst::Math::Vector2(4.0f);

	//Context Class Creation.
	//m_World = m_Context->RetrieveSubsystem<World>();
}

void Viewport::OnVisibleTick()
{
	//Return if Renderer not found.

	//Retrieve Size.
	float width = static_cast<float>(ImGui::GetWindowContentRegionMax().x - ImGui::GetWindowContentRegionMin().x);
	float height = static_cast<float>(ImGui::GetWindowContentRegionMax().y - ImGui::GetWindowContentRegionMin().y);

	//Retrieve offset.
	Amethyst::Math::Vector2 offset = Amethyst::Math::Vector2(ImGui::GetWindowPos()) + m_WindowPadding;

	//Update engine's viewport accordingly if there are changes.
	if (m_ViewportWidth != width || m_ViewportHeight != height || m_ViewportOffset != offset)
	{
		//Update the size of our viewport accordingly.

		m_ViewportWidth = width;
		m_ViewportHeight = height;
		m_ViewportOffset = offset;
	}

	//If this is our first tick and the first time the engine runs (no settings file loaded), we set the resolution to match the viewport's size.
	//This is to avoid a scenario where the resolution is much higher than what the user assumes, resulting in less performance.
	if (m_IsResolutionDirty) //And settings loaded returns false, meaning our settings file does not yet exist.
	{
		//Set Resolution of our Renderer.
		m_IsResolutionDirty = false;
	}

	//Draw the image after a potential Renderer::SetResolution() call has been made.
	//ImGuiExtensions::Image(
	//	//
	//	ImVec2(static_cast<float>(m_ViewportW))
	
	//Let the input system know if the mouse is within the viewport.

	//If the widget was released, make the engine pick an entity.
	if (ImGui::IsMouseDown(0) && ImGui::IsItemHovered())
	{

	}

	//Editor Model Drop.
}
