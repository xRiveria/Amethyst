#include "Amethyst.h"
#include "QuickDiagnostics.h"
#include "../Source/RHI/RHI_Utilities.h"
#include <iostream>
#include "../Resource/ProgressTracker.h"

namespace FPS
{
	static double g_PreviousFrameTime = 0;
	static int g_FrameCount = 0;
	static double g_FramesPerSecond = 0;
}

QuickDiagnostics::QuickDiagnostics(Editor* editor) : Widget(editor)
{
	m_WidgetName = "Diagnostics";

	//FPS::g_PreviousFrameTime = glfwGetTime();
	FPS::g_FrameCount = 0;
}

void QuickDiagnostics::OnVisibleTick()
{
	//ImGui::Text("Vendor: %s", RetrieveRHIVendor().c_str());
	//ImGui::Text("Renderer: %s", RetrieveRHIRenderer().c_str());;
	//ImGui::Text("Version: %s", RetrieveRHIVersion().c_str());

	ImGui::Separator();

	/*
	{
		double currentTime = glfwGetTime();
		double delta = currentTime - FPS::g_PreviousFrameTime;
		FPS::g_FrameCount++;

		if (delta >= 1.0) //If a second passes...
		{
			FPS::g_FramesPerSecond = double(FPS::g_FrameCount) / delta;

			FPS::g_FrameCount = 0;
			FPS::g_PreviousFrameTime = currentTime;
		}
	}
	*/
	//ImGui::Text("Frame Rate (GLFW): %.3f ms/frame (%.1f FPS)", 1000.0f / FPS::g_FramesPerSecond, FPS::g_FramesPerSecond);
	ImGui::Text("Frame Rate (ImGui): %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);

	if (ImGui::Button("Test Loading"))
	{
		AMETHYST_INFO("Pressed!");
		//Start Progress Report
		Amethyst::ProgressTracker::RetrieveInstance().Reset(Amethyst::ProgressType::World);
		Amethyst::ProgressTracker::RetrieveInstance().SetLoadStatus(Amethyst::ProgressType::World, true);
		Amethyst::ProgressTracker::RetrieveInstance().SetStatus(Amethyst::ProgressType::World, "Saving World...");

		Amethyst::ProgressTracker::RetrieveInstance().SetJobCount(Amethyst::ProgressType::World, 5000);

		for (int i = 0; i < 5000; i++)
		{
			Amethyst::ProgressTracker::RetrieveInstance().IncrementJobsDone(Amethyst::ProgressType::World);
		}

		//End
		//ProgressTracker::RetrieveInstance().SetLoadStatus(ProgressType::World, false);
	}
}
