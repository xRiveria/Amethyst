#pragma once
#include <string>
#include "../Widget.h"

namespace Amethyst
{
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
}