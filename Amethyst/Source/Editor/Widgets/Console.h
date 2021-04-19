#pragma once
#include "../Widget.h"
#include <deque>
#include <vector>
#include <ImGui/imgui.h>
#include <glm/glm.hpp>

namespace Amethyst
{
	struct LogPackage
	{
		LogPackage(const std::string& text, const int& errorLevel) : m_Text(text), m_ErrorLevel(errorLevel)
		{

		}

		std::string m_Text;
		unsigned int m_ErrorLevel = 0;
	};

	class Console : public Widget
	{
	public:
		Console();
		//Console(uint32_t maxLogCount);

		void OnVisibleTick() override;

		//Console Specific
		void AddLogPackage(const LogPackage& logPackage);
		void ClearConsole();

	private:
		std::deque<LogPackage> m_Logs;
		uint32_t m_LogMaximumCount = 300;

		//Log Types
		uint32_t m_LogTypeCount[3] = { 0, 0, 0 }; //Info, Warning & Error Respectively.
		const std::vector<glm::vec4> m_LogTypeColor =
		{
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),    // Info
			glm::vec4(0.7f, 0.75f, 0.0f, 1.0f),     // Warning
			glm::vec4(0.7f, 0.3f, 0.3f, 1.0f)       // Error
		};
		
		//Filtering
		bool m_LogTypeVisibilityState[3] = { true, true, true }; //For filtering purposes.
		ImGuiTextFilter m_LogFilter;
	};
}