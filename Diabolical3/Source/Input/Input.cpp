#include "Input.h"
#include "SDL2/SDL_scancode.h"
#include "Check.h"
#include "Types/DString.h"
#include "Logging/Logging.h"

EKeyCode DInputStack::FromSDLScanCode(uint16_t SDLScanCode)
{
	SDL_Scancode ScanCode = (SDL_Scancode)SDLScanCode;
	switch (ScanCode)
	{
	case SDL_SCANCODE_A:
		return EKeyCode::KC_A;
	case SDL_SCANCODE_B:
		return EKeyCode::KC_B;
	case SDL_SCANCODE_C:
		return EKeyCode::KC_C;
	case SDL_SCANCODE_D:
		return EKeyCode::KC_D;
	case SDL_SCANCODE_E:
		return EKeyCode::KC_E;
	case SDL_SCANCODE_F:
		return EKeyCode::KC_F;
	case SDL_SCANCODE_G:
		return EKeyCode::KC_G;
	case SDL_SCANCODE_H:
		return EKeyCode::KC_H;
	case SDL_SCANCODE_I:
		return EKeyCode::KC_I;
	case SDL_SCANCODE_J:
		return EKeyCode::KC_J;
	case SDL_SCANCODE_K:
		return EKeyCode::KC_K;
	case SDL_SCANCODE_L:
		return EKeyCode::KC_L;
	case SDL_SCANCODE_M:
		return EKeyCode::KC_M;
	case SDL_SCANCODE_N:
		return EKeyCode::KC_N;
	case SDL_SCANCODE_O:
		return EKeyCode::KC_O;
	case SDL_SCANCODE_P:
		return EKeyCode::KC_P;
	case SDL_SCANCODE_Q:
		return EKeyCode::KC_Q;
	case SDL_SCANCODE_R:
		return EKeyCode::KC_R;
	case SDL_SCANCODE_S:
		return EKeyCode::KC_S;
	case SDL_SCANCODE_T:
		return EKeyCode::KC_T;
	case SDL_SCANCODE_U:
		return EKeyCode::KC_U;
	case SDL_SCANCODE_V:
		return EKeyCode::KC_V;
	case SDL_SCANCODE_W:
		return EKeyCode::KC_W;
	case SDL_SCANCODE_X:
		return EKeyCode::KC_X;
	case SDL_SCANCODE_Y:
		return EKeyCode::KC_Y;
	case SDL_SCANCODE_Z:
		return EKeyCode::KC_Z;
	case SDL_SCANCODE_0:
		return EKeyCode::KC_0;
	case SDL_SCANCODE_1:
		return EKeyCode::KC_1;
	case SDL_SCANCODE_2:
		return EKeyCode::KC_2;
	case SDL_SCANCODE_3:
		return EKeyCode::KC_3;
	case SDL_SCANCODE_4:
		return EKeyCode::KC_4;
	case SDL_SCANCODE_5:
		return EKeyCode::KC_5;
	case SDL_SCANCODE_6:
		return EKeyCode::KC_6;
	case SDL_SCANCODE_7:
		return EKeyCode::KC_7;
	case SDL_SCANCODE_8:
		return EKeyCode::KC_8;
	case SDL_SCANCODE_9:
		return EKeyCode::KC_9;
	case SDL_SCANCODE_RETURN:
		return EKeyCode::KC_Return;
	case SDL_SCANCODE_ESCAPE:
		return EKeyCode::KC_Escape;
	case SDL_SCANCODE_BACKSPACE:
		return EKeyCode::KC_Backspace;
	case SDL_SCANCODE_TAB:
		return EKeyCode::KC_Tab;
	case SDL_SCANCODE_SPACE:
		return EKeyCode::KC_Space;
	case SDL_SCANCODE_MINUS:
		return EKeyCode::KC_Minus;
	case SDL_SCANCODE_EQUALS:
		return EKeyCode::KC_Equals;
	case SDL_SCANCODE_LEFTBRACKET:
		return EKeyCode::KC_LeftBracket;
	case SDL_SCANCODE_RIGHTBRACKET:
		return EKeyCode::KC_RightBracket;
	case SDL_SCANCODE_BACKSLASH:
		return EKeyCode::KC_BackSlash;
	case SDL_SCANCODE_SEMICOLON:
		return EKeyCode::KC_SemiColon;
	case SDL_SCANCODE_APOSTROPHE:
		return EKeyCode::KC_Apostrophe;
	case SDL_SCANCODE_GRAVE:
		return EKeyCode::KC_Grave;
	case SDL_SCANCODE_COMMA:
		return EKeyCode::KC_Comma;
	case SDL_SCANCODE_PERIOD:
		return EKeyCode::KC_Period;
	case SDL_SCANCODE_SLASH:
		return EKeyCode::KC_Slash;
	case SDL_SCANCODE_CAPSLOCK:
		return EKeyCode::KC_CapsLock;
	case SDL_SCANCODE_F1:
		return EKeyCode::KC_F1;
	case SDL_SCANCODE_F2:
		return EKeyCode::KC_F2;
	case SDL_SCANCODE_F3:
		return EKeyCode::KC_F3;
	case SDL_SCANCODE_F4:
		return EKeyCode::KC_F4;
	case SDL_SCANCODE_F5:
		return EKeyCode::KC_F5;
	case SDL_SCANCODE_F6:
		return EKeyCode::KC_F6;
	case SDL_SCANCODE_F7:
		return EKeyCode::KC_F7;
	case SDL_SCANCODE_F8:
		return EKeyCode::KC_F8;
	case SDL_SCANCODE_F9:
		return EKeyCode::KC_F9;
	case SDL_SCANCODE_F10:
		return EKeyCode::KC_F10;
	case SDL_SCANCODE_F11:
		return EKeyCode::KC_F11;
	case SDL_SCANCODE_F12:
		return EKeyCode::KC_F12;
	case SDL_SCANCODE_PRINTSCREEN:
		return EKeyCode::KC_PrintScreen;
	case SDL_SCANCODE_SCROLLLOCK:
		return EKeyCode::KC_ScrollLock;
	case SDL_SCANCODE_PAUSE:
		return EKeyCode::KC_Pause;
	case SDL_SCANCODE_INSERT:
		return EKeyCode::KC_Insert;
	case SDL_SCANCODE_HOME:
		return EKeyCode::KC_Home;
	case SDL_SCANCODE_PAGEUP:
		return EKeyCode::KC_PageUp;
	case SDL_SCANCODE_DELETE:
		return EKeyCode::KC_Delete;
	case SDL_SCANCODE_END:
		return EKeyCode::KC_End;
	case SDL_SCANCODE_PAGEDOWN:
		return EKeyCode::KC_PageDown;
	case SDL_SCANCODE_RIGHT:
		return EKeyCode::KC_Right;
	case SDL_SCANCODE_LEFT:
		return EKeyCode::KC_Left;
	case SDL_SCANCODE_DOWN:
		return EKeyCode::KC_Down;
	case SDL_SCANCODE_UP:
		return EKeyCode::KC_Up;
	case SDL_SCANCODE_LCTRL:
		return EKeyCode::KC_LeftCtrl;
	case SDL_SCANCODE_LSHIFT:
		return EKeyCode::KC_LeftShift;
	case SDL_SCANCODE_LALT:
		return EKeyCode::KC_LeftAlt;
	case SDL_SCANCODE_LGUI:
		return EKeyCode::KC_LeftGui;
	case SDL_SCANCODE_RCTRL:
		return EKeyCode::KC_RightCtrl;
	case SDL_SCANCODE_RSHIFT:
		return EKeyCode::KC_RightShift;
	case SDL_SCANCODE_RALT:
		return EKeyCode::KC_RightAlt;
	case SDL_SCANCODE_RGUI:
		return EKeyCode::KC_RightGui;
	case SDL_SCANCODE_APPLICATION:
		return EKeyCode::KC_Application;
	}

	return EKeyCode::KC_Unknown;
}

