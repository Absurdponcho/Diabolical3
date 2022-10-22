#pragma once
#include <stdint.h>

#include "KeyCodes.inl"
#include "SDL2/SDL.h"
#include "Types/Action.h"
#include "Types/DVector.h"


struct SKeyEvent
{
	bool bLeftShiftHeld : 1;
	bool bLeftCtrlHeld : 1;
	bool bLeftAltHeld : 1;
	bool bLeftGuiHeld : 1;
	bool bRightShiftHeld : 1;
	bool bRightCtrlHeld : 1;
	bool bRightAltHeld : 1;
	bool bRightGuiHeld : 1;
	bool bNumLock : 1;
	bool bCapsLock : 1;
	bool bInputConsumed : 1;
	bool bIsRepeat : 1;

	EKeyCode KeyCode;
	bool bPressed : 1;
	bool bReleased : 1;
	
	bool IsShiftHeld()
	{
		return bLeftShiftHeld || bRightShiftHeld;
	}
	
	bool IsCtrlHeld()
	{
		return bLeftCtrlHeld || bRightCtrlHeld;
	}
	
	bool IsAltHeld()
	{
		return bLeftAltHeld || bRightAltHeld;
	}
};

struct SInputHandleResult
{
	bool bConsumeInput = false;
};

class DInputStack
{
public:
	static EKeyCode FromSDLScanCode(uint16_t SDLScanCode);
	static SKeyEvent FromSDLEvent(SDL_Event& Event);
	static const char* KeyCodeToString(EKeyCode KeyCode);

	void BroadcastKeyEvent(SKeyEvent KeyEvent);
	DVector<DReturnEvent<SInputHandleResult, const SKeyEvent&>> KeyListeners;

protected:

	friend class DWindow;
};