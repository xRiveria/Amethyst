#pragma once
#include <array>
#include "../Runtime/Math/Vector2.h"
#include "../Core/ISubsystem.h"
#include "KeyCode.h"

namespace Amethyst
{
	class Input : public ISubsystem
	{
	public:
		Input(Context* context);
		~Input() = default;

		void OnUpdate(float deltaTime) override;

		//Keys
		bool GetKey(const KeyCode key) { return m_Keys[static_cast<uint32_t>(key)]; } //Returns true while the button identified by KeyCode is held down.
		bool GetKeyDown(const KeyCode key) { return GetKey(key) && !m_KeysPreviousFrame[static_cast<uint32_t>(key)]; } //Returns true during the frame the user pressed down the button identified by KeyCode.
		bool GetKeyUp(const KeyCode key) { return !GetKey(key) && m_KeysPreviousFrame[static_cast<uint32_t>(key)]; } //Returns true the first frame the user releases the button identified by KeyCode.

		//Mouse
		const Math::Vector2& GetMousePosition() const { return m_MousePosition; }
		void SetMousePosition(const Math::Vector2& mousePosition);
		const Math::Vector2& GetMouseDelta() const { return m_MouseDelta; }

		//Gamepad
		bool IsGamepadConnected() const { return m_IsGamepadConnected; }
		const Math::Vector2& GetGamepadThumbStickLeft() const { return m_GamepadThumbLeft; }
		const Math::Vector2& GetGamepadThumbStickRight() const { return m_GamepadThumbRight; }
		float GetGamepadTriggerLeft() const { return m_GamepadTriggerLeft; }
		float GetGamepadTriggerRight() const { return m_GamepadTriggerRight; }

		//Vibrate the gamepad. Motor speed range is from 0.0 to 1.0f.
		//The left motor is the low-frequency rumble motor. The right motor is the high-frequency rumble motor.
		//The two motors are not the same and they may create different vibration effects.
		bool VibrateGamepad(float leftMotorSpeed, float rightMotorSpeed) const;

	private:
		//Keys
		std::array<bool, 99> m_Keys;
		std::array<bool, 99> m_KeysPreviousFrame;
		uint32_t m_StartIndexMouse = 83; //Left Click
		uint32_t m_StartIndexGamepad = 86; //D-Pad Up

		//Mouse
		Math::Vector2 m_MousePosition = Math::Vector2::Zero;
		Math::Vector2 m_MouseDelta = Math::Vector2::Zero;
		float m_MouseWheelDelta = 0;

		//Gamepad
		bool m_IsGamepadConnected = false;
		Math::Vector2 m_GamepadThumbLeft = Math::Vector2::Zero;
		Math::Vector2 m_GamepadThumbRight = Math::Vector2::Zero;
		float m_GamepadTriggerLeft = 0.0f;
		float m_GamepadTriggerRight = 0.0f;

		//Misc
		bool m_IsNewFrame = false;
		bool m_CheckForNewDevice = false;
	};
}