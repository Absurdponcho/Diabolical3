#include "WindowSDL.h"
#include "Logging/Logging.h"
#include <gl/glew.h>
#include "Graphics/Rendering/CameraComponent.h"
#include "Graphics/Rendering/RenderThread.h"
#include "Input/Input.h"

bool bSDLInitialized = false;
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

bool DWindowSDL::GetWidthHeight(uint32_t& OutWidth, uint32_t& OutHeight) const
{
	if (!SdlWindow)
	{
		return false;
	}

	int W, H;
	SDL_GetWindowSize(SdlWindow, &W, &H);

	OutWidth = W;
	OutHeight = H;

	return true;
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
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3); 
			SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3); 
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

			glEnable(GL_CULL_FACE);
			glCullFace(GL_BACK);

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

void DWindowSDL::HandleSDLKeyEvent(SDL_Event& Event)
{
	SKeyEvent KeyEvent = DInputStack::KeyEventFromSDLEvent(Event);
	if (KeyEvent.bIsRepeat)
	{
		return;
	}
	InputStack.BroadcastKeyEvent(KeyEvent);
}

void DWindowSDL::HandleSDLAxisEvent(SDL_Event& Event)
{
	SAxisEvent AxisEvent = DInputStack::AxisEventFromSDLEvent(Event);
	InputStack.BroadcastAxisEvent(AxisEvent);
}

void DWindowSDL::SetMouseCaptureMode(EMouseCaptureMode InMouseCaptureMode)
{
	if (MouseCaptureMode == EMouseCaptureMode::MCM_None)
	{
		if (InMouseCaptureMode == EMouseCaptureMode::MCM_None)
		{
			return;
		}
		else if (InMouseCaptureMode == EMouseCaptureMode::MCM_CapturedHidden)
		{
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}
	}
	else if (MouseCaptureMode == EMouseCaptureMode::MCM_CapturedHidden)
	{
		if (InMouseCaptureMode == EMouseCaptureMode::MCM_CapturedHidden)
		{
			return;
		}
		else if (InMouseCaptureMode == EMouseCaptureMode::MCM_None)
		{
			SDL_SetRelativeMouseMode(SDL_FALSE);
		}
	}

	MouseCaptureMode = InMouseCaptureMode;
}

EMouseCaptureMode DWindowSDL::GetMouseCaptureMode() const
{
	return MouseCaptureMode;
}