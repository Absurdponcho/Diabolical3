#pragma once
#include "Types/DString.h"
#include <SDL2/SDL.h>
#include "Types/DVector.h"
#include "Types/DMemory.h"

class DWindow
{
public:
	virtual ~DWindow() {}

	virtual void AddCamera(DObjectPtr<class DCameraComponent> CameraComponent);
	virtual void RemoveCamera(DObjectPtr<class DCameraComponent> CameraComponent);
	virtual bool HasCamera(DObjectPtr<class DCameraComponent> CameraComponent);

	static DVector<DSharedPtr<DWindow>>& GetAllWindows()
	{
		return AllWindows;
	}

	DWeakPtr<DWindow> GetWeakThis();

	virtual float GetAspectRatio() { return 1.0f; }

protected:
	virtual void Tick() = 0;
	virtual void Close() = 0;
	virtual bool IsPendingRemoval() const = 0;

	static DVector<DSharedPtr<DWindow>> AllWindows;
	static void WindowTick();
	DVector<DObjectPtr<DCameraComponent>> RegisteredCameras;

	friend class DEngine;
};

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

protected:
	bool bWantsToClose = false;

	DWindowSDL(DString WindowTitle, int X, int Y, int Width, int Height, uint32_t WindowFlags = SDL_WINDOW_RESIZABLE | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);

	SDL_Window* SdlWindow = nullptr;
	SDL_GLContext SdlGlContext = nullptr;
};