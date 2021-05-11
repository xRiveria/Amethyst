#include "Amethyst.h"
#include "../Input.h"

namespace Amethyst
{
	Input::Input(Context* context) : ISubsystem(context)
	{

	}

	void Input::OnUpdate(float deltaTime)
	{
		//Check for new device.
		if (m_CheckForNewDevice)
		{
			//Check for an XBox controller.


			if (m_IsGamepadConnected)
			{
				AMETHYST_INFO("Gamepad Connected.");
			}
			else
			{
				AMETHYST_INFO("Gamepad Disconnected.");
			}

			m_CheckForNewDevice = false;
		}

		if (m_IsGamepadConnected)
		{

		}
		else
		{

		}

		m_IsNewFrame = true;
	}

	void Input::SetMousePosition(const Math::Vector2& mousePosition)
	{
	}

	bool Input::VibrateGamepad(float leftMotorSpeed, float rightMotorSpeed) const
	{
		return false;
	}
}