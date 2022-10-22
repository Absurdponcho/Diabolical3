#pragma once
#include <stdint.h>

#include "KeyCodes.inl"
#include "SDL2/SDL.h"
#include "Types/Action.h"
#include "Types/DVector.h"

struct SAxisEvent
{
	EAxisCode AxisCode;

	float MotionX;
	float MotionY;
	float MotionZ;

	// Used when there is an absolute position for the axis, e.g. Mouse position on screen
	float AbsoluteX;
	float AbsoluteY;
	float AbsoluteZ;

	bool bInputConsumed : 1;

};

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
	static SKeyEvent KeyEventFromSDLEvent(SDL_Event& Event);
	static SAxisEvent AxisEventFromSDLEvent(SDL_Event& Event);
	static const char* KeyCodeToString(EKeyCode KeyCode);

	void BroadcastKeyEvent(SKeyEvent KeyEvent);
	void BroadcastAxisEvent(SAxisEvent KeyEvent);

	DVector<DReturnEvent<SInputHandleResult, const SKeyEvent&>> KeyListeners;
	DVector<DReturnEvent<SInputHandleResult, const SAxisEvent&>> AxisListeners;

protected:

	friend class DWindow;
};