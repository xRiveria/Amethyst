#pragma once

namespace Amethyst
{
	enum class KeyCode
	{
		//Keyboard
		F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12, F13, F14, F15, //Function

		//Numbers
		Alpha0, Alpha1, Alpha2, Alpha3, Alpha4, Alpha5, Alpha6, Alpha7, Alpha8, Alpha9,

		//Numpad
		Keypad0, Keypad1, Keypad2, Keypad3, Keypad4, Keypad5, Keypad6, Keypad7, Keypad8, Keypad9,

		//Letters
		Q, W, E, R, T, Y, U, I, O, P,
		A, S, D, F, G, H, J, K, L,
		Z, X, C, V, B, N, M,

		//Controls
		Escape,
		Tab,
		ShiftLeft, ShiftRight,
		CtrlLeft, CtrlRight,
		AltLeft, AltRight,
		Space,
		CapsLock,
		Backspace,
		Enter,
		Delete,
		ArrowLeft, ArrowRight, ArrowUp, ArrowDown,
		PageUp, PageDown,
		Home,
		End,
		Insert,

		//Mouse
		ClickLeft, ClickMiddle, ClickRight,

		///
		/* To Be Updated 
		//Gamepad
		DPadUp, DPadDown, DPadLeft, DPadRight,
		ButtonA, ButtonB, ButtonX, ButtonY,
		Start, Back,
		LeftThumb, RightThumb,
		LeftShoulder, RightShoulder
		*/
	};
}