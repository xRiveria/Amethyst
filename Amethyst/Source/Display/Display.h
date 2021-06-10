#pragma once
#include "DisplayMode.h"
#include "../Core/Context.h"

namespace Amethyst
{
	class Display
	{
	public:
		Display() = default;

		// Display Modes
		static void RegisterDisplayMode(const DisplayMode& displayMode, Context* engineContext);
		static void SetActiveDisplayMode(const DisplayMode& displayMode) { m_DisplayModeActive = displayMode; }
		static DisplayMode& RetrieveActiveDisplayMode() { return m_DisplayModeActive; }
		static const std::vector<DisplayMode>& RetrieveDisplayModes() { return m_DisplayModes; }

		// Display Dimensions
		static uint32_t RetrieveDisplayWidthPrimary();
		static uint32_t RetrieveDisplayHeightPrimary();

		// Virtual Display Dimensions (Multiple Displays)
		static uint32_t RetrieveDisplayWidthVirtual();
		static uint32_t RetrieveDisplayHeightVirtual();

	private:
		static std::vector<DisplayMode> m_DisplayModes;
		static DisplayMode m_DisplayModeActive;
	};
}