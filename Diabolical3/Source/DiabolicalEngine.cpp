#include <iostream>
#include "Online/Socket/DSocket.h"
#include "Online/NetworkManager.h"
#include "CommandLine/CmdLine.h"
#include "World/World.h"
#include "World/ObjectManager.h"
#include "Logging/Logging.h"
#include "Graphics/Window.h"
#include "Graphics/WindowSDL.h"
#include "AssetManager/AssetManager.h"
#include "Graphics/Rendering/Material.h"
#include "Graphics/Rendering/RenderThread.h"
#include "World/Object.h"
#include "Time/Time.h"

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
	static float LastTickTime = 0;
	float TimeSinceStart = DTime::GetSecondsSinceStart();

	STickInfo TickInfo;
	TickInfo.DeltaTime = TimeSinceStart - LastTickTime;
	LastTickTime = TimeSinceStart;

	ObjectManager->SetTicking(true);
	for (auto& Object : ObjectManager->ManagedObjects)
	{
		if (!Object.IsValid())
		{
			return;
		}
		Object->Tick(TickInfo);
	}
	ObjectManager->SetTicking(false);

	DGameThread::RunInvokedFunctions();
	DServer* Server = DNetworkManager::GetServer();
	if (Server)
	{

		// Send syncvars every 3 ticks for performance
		static int SyncVarIndex = 0;
		//if (SyncVarIndex % 3 == 0)
		//{
			Server->SendDirtySyncVars();
		//}
		SyncVarIndex++;

	}

	if (DWindowSDL::UsingSDL())
	{
		SDL_Event Event;
		while (SDL_PollEvent(&Event))
		{
			DSharedPtr<DWindowSDL> WindowSdl = DWindowSDL::GetFromSdlId(Event.window.windowID).Lock();
			if (WindowSdl.IsValid())
			{
				switch (Event.type) 
				{
				case SDL_QUIT:
					WindowSdl->Close();
					break;
				case SDL_KEYDOWN:
				case SDL_KEYUP:
					WindowSdl->HandleSDLKeyEvent(Event);
					break;
				case SDL_MOUSEMOTION:
					WindowSdl->HandleSDLAxisEvent(Event);
					break;
				}
			}
		}

		DWindow::WindowTick();

	}

	DThread::CheckManagedThreads();

	DEngine::RenderThread->SyncThreads();

	if (DNetworkManager::IsDedicatedServer())
	{
		DThread::Sleep(10);
	}
	else if (DWindow::GetAllWindows().Size() == 0)
	{
		DThread::Sleep(1);
	}
}
