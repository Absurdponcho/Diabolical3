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
#include "Graphics/Window.h"
#include "World/SceneObject.h"
#include "World/SceneComponent.h"
#include "Graphics/Rendering/CameraComponent.h"
#include "Graphics/Rendering/MeshComponent.h"

int main(int argc, char* argv[])
{
	DEngine::InitAll(argc, argv);
	DObjectPtr<DWorld> GameWorld;
	DVector<DObjectPtr<DSceneObject>> MeshObjects;
	DObjectPtr<DWindow> GameWindow = DObjectPtr<DWindow>();
	if (!DNetworkManager::IsDedicatedServer())
	{
		GameWindow = DWindowSDL::MakeNew("TinyRPG", 200, 200, 800, 600);
	}

	if (DNetworkManager::GetServer())
	{
		GameWorld = DObject::CreateNew<DWorld>("Test World");
		
		{	// Camera
			DObjectPtr<DSceneObject> CameraObject = DObject::CreateNew<DSceneObject>("Test Camera Object");
			GameWorld->AddSceneObject(CameraObject);

			DObjectPtr<DCameraComponent> CameraComponent = DObject::CreateNew<DCameraComponent>("Test Camera Component");
			CameraObject->RegisterComponent(CameraComponent);

			STransformf CameraTransform;
			CameraTransform.Position = { 0, 10, 10 };
			CameraTransform.Scale = SVector3f::OneVector;
			CameraObject->SetTransform(CameraTransform);
		}

		{	// Mesh
			for (int x = -4; x < 5; x++)
			{
				for (int z = 5; z < 50; z++)
				{
					DObjectPtr<DSceneObject> MeshObject = DObject::CreateNew<DSceneObject>("Test Mesh Object");
					GameWorld->AddSceneObject(MeshObject);

					DObjectPtr<DCubeComponent> MeshComponent = DObject::CreateNew<DCubeComponent>("Test Cube Component");
					MeshObject->RegisterComponent(MeshComponent);

					STransformf MeshTransform;
					MeshTransform.Position = { x * 2, 0, -z * 2 };
					MeshTransform.Scale = SVector3f::OneVector;
					MeshObject->SetTransform(MeshTransform);

					MeshObjects.Add(MeshObject);
				}
			}
		}
	}
	
	float Index = 0;
	while (true) 
	{
		DEngine::ManageAllTickFunctions();

		for (DObjectPtr<DSceneObject>& Object : MeshObjects)
		{
			STransformf Transform = Object->GetTransform();
			Transform.Position.y = sin(Index);
			Object->SetTransform(Transform);
		}
		Index += 0.05f;
	}
}
