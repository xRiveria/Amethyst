#pragma once
#include <array>
#include "../Runtime/Math/Vector2.h"
#include "../Core/ISubsystem.h"
#include "KeyCode.h"
#include "../Core/Variant.h"

/* === Input System ===

	Amethyst stores internal bool states for each key (mouse, keyboard, controller). This state is polled from SDL, which is the bridge to our platform's input system.
	We use this polled information for all our needs, which is fed in and out of our event system.
*/

namespace Amethyst
{
	class Input : public ISubsystem
	{
	public:
		Input(Context* engineContext);
		~Input() = default;

		// === ISubsystem ===
		void OnUpdate(float deltaTime) override;
		void OnPostUpdate() override;

		// =======

		// Polling Driven Input
		void PollMouse();
		void PollKeyboard();

		// Event Driven Input
		void OnEvent(const Variant& eventVariant);
		void OnEventMouse(void* eventMouse);
		/// void OnEventController(void* eventController);

		// Keys
		bool GetKey(const KeyCode key) { return m_Keys[static_cast<uint32_t>(key)]; }									//Returns true while the button identified by KeyCode is held down.
		bool GetKeyDown(const KeyCode key) { return GetKey(key) && !m_KeysPreviousFrame[static_cast<uint32_t>(key)]; }  //Returns true during the frame the user pressed down the button identified by KeyCode.
		bool GetKeyUp(const KeyCode key) { return !GetKey(key) && m_KeysPreviousFrame[static_cast<uint32_t>(key)]; }    //Returns true the first frame the user releases the button identified by KeyCode.

		// Mouse
		void SetMouseCursorVisible(const bool isVisible);
		bool GetMouseCursorVisible() const { return m_MouseCursorVisible; }
		void SetMouseIsInViewport(const bool isInViewport) { m_MouseIsInViewport = isInViewport; }
		bool GetMouseIsInViewport() const { return m_MouseIsInViewport; }
		const Math::Vector2& GetMousePosition() const { return m_MousePosition; }
		void SetMousePosition(const Math::Vector2& mousePosition);
		const Math::Vector2& GetMouseDelta() const { return m_MouseDelta; }
		const Math::Vector2& GetMouseWheelDelta() const { return m_MouseWheelDelta; }
		void SetEditorViewportOffset(const Math::Vector2& offset) { m_EditorViewportOffset = offset; }
		const Math::Vector2 GetMousePositionRelativeToWindow() const;
		const Math::Vector2 GetMousePositionRelativeToEditorViewport() const;

		// Gamepad
		/// bool IsGamepadConnected() const { return m_IsGamepadConnected; }
		/// const Math::Vector2& GetGamepadThumbStickLeft() const { return m_GamepadThumbLeft; }
		/// const Math::Vector2& GetGamepadThumbStickRight() const { return m_GamepadThumbRight; }
		/// float GetGamepadTriggerLeft() const { return m_GamepadTriggerLeft; }
		/// float GetGamepadTriggerRight() const { return m_GamepadTriggerRight; }

		// Vibrate the gamepad. Motor speed range is from 0.0 to 1.0f.
		// The left motor is the low-frequency rumble motor. The right motor is the high-frequency rumble motor.
		// The two motors are not the same and they may create different vibration effects.
		/// bool VibrateGamepad(float leftMotorSpeed, float rightMotorSpeed) const;

	private:
		// Keys
		std::array<bool, 85> m_Keys; // Array size will be the max size of your key codes enum.
		std::array<bool, 85> m_KeysPreviousFrame;
		uint32_t m_StartIndexMouse   = 83; // Start index of our mouse in our KeyCodes enum. 
		/// uint32_t m_StartIndexGamepad = 86; //D-Pad Up

		// Mouse
		Math::Vector2 m_MousePosition        = Math::Vector2::Zero;
		Math::Vector2 m_MouseDelta           = Math::Vector2::Zero; // Difference between the last and current known position of the mouse.
		Math::Vector2 m_MouseWheelDelta		 = Math::Vector2::Zero;
		Math::Vector2 m_EditorViewportOffset = Math::Vector2::Zero;
		bool m_MouseIsInViewport = true;
		bool m_MouseCursorVisible = true;

		/// // Gamepad
		/// bool m_IsGamepadConnected = false;
		/// Math::Vector2 m_GamepadThumbLeft = Math::Vector2::Zero;
		/// Math::Vector2 m_GamepadThumbRight = Math::Vector2::Zero;
		/// float m_GamepadTriggerLeft = 0.0f;
		/// float m_GamepadTriggerRight = 0.0f;
	};
}