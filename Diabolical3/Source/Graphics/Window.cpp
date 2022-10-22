#include "Window.h"
#include "Logging/Logging.h"
#include <gl/glew.h>
#include "Graphics/Rendering/CameraComponent.h"
#include "Graphics/Rendering/RenderThread.h"

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

