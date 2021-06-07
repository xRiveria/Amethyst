#pragma once
#include <string>
#include "../Widget.h"

class ProgressDialog : public Widget
{
public:
	ProgressDialog(Editor* editor);
	~ProgressDialog() = default;

	void OnTickAlways() override;
	void OnVisibleTick() override;

private:
	float m_Progress;
	std::string m_ProgressStatus;
};
