#include "Amethyst.h"
#include "ProgressDialog.h"
#include "../Resource/ProgressTracker.h"
#include <iostream>

namespace Amethyst
{
	static float g_DialogWidth = 500.0f;

	ProgressDialog::ProgressDialog(Editor* editor) : Widget(editor)
	{
		m_WidgetName = "Hold on...";
		m_IsWidgetVisible = false;
		m_Progress = 0.0f;
		m_WidgetSize = Math::Vector2(g_DialogWidth, 83.0f);
		m_WidgetFlags |= ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoDocking;
	}

	void ProgressDialog::OnTickAlways()
	{
		//Determine if an operation is in progress.
		ProgressTracker& progressReport = ProgressTracker::RetrieveInstance();
		const bool isLoadingModel = progressReport.RetrieveLoadStatus(ProgressType::ModelImporter);
		const bool isLoadingScene = progressReport.RetrieveLoadStatus(ProgressType::World);
		const bool inProgress = isLoadingModel || isLoadingScene;

		//Acquire progress.
		if (isLoadingModel)
		{
			m_Progress = progressReport.RetrievePercentage(ProgressType::ModelImporter);
			m_ProgressStatus = progressReport.RetrieveStatus(ProgressType::ModelImporter);
		}
		else if (isLoadingScene)
		{
			m_Progress = progressReport.RetrievePercentage(ProgressType::World);
			m_ProgressStatus = progressReport.RetrieveStatus(ProgressType::World);
		}

		//Show only if an operation is in progress.
		SetWidgetVisibility(inProgress);
	}

	void ProgressDialog::OnVisibleTick()
	{
		ImGui::SetWindowFocus();
		ImGui::PushItemWidth(g_DialogWidth - ImGui::GetStyle().WindowPadding.x * 2.0f);
		ImGui::ProgressBar(m_Progress, ImVec2(0.0f, 0.0f));
		ImGui::Text(m_ProgressStatus.c_str());
		ImGui::PopItemWidth();
	}
}