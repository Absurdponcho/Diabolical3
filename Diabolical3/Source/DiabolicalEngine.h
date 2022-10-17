#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32) && !defined(__CYGWIN__)

#define PLATFORM_WINDOWS true

#endif

#include "Types/DMemory.h"
#include "Types/Action.h"
#include <SDL2/SDL.h>

class DEngine
{
public:
	static void InitAll(int argc, char* argv[]);
	static class DObjectManager* ObjectManager;
	static class DRenderThread* RenderThread;
	static DVector<DActionBase> GameThreadActions;
	static void ManageAllTickFunctions();

private:
};