const char* DInputStack::KeyCodeToString(EKeyCode KeyCode)
{
	switch (KeyCode)
	{
	case EKeyCode::KC_A:
		return "A";
	case EKeyCode::KC_B:
		return "B";
	case EKeyCode::KC_C:
		return "C";
	case EKeyCode::KC_D:
		return "D";
	case EKeyCode::KC_E:
		return "E";
	case EKeyCode::KC_F:
		return "F";
	case EKeyCode::KC_G:
		return "G";
	case EKeyCode::KC_H:
		return "H";
	case EKeyCode::KC_I:
		return "I";
	case EKeyCode::KC_J:
		return "J";
	case EKeyCode::KC_K:
		return "K";
	case EKeyCode::KC_L:
		return "L";
	case EKeyCode::KC_M:
		return "M";
	case EKeyCode::KC_N:
		return "N";
	case EKeyCode::KC_O:
		return "O";
	case EKeyCode::KC_P:
		return "P";
	case EKeyCode::KC_Q:
		return "Q";
	case EKeyCode::KC_R:
		return "R";
	case EKeyCode::KC_S:
		return "S";
	case EKeyCode::KC_T:
		return "T";
	case EKeyCode::KC_U:
		return "U";
	case EKeyCode::KC_V:
		return "V";
	case EKeyCode::KC_W:
		return "W";
	case EKeyCode::KC_X:
		return "X";
	case EKeyCode::KC_Y:
		return "Y";
	case EKeyCode::KC_Z:
		return "Z";
	case EKeyCode::KC_0:
		return "0";
	case EKeyCode::KC_1:
		return "1";
	case EKeyCode::KC_2:
		return "2";
	case EKeyCode::KC_3:
		return "3";
	case EKeyCode::KC_4:
		return "4";
	case EKeyCode::KC_5:
		return "5";
	case EKeyCode::KC_6:
		return "6";
	case EKeyCode::KC_7:
		return "7";
	case EKeyCode::KC_8:
		return "8";
	case EKeyCode::KC_9:
		return "9";
	case EKeyCode::KC_Return:
		return "Return";
	case EKeyCode::KC_Escape:
		return "Escape";
	case EKeyCode::KC_Backspace:
		return "Backspace";
	case EKeyCode::KC_Tab:
		return "Tab";
	case EKeyCode::KC_Space:
		return "Space";
	case EKeyCode::KC_Minus:
		return "Minus";
	case EKeyCode::KC_Equals:
		return "Equals";
	case EKeyCode::KC_LeftBracket:
		return "Left Bracket";
	case EKeyCode::KC_RightBracket:
		return "Right Bracket";
	case EKeyCode::KC_BackSlash:
		return "Back Slash";
	case EKeyCode::KC_SemiColon:
		return "Semi Colon";
	case EKeyCode::KC_Apostrophe:
		return "Apostrophe";
	case EKeyCode::KC_Grave:
		return "Grave";
	case EKeyCode::KC_Comma:
		return "Comma";
	case EKeyCode::KC_Period:
		return "Period";
	case EKeyCode::KC_Slash:
		return "Slash";
	case EKeyCode::KC_CapsLock:
		return "Caps Lock";
	case EKeyCode::KC_F1:
		return "F1";
	case EKeyCode::KC_F2:
		return "F2";
	case EKeyCode::KC_F3:
		return "F3";
	case EKeyCode::KC_F4:
		return "F4";
	case EKeyCode::KC_F5:
		return "F5";
	case EKeyCode::KC_F6:
		return "F6";
	case EKeyCode::KC_F7:
		return "F7";
	case EKeyCode::KC_F8:
		return "F8";
	case EKeyCode::KC_F9:
		return "F9";
	case EKeyCode::KC_F10:
		return "F10";
	case EKeyCode::KC_F11:
		return "F11";
	case EKeyCode::KC_F12:
		return "F12";
	case EKeyCode::KC_PrintScreen:
		return "Print Screen";
	case EKeyCode::KC_ScrollLock:
		return "Scroll Lock";
	case EKeyCode::KC_Pause:
		return "Pause";
	case EKeyCode::KC_Insert:
		return "Insert";
	case EKeyCode::KC_Home:
		return "Home";
	case EKeyCode::KC_PageUp:
		return "Page Up";
	case EKeyCode::KC_Delete:
		return "Delete";
	case EKeyCode::KC_End:
		return "End";
	case EKeyCode::KC_PageDown:
		return "Page Down";
	case EKeyCode::KC_Right:
		return "Right Arrow";
	case EKeyCode::KC_Left:
		return "Left Arrow";
	case EKeyCode::KC_Down:
		return "Down Arrow";
	case EKeyCode::KC_Up:
		return "Up Arrow";
	case EKeyCode::KC_LeftCtrl:
		return "Left Control";
	case EKeyCode::KC_LeftShift:
		return "Left Shift";
	case EKeyCode::KC_LeftAlt:
		return "Left Alt";
	case EKeyCode::KC_LeftGui:
		return "Left Gui";
	case EKeyCode::KC_RightCtrl:
		return "Right Control";
	case EKeyCode::KC_RightShift:
		return "Right Shift";
	case EKeyCode::KC_RightAlt:
		return "Right Alt";
	case EKeyCode::KC_RightGui:
		return "Right Gui";
	case EKeyCode::KC_Application:
		return "Application";
	default:
		return "Unknown";
	}
}

