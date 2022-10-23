#pragma once
#include "Window.h"

class DWindowSDL : public DWindow
{
public:
	static DWeakPtr<DWindow> MakeNew(DString WindowTitle, int X, int Y, int Width, int Height, uint32_t WindowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

	virtual ~DWindowSDL();
	virtual void Tick() override;
	virtual void Close() override;
	virtual bool IsPendingRemoval() const override
	{
		return bWantsToClose;
	}

	static bool UsingSDL();
	static DWeakPtr<DWindow> GetFromSdlId(uint32_t SdlId);

	virtual void SetMouseCaptureMode(EMouseCaptureMode InMouseCaptureMode) override;
	virtual EMouseCaptureMode GetMouseCaptureMode() const override;

protected:
	EMouseCaptureMode MouseCaptureMode = EMouseCaptureMode::MCM_None;
	bool bWantsToClose = false;

	DWindowSDL(DString WindowTitle, int X, int Y, int Width, int Height, uint32_t WindowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

	SDL_Window* SdlWindow = nullptr;
	SDL_GLContext SdlGlContext = nullptr;

	void HandleSDLKeyEvent(SDL_Event& Event);
	void HandleSDLAxisEvent(SDL_Event& Event);

	friend class DEngine;
};