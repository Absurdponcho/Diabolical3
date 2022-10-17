#include "Window.h"
#include "Logging/Logging.h"
#include <gl/glew.h>
#include "Graphics/Rendering/CameraComponent.h"
#include "Graphics/Rendering/RenderThread.h"

bool bSDLInitialized = false;
DVector<DSharedPtr<DWindow>> DWindow::AllWindows;

void DWindow::WindowTick()
{
	for (int Index = (int)AllWindows.Size() - 1; Index >= 0; Index--)
	{
		DSharedPtr<DWindow>& Window = AllWindows[Index];
		if (Window->IsPendingRemoval())
		{
			AllWindows.RemoveAt(Index);
			continue;
		}
		Window->Tick();
	}
}

void DWindow::AddCamera(DObjectPtr<DCameraComponent> CameraComponent)
{
	Check(!HasCamera(CameraComponent));
	if (HasCamera(CameraComponent))
	{
		return;
	}

	RegisteredCameras.Add(CameraComponent);
	LOG("Registered camera in window");
}

void DWindow::RemoveCamera(DObjectPtr<DCameraComponent> CameraComponent)
{
	for (int Index = 0; Index < RegisteredCameras.Size(); Index++)
	{
		if (CameraComponent.Get() == RegisteredCameras[Index].Get())
		{
			RegisteredCameras.RemoveAt(Index);
			return;
		}
	}
}

bool DWindow::HasCamera(DObjectPtr<DCameraComponent> CameraComponent)
{
	for (DObjectPtr<DCameraComponent>& Camera : RegisteredCameras)
	{
		if (Camera.Get() == CameraComponent.Get())
		{
			return true;
		}
	}
	return false;
}

DWeakPtr<DWindow> DWindow::GetWeakThis()
{
	for (int Index = (int)AllWindows.Size() - 1; Index >= 0; Index--)
	{
		DSharedPtr<DWindow>& Window = AllWindows[Index];
		if (Window->IsPendingRemoval())
		{
			AllWindows.RemoveAt(Index);
			continue;
		}
		if (Window.GetRaw() == this)
		{
			return Window;
		}
	}

	return DWeakPtr<DWindow>();
}

bool DWindowSDL::UsingSDL()
{
	return bSDLInitialized;
}

void DWindowSDL::Tick()
{
	Check(SdlWindow);
	if (SdlWindow)
	{
		for (int Index = (int)RegisteredCameras.Size() - 1; Index >= 0; Index--)
		{
			if (!RegisteredCameras[Index].IsValid())
			{
				RegisteredCameras.RemoveAt(Index);
				continue;
			}
			RegisteredCameras[Index]->RenderScene(GetWeakThis());
		}

		DWeakPtr<DWindowSDL> WeakThis = GetWeakThis();
		DEngine::RenderThread->Invoke(DAction<int>([WeakThis](int a) mutable
			{
				DSharedPtr<DWindowSDL> SharedThis = WeakThis.Lock();
				if (!SharedThis.IsValid())
				{
					return;
				}
				SDL_GL_SwapWindow(SharedThis->SdlWindow);
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
			}));
	}
}

DWeakPtr<DWindow> DWindowSDL::GetFromSdlId(uint32_t SdlId)
{
	for (DSharedPtr<DWindow>& Window : AllWindows)
	{
		if (DWindowSDL* WindowSdl = dynamic_cast<DWindowSDL*>(Window.GetRaw()))
		{
			if (!WindowSdl->SdlWindow) continue;
			if (SDL_GetWindowID(WindowSdl->SdlWindow))
			{
				return Window;
			}
		}
	}
	return DWeakPtr<DWindow>();
}

DWeakPtr<DWindow> DWindowSDL::MakeNew(DString WindowTitle, int X, int Y, int Width, int Height, uint32_t WindowFlags)
{
	DSharedPtr<DWindow> WindowSDL = new DWindowSDL(WindowTitle, X, Y, Width, Height, WindowFlags);
	AllWindows.Add(WindowSDL);
	return WindowSDL;
}

void DWindowSDL::Close()
{
	bWantsToClose = true;
}

DWindowSDL::DWindowSDL(DString WindowTitle, int X, int Y, int Width, int Height, uint32_t WindowFlags)
{	
	if (!bSDLInitialized)
	{
		if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
		{
			Checkf(false, "Failed to initialize SDL.");
			LOG_ERR("Failed to initialize SDL");
			return;
		}
		else
		{
			bSDLInitialized = true;
			LOG("SDL initialized successfully.");
		}
	}

	SdlWindow = SDL_CreateWindow(*WindowTitle, X, Y, Width, Height, WindowFlags);
	Check(SdlWindow);
	if (!SdlWindow)
	{
		LOG_ERR("Failed to initialize SDL window.");
		return;
	}

	DEngine::RenderThread->InvokeImmediately(DAction<int>([=](int a)
		{
			SdlGlContext = SDL_GL_CreateContext(SdlWindow);
			Check(SdlGlContext);
			if (!SdlGlContext)
			{
				LOG_ERR("Failed to initialize SDL GL context.");
				return;
			}

			LOG(DString::Format("Successfully initialized SDL window \"%s\" Size X=%i,Y=%i", *WindowTitle, Width, Height));

			int Success = SDL_GL_SetSwapInterval(-1); //try adaptive vsync
			if (Success == 0)
			{
				LOG("Adaptive VSync enabled.");
			}
			else
			{
				Success = SDL_GL_SetSwapInterval(1); //try normal vsync
				if (Success == 0)
				{
					LOG("VSync enabled.");
				}
				else
				{
					LOG_ERR("Failed to set VSync.");
				}
			}

			LOG("Initializing OpenGL");

			glewInit();
			glEnable(GL_DEPTH_TEST);
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

			LOG("OpenGL initialized.");
		}));
}

DWindowSDL::~DWindowSDL()
{
	DEngine::RenderThread->InvokeImmediately(DAction<int>([=](int a)
		{
			SDL_GL_DeleteContext(SdlGlContext);
		}));
	SDL_DestroyWindow(SdlWindow);
}

