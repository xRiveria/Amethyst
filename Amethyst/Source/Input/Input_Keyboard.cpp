#include "Amethyst.h"
#include "Input.h"
#include "SDL/SDL.h"

namespace Amethyst
{
	void Input::PollKeyboard()
	{
		// Retrieve keyboard states. These keys correspond to our KeyCode class enum numbers.
		const Uint8* keyStates = SDL_GetKeyboardState(nullptr);

		// Function 
        m_Keys[0]  = keyStates[SDL_SCANCODE_F1];
        m_Keys[1]  = keyStates[SDL_SCANCODE_F2];
        m_Keys[2]  = keyStates[SDL_SCANCODE_F3];
        m_Keys[3]  = keyStates[SDL_SCANCODE_F4];
        m_Keys[4]  = keyStates[SDL_SCANCODE_F5];
        m_Keys[5]  = keyStates[SDL_SCANCODE_F6];
        m_Keys[6]  = keyStates[SDL_SCANCODE_F7];
        m_Keys[7]  = keyStates[SDL_SCANCODE_F8];
        m_Keys[8]  = keyStates[SDL_SCANCODE_F9];
        m_Keys[9]  = keyStates[SDL_SCANCODE_F10];
        m_Keys[10] = keyStates[SDL_SCANCODE_F11];
        m_Keys[11] = keyStates[SDL_SCANCODE_F12];
        m_Keys[12] = keyStates[SDL_SCANCODE_F13];
        m_Keys[13] = keyStates[SDL_SCANCODE_F14];
        m_Keys[14] = keyStates[SDL_SCANCODE_F15];

		// Numbers
        m_Keys[15] = keyStates[SDL_SCANCODE_0];
        m_Keys[16] = keyStates[SDL_SCANCODE_1];
        m_Keys[17] = keyStates[SDL_SCANCODE_2];
        m_Keys[18] = keyStates[SDL_SCANCODE_3];
        m_Keys[19] = keyStates[SDL_SCANCODE_4];
        m_Keys[20] = keyStates[SDL_SCANCODE_5];
        m_Keys[21] = keyStates[SDL_SCANCODE_6];
        m_Keys[22] = keyStates[SDL_SCANCODE_7];
        m_Keys[23] = keyStates[SDL_SCANCODE_8];
        m_Keys[24] = keyStates[SDL_SCANCODE_9];

		// Keypad
        m_Keys[25] = keyStates[SDL_SCANCODE_KP_0];
        m_Keys[26] = keyStates[SDL_SCANCODE_KP_1];
        m_Keys[27] = keyStates[SDL_SCANCODE_KP_2];
        m_Keys[28] = keyStates[SDL_SCANCODE_KP_3];
        m_Keys[29] = keyStates[SDL_SCANCODE_KP_4];
        m_Keys[30] = keyStates[SDL_SCANCODE_KP_5];
        m_Keys[31] = keyStates[SDL_SCANCODE_KP_6];
        m_Keys[32] = keyStates[SDL_SCANCODE_KP_7];
        m_Keys[33] = keyStates[SDL_SCANCODE_KP_8];
        m_Keys[34] = keyStates[SDL_SCANCODE_KP_9];

		// Letters
        m_Keys[35] = keyStates[SDL_SCANCODE_Q];
        m_Keys[36] = keyStates[SDL_SCANCODE_W];
        m_Keys[37] = keyStates[SDL_SCANCODE_E];
        m_Keys[38] = keyStates[SDL_SCANCODE_R];
        m_Keys[39] = keyStates[SDL_SCANCODE_T];
        m_Keys[40] = keyStates[SDL_SCANCODE_Y];
        m_Keys[41] = keyStates[SDL_SCANCODE_U];
        m_Keys[42] = keyStates[SDL_SCANCODE_I];
        m_Keys[43] = keyStates[SDL_SCANCODE_O];
        m_Keys[44] = keyStates[SDL_SCANCODE_P];
        m_Keys[45] = keyStates[SDL_SCANCODE_A];
        m_Keys[46] = keyStates[SDL_SCANCODE_S];
        m_Keys[47] = keyStates[SDL_SCANCODE_D];
        m_Keys[48] = keyStates[SDL_SCANCODE_F];
        m_Keys[49] = keyStates[SDL_SCANCODE_G];
        m_Keys[50] = keyStates[SDL_SCANCODE_H];
        m_Keys[51] = keyStates[SDL_SCANCODE_J];
        m_Keys[52] = keyStates[SDL_SCANCODE_K];
        m_Keys[53] = keyStates[SDL_SCANCODE_L];
        m_Keys[54] = keyStates[SDL_SCANCODE_Z];
        m_Keys[55] = keyStates[SDL_SCANCODE_X];
        m_Keys[56] = keyStates[SDL_SCANCODE_C];
        m_Keys[57] = keyStates[SDL_SCANCODE_V];
        m_Keys[58] = keyStates[SDL_SCANCODE_B];
        m_Keys[59] = keyStates[SDL_SCANCODE_N];
        m_Keys[60] = keyStates[SDL_SCANCODE_M];

		// Controls
        m_Keys[61] = keyStates[SDL_SCANCODE_ESCAPE];
        m_Keys[62] = keyStates[SDL_SCANCODE_TAB];
        m_Keys[63] = keyStates[SDL_SCANCODE_LSHIFT];
        m_Keys[64] = keyStates[SDL_SCANCODE_RSHIFT];
        m_Keys[65] = keyStates[SDL_SCANCODE_LCTRL];
        m_Keys[66] = keyStates[SDL_SCANCODE_RCTRL];
        m_Keys[67] = keyStates[SDL_SCANCODE_LALT];
        m_Keys[68] = keyStates[SDL_SCANCODE_RALT];
        m_Keys[69] = keyStates[SDL_SCANCODE_SPACE];
        m_Keys[70] = keyStates[SDL_SCANCODE_CAPSLOCK];
        m_Keys[71] = keyStates[SDL_SCANCODE_BACKSPACE];
        m_Keys[72] = keyStates[SDL_SCANCODE_RETURN];
        m_Keys[73] = keyStates[SDL_SCANCODE_DELETE];
        m_Keys[74] = keyStates[SDL_SCANCODE_LEFT];
        m_Keys[75] = keyStates[SDL_SCANCODE_RIGHT];
        m_Keys[76] = keyStates[SDL_SCANCODE_UP];
        m_Keys[77] = keyStates[SDL_SCANCODE_DOWN];
        m_Keys[78] = keyStates[SDL_SCANCODE_PAGEUP];
        m_Keys[79] = keyStates[SDL_SCANCODE_PAGEDOWN];
        m_Keys[80] = keyStates[SDL_SCANCODE_HOME];
        m_Keys[81] = keyStates[SDL_SCANCODE_END];
        m_Keys[82] = keyStates[SDL_SCANCODE_INSERT];
	}
}