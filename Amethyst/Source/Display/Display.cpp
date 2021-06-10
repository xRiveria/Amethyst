#include "Amethyst.h"
#include "Display.h"
#include <Windows.h>

namespace Amethyst
{
	std::vector<DisplayMode> Display::m_DisplayModes;
	DisplayMode Display::m_DisplayModeActive;

	void Display::RegisterDisplayMode(const DisplayMode& displayMode, Context* engineContext)
	{
		// Early exit if display is already registered.
		for (const DisplayMode& registeredDisplayMode : m_DisplayModes)
		{
			if (registeredDisplayMode == displayMode)
			{
				return;
			}
		}

		DisplayMode& newDisplayMode = m_DisplayModes.emplace_back(displayMode);

		AMETHYST_INFO("Registered display mode of %dx%d with %d hertz.", newDisplayMode.m_Width, newDisplayMode.m_Height, newDisplayMode.m_Hertz);

		// Keep display modes sorted, based on refresh rate (from highest to lowest).
		std::sort(m_DisplayModes.begin(), m_DisplayModes.end(), [](const DisplayMode& displayModeA, const DisplayMode& displayModeB)
		{
			return displayModeA.m_Hertz > displayModeB.m_Hertz;
		});

		// Find preferred display mode.
		for (const DisplayMode& displayMode : m_DisplayModes)
		{
			// Try to use a higher resolution.
			if (displayMode.m_Width > m_DisplayModeActive.m_Width || displayMode.m_Height > m_DisplayModeActive.m_Height)
			{
				// But not a lower refresh rate.
				if (displayMode.m_Hertz >= m_DisplayModeActive.m_Hertz)
				{
					m_DisplayModeActive.m_Width = displayMode.m_Width;
					m_DisplayModeActive.m_Height = displayMode.m_Height;
					m_DisplayModeActive.m_Hertz = displayMode.m_Hertz;
					m_DisplayModeActive.m_Numerator = displayMode.m_Numerator;
					m_DisplayModeActive.m_Denominator = displayMode.m_Denominator;
				}
			}
		}

		// Let the timer know about the fresh rates this monitor is capable of (will result in low latency/smooth ticking).
		engineContext->RetrieveSubsystem<Timer>()->SetTargetFPS(m_DisplayModes.front().m_Hertz);
	}

	// See: https://docs.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-getsystemmetrics

	uint32_t Display::RetrieveDisplayWidthPrimary()
	{
		return static_cast<uint32_t>(GetSystemMetrics(SM_CXSCREEN)); // The width of the primary display monitor in pixels.
	}

	uint32_t Display::RetrieveDisplayHeightPrimary()
	{
		return static_cast<uint32_t>(GetSystemMetrics(SM_CYSCREEN)); // The height of the primary display monitor in pixels.
	}

	uint32_t Display::RetrieveDisplayWidthVirtual()
	{
		return static_cast<uint32_t>(GetSystemMetrics(SM_CXVIRTUALSCREEN)); // The width of the virtual screen in pixels. The virtual screen is the bounding rectangle of all display monitors.
	}

	uint32_t Display::RetrieveDisplayHeightVirtual()
	{
		return static_cast<uint32_t>(GetSystemMetrics(SM_CYVIRTUALSCREEN)); // The height of the virtual screen in pixels. The virtual screen is the bounding rectangle of all display monitors.
	}
}