SKeyEvent DInputStack::FromSDLEvent(SDL_Event& Event)
{
	SDL_Keymod Keymod = SDL_GetModState();
	switch (Event.type)
	{
		case SDL_KEYDOWN:
		case SDL_KEYUP:
			SKeyEvent OutEvent;

			OutEvent.KeyCode = FromSDLScanCode(Event.key.keysym.scancode);

			OutEvent.bLeftAltHeld = Event.key.keysym.mod & SDL_Keymod::KMOD_LALT;
			OutEvent.bRightAltHeld = Event.key.keysym.mod & SDL_Keymod::KMOD_RALT;
			OutEvent.bLeftCtrlHeld = Event.key.keysym.mod & SDL_Keymod::KMOD_LCTRL;
			OutEvent.bRightCtrlHeld = Event.key.keysym.mod & SDL_Keymod::KMOD_RCTRL;
			OutEvent.bLeftShiftHeld = Event.key.keysym.mod & SDL_Keymod::KMOD_LSHIFT;
			OutEvent.bRightShiftHeld = Event.key.keysym.mod & SDL_Keymod::KMOD_RSHIFT;
			OutEvent.bLeftGuiHeld = Event.key.keysym.mod & SDL_Keymod::KMOD_LGUI;
			OutEvent.bRightGuiHeld = Event.key.keysym.mod & SDL_Keymod::KMOD_RGUI;
			OutEvent.bNumLock = Event.key.keysym.mod & SDL_Keymod::KMOD_NUM;
			OutEvent.bCapsLock = Event.key.keysym.mod & SDL_Keymod::KMOD_CAPS;
			OutEvent.bPressed = Event.type == SDL_KEYDOWN;
			OutEvent.bReleased = !OutEvent.bPressed;
			OutEvent.bInputConsumed = false;
			OutEvent.bIsRepeat = Event.key.repeat;

			return OutEvent;
		default:
			Check(false);
			return SKeyEvent();
	}
}

void DInputStack::BroadcastKeyEvent(SKeyEvent KeyEvent)
{
	for (DReturnEvent<SInputHandleResult, const SKeyEvent&>& Callback : KeyListeners)
	{
		SInputHandleResult Result = Callback.Invoke(KeyEvent);
		if (Result.bConsumeInput)
		{
			KeyEvent.bInputConsumed = true;
		}
	}
}
