#include <iostream>
#include "DiabolicalEngine.h"
#include "Logging/Logging.h"
#include "World/Object.h"
#include "World/World.h"
#include "Online/NetworkManager.h"
#include "Thread/GameThread.h"
#include "Thread/Thread.h"
#include "Types/BitStream.h"
#include "Types/Map.h"
#include "Online/Server.h"
#include "Online/RPC.h"
#include "Graphics/WindowSDL.h"
#include "World/SceneObject.h"
#include "World/SceneComponent.h"
#include "Graphics/Rendering/CameraComponent.h"
#include "Graphics/Rendering/MeshComponent.h"
#include "Game/TinyRPGGameMode.h"
#include "Graphics/Window.h"
#include "Graphics/Rendering/PointLightComponent.h"
#include "Graphics/Rendering/DirectionalLightComponent.h"

int main(int argc, char* argv[])
{
	DEngine::InitAll(argc, argv);

	DObjectPtr<DWorld> GameWorld;
	DVector<DObjectPtr<DSceneObject>> MeshObjects;
	DObjectPtr<DWindow> GameWindow = DObjectPtr<DWindow>();
	DObjectPtr<DTinyRPGGameMode> GameMode = DObjectPtr<DTinyRPGGameMode>();

	if (!DNetworkManager::IsDedicatedServer())
	{
		GameWindow = DWindowSDL::MakeNew("TinyRPG", 200, 200, 1920, 1080);
		GameWindow->SetMouseCaptureMode(EMouseCaptureMode::MCM_CapturedHidden);
	}

	if (DNetworkManager::GetServer())
	{
		GameWorld = DObject::CreateNew<DWorld>("Test World");
		GameMode = DObject::CreateNew<DTinyRPGGameMode>("TinyRPG Game Mode");
		GameMode->SetGameWorld(GameWorld);

		if (!DNetworkManager::IsDedicatedServer())
		{
			// Generate a player for the server player
			GameMode->CreateNewPlayerObject();
		}

		{	// Mesh
			for (int x = 2; x < 3; x++)
			{
				for (int z = -10; z < 10; z++)
				{
					DObjectPtr<DSceneObject> MeshObject = DObject::CreateNew<DSceneObject>("Test Mesh Object");
					GameWorld->AddSceneObject(MeshObject);

					DObjectPtr<DCubeComponent> MeshComponent = DObject::CreateNew<DCubeComponent>("Test Cube Component");
					MeshObject->RegisterComponent(MeshComponent);

					STransformf MeshTransform;
					MeshTransform.Position = { x * 2, -1, -z * 5 };
					MeshTransform.Scale = SVector3f(7, 1, 5);
					MeshObject->SetTransform(MeshTransform);

					MeshObjects.Add(MeshObject);

					{
						DObjectPtr<DSceneObject> LightObject = DObject::CreateNew<DSceneObject>("Test Light Object");
						GameWorld->AddSceneObject(LightObject);

						MeshTransform.Position = MeshTransform.Position + SVector3f(-1, 0.6, 0);
						MeshTransform.Scale = SVector3f::OneVector;
						LightObject->SetTransform(MeshTransform);

						auto LightComponent = DObject::CreateNew<DPointLightComponent>("Test Light");
						LightObject->RegisterComponent(LightComponent);
						LightComponent->LightColor = SVector3f(1, 1, 0);
					}

					{
						DObjectPtr<DSceneObject> LightObject = DObject::CreateNew<DSceneObject>("Test Light Object");
						GameWorld->AddSceneObject(LightObject);

						MeshTransform.Position = MeshTransform.Position + SVector3f(3, 0.6, 0);
						MeshTransform.Scale = SVector3f::OneVector;
						LightObject->SetTransform(MeshTransform);

						auto LightComponent = DObject::CreateNew<DPointLightComponent>("Test Light");
						LightObject->RegisterComponent(LightComponent);

						LightComponent->LightColor = SVector3f(1, 0, 0);
					}
				}
			}

			for (int x = -3; x < -2; x++)
			{
				for (int z = -10; z < 10; z++)
				{
					DObjectPtr<DSceneObject> MeshObject = DObject::CreateNew<DSceneObject>("Test Mesh Object");
					GameWorld->AddSceneObject(MeshObject);

					DObjectPtr<DCubeComponent> MeshComponent = DObject::CreateNew<DCubeComponent>("Test Cube Component");
					MeshObject->RegisterComponent(MeshComponent);

					STransformf MeshTransform;
					MeshTransform.Position = { x * 2, -1, -z * 5 };
					MeshTransform.Scale = SVector3f(7, 1, 5);
					MeshObject->SetTransform(MeshTransform);

					MeshObjects.Add(MeshObject);

					{
						DObjectPtr<DSceneObject> LightObject = DObject::CreateNew<DSceneObject>("Test Light Object");
						GameWorld->AddSceneObject(LightObject);

						MeshTransform.Position = MeshTransform.Position + SVector3f(1, 0.6, 0);
						MeshTransform.Scale = SVector3f::OneVector;
						LightObject->SetTransform(MeshTransform);

						auto LightComponent = DObject::CreateNew<DPointLightComponent>("Test Light");
						LightObject->RegisterComponent(LightComponent);
						LightComponent->LightColor = SVector3f(0, 1, 0);
					}

					{
						DObjectPtr<DSceneObject> LightObject = DObject::CreateNew<DSceneObject>("Test Light Object");
						GameWorld->AddSceneObject(LightObject);

						MeshTransform.Position = MeshTransform.Position + SVector3f(-3, 0.6, 0);
						MeshTransform.Scale = SVector3f::OneVector;
						LightObject->SetTransform(MeshTransform);

						auto LightComponent = DObject::CreateNew<DPointLightComponent>("Test Light");
						LightObject->RegisterComponent(LightComponent);

						LightComponent->LightColor = SVector3f(0, 0, 1);
					}
				}
			}

			DObjectPtr<DSceneObject> LightObject = DObject::CreateNew<DSceneObject>("Directional Light Object");
			GameWorld->AddSceneObject(LightObject);

			STransformf MeshTransform = STransformf();
			MeshTransform.SetEulerRotation({ 90, 0, 0 });
			LightObject->SetTransform(MeshTransform);

			auto LightComponent = DObject::CreateNew<DDirectionalLightComponent>("Test Light");
			LightObject->RegisterComponent(LightComponent);

			LightComponent->LightColor = SVector3f(1, 1, 1);
		}
	}
	

	float Index = 0;
	while (true) 
	{
		DEngine::ManageAllTickFunctions();

		for (DObjectPtr<DSceneObject>& Object : MeshObjects)
		{
			STransformf Transform = Object->GetTransform();
			Transform.Position.Y = sin(Index);
			//Object->SetTransform(Transform);
		}
		Index += 0.05f;
	}
}
