#include <iostream>
#include "Online/Socket/DSocket.h"
#include "Online/NetworkManager.h"
#include "CommandLine/CmdLine.h"
#include "World/World.h"
#include "World/ObjectManager.h"
#include "Logging/Logging.h"
#include <SDL2/SDL.h>
#include "Graphics/Window.h"
#include <GL/glew.h>
#include <gl/GL.h>
#include "AssetManager/AssetManager.h"
#include "Graphics/Rendering/Material.h"
#include "Graphics/Rendering/RenderThread.h"

DObjectManager* DEngine::ObjectManager = nullptr;
DRenderThread* DEngine::RenderThread = nullptr;

void DEngine::InitAll(int argc, char* argv[])
{
	DCommandLine::Init(argc, argv);
	DSocket::InitSockets();
	DNetworkManager::Initialize();
	DAssetManager::Initialize();
	RenderThread = new DRenderThread();

	LOG(DString::Format("Registered Objects (%i):    ", DObjectRegistry::GetRegisteredObjects().Size()));
	for (auto& Pair : DObjectRegistry::GetRegisteredObjects())
	{
		DRegisteredObjectBase* RegisteredObject = Pair.second;
		LOG(DString::Format("        \"%s\", of size %i bytes", *RegisteredObject->GetObjectName(), RegisteredObject->GetObjectSize()));
	}

	ObjectManager = new DObjectManager();
}

void DEngine::ManageAllTickFunctions()
{
	DGameThread::RunInvokedFunctions();
	DServer* Server = DNetworkManager::GetServer();
	if (Server)
	{

		// Send syncvars every 3 ticks for performance
		static int SyncVarIndex = 0;
		if (SyncVarIndex % 3 == 0)
		{
			Server->SendDirtySyncVars();
		}
		SyncVarIndex++;

	}

	if (DWindowSDL::UsingSDL())
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			switch (Event.type) {
				case SDL_QUIT:
				DSharedPtr<DWindow> WindowSdl = DWindowSDL::GetFromSdlId(Event.window.windowID).Lock();
				if (WindowSdl.IsValid())
				{
					WindowSdl->Close();
				}
				break;
			}
		}

		DWindow::WindowTick();

	}

	DThread::CheckManagedThreads();

	DEngine::RenderThread->SyncThreads();

	if (Server)
	{
		DThread::Sleep(10);
	}
	else
	{
		DThread::Sleep(1);
	}
}
