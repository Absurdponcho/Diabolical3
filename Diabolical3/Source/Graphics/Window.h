#pragma once
#include "Types/DString.h"
#include <SDL2/SDL.h>
#include "Types/DVector.h"
#include "Types/DMemory.h"
#include "Input/Input.h"

enum class EMouseCaptureMode : uint8_t
{
	MCM_None,
	MCM_CapturedHidden,
};

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
	virtual bool GetWidthHeight(uint32_t& OutWidth, uint32_t& OutHeight) const = 0;
	DInputStack InputStack;

	virtual void SetMouseCaptureMode(EMouseCaptureMode MouseCaptureMode) = 0;
	virtual EMouseCaptureMode GetMouseCaptureMode() const = 0;

protected:
	virtual void Tick() = 0;
	virtual void Close() = 0;
	virtual bool IsPendingRemoval() const = 0;

	static DVector<DSharedPtr<DWindow>> AllWindows;
	static void WindowTick();
	DVector<DObjectPtr<DCameraComponent>> RegisteredCameras;

	friend class DEngine;
};
